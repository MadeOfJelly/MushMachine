#include <mm/opengl/render_tasks/tilemap.hpp>

#include <mm/opengl/shader.hpp>
#include <mm/opengl/buffer.hpp>
#include <mm/opengl/vertex_array_object.hpp>

#include <mm/fs_const_archiver.hpp>

#include <mm/engine.hpp>

#include <mm/services/scene_service_interface.hpp>
#include <entt/entity/registry.hpp>

#include <mm/components/transform4x4.hpp>
#include <mm/opengl/components/texture.hpp>
#include <mm/components/color.hpp>

#include <mm/opengl/camera_3d.hpp>

#include "./tilemap_renderable.hpp"

#include <tracy/Tracy.hpp>
#ifndef MM_OPENGL_3_GLES
	#include <tracy/TracyOpenGL.hpp>
#else
	#define TracyGpuContext
	#define TracyGpuCollect
	#define TracyGpuZone(...)
#endif

#include <mm/logger.hpp>
#define LOG_SSR(x) LOG("TilemapRenderTask", x)

namespace MM::OpenGL::RenderTasks {

Tilemap::Tilemap(MM::Engine& engine) {
	float vertices[] = {
		-0.5f, 0.5f,
		-0.5f, -0.5f,
		0.5f, -0.5f,
		0.5f, -0.5f,
		0.5f, 0.5f,
		-0.5f, 0.5f,
	};

	// bias to avoid cracks
	const float vert_offset_fact = 1.001f;
	for (size_t i = 0; i < 2 * 6; i++) {
		vertices[i] *= vert_offset_fact;
	}

	_vertexBuffer = std::make_unique<MM::OpenGL::Buffer>(vertices, 2 * 6 * sizeof(float), GL_STATIC_DRAW);
	_vao = std::make_unique<MM::OpenGL::VertexArrayObject>();
	_vao->bind();
	_vertexBuffer->bind(GL_ARRAY_BUFFER);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);

	glEnableVertexAttribArray(1); // Tilemap::Tile::pos
	glEnableVertexAttribArray(2); // Tilemap::Tile::atlas_index
	//glEnableVertexAttribArray(3); // Tilemap::Tile::color

	_vertexBuffer->unbind(GL_ARRAY_BUFFER);
	_vao->unbind();

	setupShaderFiles();
	_shader = MM::OpenGL::Shader::createF(engine, vertexPath, fragmentPath);
	assert(_shader != nullptr);
}

Tilemap::~Tilemap(void) {
}

void Tilemap::render(MM::Services::OpenGLRenderer& rs, MM::Engine& engine) {
	ZoneScopedN("MM::OpenGL::Renderers::TilemapRenderer::render");

	auto* ssi = engine.tryService<MM::Services::SceneServiceInterface>();
	if (ssi == nullptr) {
		return; // nothing to draw
	}

	auto& scene = ssi->getScene();

	if (!scene.ctx().contains<Camera3D>()) {
		return; // nothing to draw
	}

	rs.targets[target_fbo]->bind(MM::OpenGL::FrameBufferObject::W);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDisable(GL_BLEND);

	_shader->bind();
	_vertexBuffer->bind(GL_ARRAY_BUFFER);
	_vao->bind();

	MM::OpenGL::Camera3D& cam = scene.ctx().at<MM::OpenGL::Camera3D>();
	auto vp = cam.getViewProjection();

	_shader->setUniform3f("_ambient_color", ambient_color);

	scene.view<MM::Components::Transform4x4, OpenGL::TilemapRenderable>()
		.each([&](auto, MM::Components::Transform4x4& t, OpenGL::TilemapRenderable& tilemap) {
		_shader->setUniformMat4f("_WVP", vp * t.trans);

		// for each sprite layer
		for (auto& sp_layer : tilemap.sprite_layer) {
			assert(sp_layer.sprite_sheet.tex); // debug
			assert(sp_layer.map_buffer);
			assert(sp_layer.map_buffer->getSize());

			sp_layer.sprite_sheet.tex->bind(0);

			_shader->setUniform2ui("_tileCount", sp_layer.sprite_sheet.tile_count.x, sp_layer.sprite_sheet.tile_count.y);

			// renderj
			{
				TracyGpuZone("MM::OpenGL::Renderers::TilemapRenderer::render.each_tilemap.each_sprite_layer");

				sp_layer.map_buffer->bind();
				OpenGL::TilemapRenderable::Tile::setupGLBindings();

				glVertexAttribDivisor(1, 1);
				glVertexAttribDivisor(2, 1);
				//glVertexAttribDivisor(3, 1);

				glDrawArraysInstanced(GL_TRIANGLES, 0, 6, sp_layer.map_buffer->getSize());
			}
		}
	});

	_vao->unbind();
	_vertexBuffer->unbind(GL_ARRAY_BUFFER);
	_shader->unbind();
}

