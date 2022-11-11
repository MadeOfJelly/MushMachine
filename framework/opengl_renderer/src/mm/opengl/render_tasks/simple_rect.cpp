#include "./simple_rect.hpp"

#include <mm/opengl/shader.hpp>
#include <mm/opengl/buffer.hpp>
#include <mm/opengl/vertex_array_object.hpp>

#include <mm/fs_const_archiver.hpp>

#include <mm/engine.hpp>

#include <mm/services/scene_service_interface.hpp>
#include <entt/entity/registry.hpp>

#include <mm/components/transform4x4.hpp>
#include <mm/components/color.hpp>

#include <tracy/Tracy.hpp>

#include <mm/logger.hpp>
#define LOG_SRR(x) LOG("SimpleRectRenderTask", x)

namespace MM::OpenGL::RenderTasks {

SimpleRect::SimpleRect(Engine& engine) {
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

SimpleRect::~SimpleRect(void) {
}

void SimpleRect::render(Services::OpenGLRenderer& rs, Engine& engine) {
	ZoneScopedN("MM::OpenGL::RenderTasks::SimpleRect::render");

	auto* ssi = engine.tryService<MM::Services::SceneServiceInterface>();
	if (ssi == nullptr) {
		return; // nothing to draw
	}

	auto& scene = ssi->getScene();

	if (!scene.ctx().contains<Camera3D>()) {
		return; // nothing to draw
	}

	rs.targets[target_fbo]->bind(FrameBufferObject::RW);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	_shader->bind();
	_vao->bind();

	Camera3D& cam = scene.ctx().get<Camera3D>();
	auto vp = cam.getViewProjection();

	scene.view<const Components::Transform4x4>().each([this, &scene, &vp](entt::entity e, const auto& t) {
		_shader->setUniformMat4f("_WVP", vp * t.trans);

		if (scene.all_of<Components::Color>(e)) {
			_shader->setUniform4f("_color", scene.get<Components::Color>(e).color);
		} else {
			_shader->setUniform4f("_color", default_color);
		}

		glDrawArrays(GL_TRIANGLES, 0, 6);
	});

	_vao->unbind();
	_shader->unbind();
}

void SimpleRect::setupShaderFiles(void) {
	FS_CONST_MOUNT_FILE(vertexPath,
GLSL_VERSION_STRING
R"(
#ifdef GL_ES
	precision mediump float;
#endif

in vec2 _vertexPosition;
uniform mat4 _WVP;

void main() {
	gl_Position = _WVP * vec4(_vertexPosition, 0.0, 1.0);
})")

	FS_CONST_MOUNT_FILE(fragmentPath,
GLSL_VERSION_STRING
R"(
#ifdef GL_ES
	precision mediump float;
#endif

uniform vec4 _color;

out vec4 _out_color;

void main() {
	_out_color = _color;
	//gl_FragColor = vec4(vec3(gl_FragCoord.z), 1.0);
})")
}

} // MM::OpenGL::RenderTasks

