#include "./simple_spritesheet.hpp"

#include <mm/opengl/shader.hpp>
#include <mm/opengl/buffer.hpp>
#include <mm/opengl/vertex_array_object.hpp>

#include <mm/fs_const_archiver.hpp>

#include <mm/engine.hpp>

#include <mm/services/scene_service_interface.hpp>
#include <entt/entity/registry.hpp>

#include <mm/components/transform2d.hpp>
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

#include <mm/logger.hpp>
#define LOG_SSSR(x) LOG("SimpleSpriteSheetRenderTask", x)

namespace MM::OpenGL::RenderTasks {

SimpleSpriteSheet::SimpleSpriteSheet(Engine& engine) {
	default_cam.setOrthographic();
	default_cam.updateView();

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

	_vertexBuffer->unbind(GL_ARRAY_BUFFER);
	_vao->unbind();

	setupShaderFiles();
	_shader = Shader::createF(engine, vertexPath, fragmentPath);
	assert(_shader != nullptr);

	auto& scene = engine.tryService<MM::Services::SceneServiceInterface>()->getScene();
	if (!scene.try_ctx<Camera3D>()) {
		LOG_SSSR("warn: scene has no Camera!");
	}
}

SimpleSpriteSheet::~SimpleSpriteSheet(void) {
}

void SimpleSpriteSheet::render(Services::OpenGLRenderer& rs, Engine& engine) {
	ZoneScopedN("MM::OpenGL::RenderTasks::SimpleSpriteSheet::render");

	rs.targets[target_fbo]->bind(FrameBufferObject::W);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	_shader->bind();
	_vertexBuffer->bind(GL_ARRAY_BUFFER);
	_vao->bind();


	auto& scene = engine.tryService<MM::Services::SceneServiceInterface>()->getScene();

	auto* cam = scene.try_ctx<Camera3D>();
	if (!cam) {
		cam = &default_cam;
	}

	auto vp = cam->getViewProjection();

	auto view = scene.view<Components::Transform2D, SpriteSheetRenderable>();

	view.each([&](auto e, Components::Transform2D& t, SpriteSheetRenderable& spr) {
		assert(spr.sp.tex); // debug

		TracyGpuZone("MM::OpenGL::Renderers::SimpleSpriteSheetRenderer::render.each");

		spr.sp.tex->bind(0);

		_shader->setUniformMat4f("_WVP", vp * t.getTransform4(t.position.y/10.f + 500.f));
		_shader->setUniform2ui("_tileCount", spr.sp.tile_count.x, spr.sp.tile_count.y);
		_shader->setUniform1ui("_atlasIndex", spr.tile_index);

		if (scene.all_of<Components::Color>(e)) {
			_shader->setUniform4f("_color", scene.get<Components::Color>(e).color);
		} else {
			_shader->setUniform4f("_color", default_color);
		}

		glDrawArrays(GL_TRIANGLES, 0, 6);
	});

	_vao->unbind();
	_vertexBuffer->unbind(GL_ARRAY_BUFFER);
	_shader->unbind();
}

void SimpleSpriteSheet::setupShaderFiles(void) {
	FS_CONST_MOUNT_FILE(vertexPath,
GLSL_VERSION_STRING
R"(
#ifdef GL_ES
	precision mediump float;
#endif

in vec2 _vertexPosition;

uniform mat4 _WVP;
uniform uvec2 _tileCount;

uniform uint _atlasIndex;

out vec2 _tex_pos;

void main() {
	// position
	gl_Position = _WVP * vec4(_vertexPosition, 0, 1);


	// uv
	uint row = _atlasIndex / _tileCount.x;
	uint column = _atlasIndex % _tileCount.x;

	_tex_pos.x = (float(column) + 0.5 + _vertexPosition.x) / float(_tileCount.x);
	_tex_pos.y = 1.0 - (float(row) + 0.5 - _vertexPosition.y) / float(_tileCount.y);

	//_tex_pos = vec2(_vertexPosition.x + 0.5, _vertexPosition.y + 0.5);
})")

	FS_CONST_MOUNT_FILE(fragmentPath,
GLSL_VERSION_STRING
R"(
#ifdef GL_ES
	precision mediump float;
#endif

uniform vec4 _color;
uniform sampler2D _tex0;

in vec2 _tex_pos;

out vec4 _out_color;

void main() {
	vec4 tmp_col = texture(_tex0, _tex_pos) * _color;

	if (tmp_col.a == 0.0) {
		discard;
	}

	//gl_FragColor = tmp_col;
	_out_color = tmp_col;
	//gl_FragColor = texture(_tex0, _tex_pos) * _color;
})")
}

} // MM::OpenGL::RenderTasks

