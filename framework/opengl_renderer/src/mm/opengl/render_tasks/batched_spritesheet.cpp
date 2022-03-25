#include "./batched_spritesheet.hpp"

#include <mm/opengl/shader.hpp>
#include <mm/opengl/buffer.hpp>
#include <mm/opengl/vertex_array_object.hpp>

#include <mm/fs_const_archiver.hpp>

#include <mm/engine.hpp>

#include <mm/services/scene_service_interface.hpp>
#include <entt/entity/registry.hpp>

#include <mm/components/transform4x4.hpp>
#include <mm/components/color.hpp>
#include "./spritesheet_renderable.hpp"

#include <tracy/Tracy.hpp>
#ifndef MM_OPENGL_3_GLES
	#include <tracy/TracyOpenGL.hpp>
#else
	#define TracyGpuContext
	#define TracyGpuCollect
	#define TracyGpuZone(...)
#endif

namespace MM::OpenGL::RenderTasks {

BatchedSpriteSheet::BatchedSpriteSheet(Engine& engine) {
	float vertices[] = {
		-0.5f, 0.5f,
		-0.5f, -0.5f,
		0.5f, -0.5f,
		0.5f, -0.5f,
		0.5f, 0.5f,
		-0.5f, 0.5f,
	};

	_vertexBuffer = std::make_unique<Buffer>(vertices, 2 * 6 * sizeof(float), GL_STATIC_DRAW);
	_vao = std::make_unique<VertexArrayObject>();
	_vao->bind();
	_vertexBuffer->bind(GL_ARRAY_BUFFER);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);

	gl_inst_buffer = std::make_unique<MM::OpenGL::InstanceBuffer<gl_instance_data>>();

	_vertexBuffer->unbind(GL_ARRAY_BUFFER);
	_vao->unbind();

	setupShaderFiles();
	_shader = Shader::createF(engine, vertexPath, fragmentPath);
	assert(_shader != nullptr);
}

BatchedSpriteSheet::~BatchedSpriteSheet(void) {
}

