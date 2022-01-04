#include "./simple_sprite.hpp"

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

#include <tracy/Tracy.hpp>
#ifndef MM_OPENGL_3_GLES
	#include <tracy/TracyOpenGL.hpp>
#else
	#define TracyGpuContext
	#define TracyGpuCollect
	#define TracyGpuZone(...)
#endif


namespace MM::OpenGL::RenderTasks {

SimpleSprite::SimpleSprite(Engine& engine) {
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
}

SimpleSprite::~SimpleSprite(void) {
}

void SimpleSprite::render(Services::OpenGLRenderer& rs, Engine& engine) {
	ZoneScopedN("MM::OpenGL::RenderTasks::SimpleSprite::render");

	auto* ssi = engine.tryService<MM::Services::SceneServiceInterface>();
	if (ssi == nullptr) {
		return; // nothing to draw
	}

	auto& scene = ssi->getScene();

	rs.targets[target_fbo]->bind(FrameBufferObject::W);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	_shader->bind();
	_vertexBuffer->bind(GL_ARRAY_BUFFER);
	_vao->bind();


	auto* cam = scene.try_ctx<Camera3D>();
	if (!cam) {
		cam = &default_cam;
	}

	auto vp = cam->getViewProjection();

	scene.view<const Components::Transform4x4, Components::OpenGL::Texture>().each([this, &scene, &vp](entt::entity e, const auto& t, auto& tex) {
		assert(tex.tex); // debug

		tex.tex->bind(0);

		_shader->setUniformMat4f("_WVP", vp * t.trans);

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

void SimpleSprite::setupShaderFiles(void) {
	FS_CONST_MOUNT_FILE(vertexPath,
GLSL_VERSION_STRING
R"(
#ifdef GL_ES
	precision mediump float;
#endif

in vec2 _vertexPosition;
uniform mat4 _WVP;

out vec2 _tex_pos;

void main() {
	gl_Position = _WVP * vec4(_vertexPosition, 0, 1);
	_tex_pos = vec2(_vertexPosition.x + 0.5, _vertexPosition.y + 0.5);
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