void Tilemap::setupShaderFiles(void) {
	FS_CONST_MOUNT_FILE(vertexPath,
GLSL_VERSION_STRING
R"(

uniform mat4 _WVP;
uniform uvec2 _tileCount;

layout(location = 0) in vec2 _vertexPosition;
layout(location = 1) in vec2 _tPos;
layout(location = 2) in uint _atlasIndex;
//layout(location = 3) in vec3 _tColor;

//out vec4 _color;
centroid out vec2 _tex_pos;

void main() {
	//gl_Position = vec4(_WVP * vec3(_vertexPosition, 1), 1);
	//mat3 tmp = mat3(_WVP[0], _WVP[1], vec3(_WVP[2][0] * _tPos.x, _WVP[2][1] * _tPos.y, _WVP[2][2]));
	//gl_Position = vec4(tmp * vec3(_vertexPosition, 1), 1);

	mat4 tmp = mat4(1.0);
	tmp[3] = vec4(_tPos.x, -_tPos.y, -_tPos.y / 10.f, 1);
	gl_Position = (_WVP * tmp) * vec4(_vertexPosition, 0, 1);


	//_color = vec4(_tColor, 1.0);

	uint row = _atlasIndex / _tileCount.x;
	uint column = _atlasIndex % _tileCount.x;

	//_tex_pos = vec2(_vertexPosition.x + 0.5, _vertexPosition.y + 0.5);
	//_tex_pos.x = _vertexPosition.x + 0.5;
	_tex_pos.x = (float(column) + 0.5 + _vertexPosition.x) / float(_tileCount.x);

	_tex_pos.y = 1.0 - (float(row) + 0.5 - _vertexPosition.y) / float(_tileCount.y);
	//_tex_pos.y = _vertexPosition.y + 0.5;
}
)")

	FS_CONST_MOUNT_FILE(fragmentPath,
GLSL_VERSION_STRING
R"(
#ifdef GL_ES
	precision mediump float;
#endif

uniform sampler2D _tex0;

//in vec4 _color;
uniform vec3 _ambient_color;
// centroid until i switch from atlas to array
centroid in vec2 _tex_pos;

out vec4 _out_color;


bool should_discard(vec2 screen_pos, float alpha) {
	const int dither[8 * 8] = int[8 * 8](
		 0, 32,  8, 40,  2, 34, 10, 42,
		48, 16, 56, 24, 50, 18, 58, 26,
		12, 44,  4, 36, 14, 46,  6, 38,
		60, 28, 52, 20, 62, 30, 54, 22,
		 3, 35, 11, 43,  1, 33,  9, 41,
		51, 19, 59, 27, 49, 17, 57, 25,
		15, 47,  7, 39, 13, 45,  5, 37,
		63, 31, 55, 23, 61, 29, 53, 21
	);

	int mat_value = dither[int(screen_pos.x)%8 + 8 * (int(screen_pos.y)%8)];
	float n_mat_value = float(mat_value) * (1.0/64.0);

	return n_mat_value < 1.0 - alpha;

	//return mix(n_mat_value, 1.0, alpha) < 0.5;
	//return n_mat_value * alpha < 0.5;
	//return alpha == 0.0;
}

void main() {
	vec4 tmp_col = texture(_tex0, _tex_pos);

	//if (tmp_col.a == 0.0) {
	if (should_discard(gl_FragCoord.xy, tmp_col.a)) {
		discard;
	}

	//_out_color = tmp_col;
	//_out_color = vec4(tmp_col.rgb * _ambient_color, tmp_col.a);
	_out_color = vec4(tmp_col.rgb * _ambient_color, 1.0);
}
)")
}

} // MM::OpenGL::RenderTasks