void BatchedSpriteSheet::render(Services::OpenGLRenderer& rs, Engine& engine) {
	ZoneScopedN("MM::OpenGL::RenderTasks::BatchedSpriteSheet::render");

	auto* ssi = engine.tryService<MM::Services::SceneServiceInterface>();
	if (ssi == nullptr) {
		return; // nothing to draw
	}

	auto& scene = ssi->getScene();

	if (!scene.ctx().contains<Camera3D>()) {
		return; // nothing to draw
	}

	struct sp_data {
		SpriteSheet sp;
		struct instance_data {
			const MM::Components::Transform4x4* trans = nullptr;
			glm::vec4* color = nullptr;
			uint32_t tile_index = 0;
		};
		std::vector<instance_data> instances;
	};
	// HACK: assume same sp for same texture
	std::unordered_map<MM::OpenGL::Texture::handle_t, sp_data> batch_map;

	scene.view<const Components::Transform4x4, SpriteSheetRenderable>().each([this, &scene, &batch_map](entt::entity e, const auto& t, auto& spr) {
		//// if off screen, early out
		//if (false) { // TODO:
			//return;
		//}

		assert(spr.sp.tex); // debug

		// first insert
		if (!batch_map.count(spr.sp.tex)) {
			auto& sp_ent = batch_map[spr.sp.tex];
			sp_ent.sp = spr.sp;
		}

		auto* tmp_col_ptr = &default_color;
		if (scene.all_of<Components::Color>(e)) {
			tmp_col_ptr = &scene.get<Components::Color>(e).color;
		}

		auto& tmp_new_ent = batch_map[spr.sp.tex].instances.emplace_back();
		tmp_new_ent.trans = &t;
		tmp_new_ent.tile_index = spr.tile_index;
		tmp_new_ent.color = tmp_col_ptr;
	});

	rs.targets[target_fbo]->bind(FrameBufferObject::W);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	_shader->bind();
	_vertexBuffer->bind(GL_ARRAY_BUFFER);
	_vao->bind();

	auto& cam = scene.ctx().at<Camera3D>();

	auto vp = cam.getViewProjection();
	_shader->setUniformMat4f("_VP", vp);

	for (auto& sp_ent : batch_map) {
		TracyGpuZone("MM::OpenGL::RenderTasks::BatchedSpriteSheet::render::sprite_entry");
		sp_ent.second.sp.tex->bind(0);

		_shader->setUniform2ui("_tileCount", sp_ent.second.sp.tile_count.x, sp_ent.second.sp.tile_count.y);

		auto* inst_memory = gl_inst_buffer->map(sp_ent.second.instances.size(), GL_DYNAMIC_DRAW);
		for (auto& inst : sp_ent.second.instances) {
			//inst_memory->pos_trans = inst.trans->getTransform4(inst.trans->position.y/10.f + 500.f);
			inst_memory->pos_trans = inst.trans->trans; // TODO: this is ugly
			inst_memory->color = *inst.color;
			inst_memory->tile_index = inst.tile_index;

			inst_memory++;
		}
		gl_inst_buffer->unmap();

		static_assert(std::is_standard_layout<gl_instance_data>::value); // check if offsetof() is usable

		// TODO: optimize, dont call attrib pointer each draw

		// mat4, oof
		// attptr 1-4
		for (size_t i = 0; i < 4; i++) {
			glVertexAttribPointer(
				1 + i,
				4,
				GL_FLOAT, GL_FALSE,
				sizeof(gl_instance_data),
				(void*) (offsetof(gl_instance_data, pos_trans) + sizeof(glm::mat4::row_type) * i)
			);
			glVertexAttribDivisor(1 + i, 1);
			glEnableVertexAttribArray(1 + i);
		}


		glVertexAttribIPointer(5, 1, GL_UNSIGNED_INT, sizeof(gl_instance_data), (void*) offsetof(gl_instance_data, tile_index));
		glVertexAttribDivisor(5, 1);
		glEnableVertexAttribArray(5);

		glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, sizeof(gl_instance_data), (void*) offsetof(gl_instance_data, color));
		glVertexAttribDivisor(6, 1);
		glEnableVertexAttribArray(6);

		glDrawArraysInstanced(GL_TRIANGLES, 0, 6, sp_ent.second.instances.size());
	}

	_vao->unbind();
	_vertexBuffer->unbind(GL_ARRAY_BUFFER);
	_shader->unbind();
}

void BatchedSpriteSheet::setupShaderFiles(void) {
	FS_CONST_MOUNT_FILE(vertexPath,
GLSL_VERSION_STRING
R"(
#ifdef GL_ES
	precision mediump float;
#endif

uniform mat4 _VP;
uniform uvec2 _tileCount;

layout(location = 0) in vec2 _vertexPosition;
layout(location = 1) in mat4 _pos_trans;
layout(location = 5) in uint _atlasIndex;
layout(location = 6) in vec4 _color;

out vec2 _tex_pos;
out vec4 _tex_color;

void main() {
	// fwd
	_tex_color = _color;

	// position
	gl_Position = _VP * _pos_trans * vec4(_vertexPosition, 0, 1);


	// uv
	uint row = _atlasIndex / _tileCount.x;
	uint column = _atlasIndex % _tileCount.x;

	_tex_pos.x = (float(column) + 0.5 + _vertexPosition.x) / float(_tileCount.x);
	_tex_pos.y = 1.0 - (float(row) + 0.5 - _vertexPosition.y) / float(_tileCount.y);
})")

	FS_CONST_MOUNT_FILE(fragmentPath,
GLSL_VERSION_STRING
R"(
#ifdef GL_ES
	precision mediump float;
#endif

uniform sampler2D _tex0;

in vec2 _tex_pos;
in vec4 _tex_color;

out vec4 _out_color;

void main() {
	vec4 tmp_col = texture(_tex0, _tex_pos) * _tex_color;

	if (tmp_col.a == 0.0) {
		discard;
	}

	_out_color = tmp_col;
})")
}

} // MM::OpenGL::RenderTasks

