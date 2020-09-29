#include "./copy_to_fb.hpp"

#include <mm/opengl/shader.hpp>
#include <mm/opengl/buffer.hpp>
#include <mm/opengl/vertex_array_object.hpp>

#include <mm/fs_const_archiver.hpp>

#include <mm/engine.hpp>

#include <mm/opengl/texture.hpp>

#include <tracy/Tracy.hpp>

namespace MM::OpenGL::RenderTasks {

CopyToFB::CopyToFB(Engine& engine) {
	float vertices[] = {
		-1.f, 1.f,
		-1.f, -1.f,
		1.f, -1.f,
		1.f, -1.f,
		1.f, 1.f,
		-1.f, 1.f,
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

CopyToFB::~CopyToFB(void) {
}

void CopyToFB::render(Services::OpenGLRenderer& rs, Engine&) {
	ZoneScopedN("MM::OpenGL::RenderTasks::CopyToFB::render");

	rs.targets[target_fbo]->bind(FrameBufferObject::W);
	glDisable(GL_DEPTH_TEST);

	_shader->bind();
	_vertexBuffer->bind(GL_ARRAY_BUFFER);
	_vao->bind();

	auto& rm = MM::ResourceManager<MM::OpenGL::Texture>::ref();

	auto tex = rm.get(entt::hashed_string::value(src_tex.c_str()));
	tex->bind(0);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	_vao->unbind();
	_vertexBuffer->unbind(GL_ARRAY_BUFFER);
	_shader->unbind();
}

void CopyToFB::reloadShaders(MM::Engine& engine) {
	auto tmp_shader = Shader::createF(engine, vertexPath, fragmentPath);
	if (tmp_shader) {
		_shader = tmp_shader;
	}
}

void CopyToFB::setupShaderFiles(void) {
	FS_CONST_MOUNT_FILE(vertexPath,
GLSL_VERSION_STRING
R"(
#ifdef GL_ES
	precision mediump float;
#endif

in vec2 _vertexPosition;
out vec2 _tex;

void main() {
	gl_Position = vec4(_vertexPosition, 0, 1);
	_tex = vec2(_vertexPosition.x * 0.5 + 0.5, _vertexPosition.y * 0.5 + 0.5);
})")

	FS_CONST_MOUNT_FILE(fragmentPath,
GLSL_VERSION_STRING
R"(
#ifdef GL_ES
	precision mediump float;
#endif

uniform sampler2D tex0;

in vec2 _tex;

out vec4 _out_color;

void main() {
	_out_color = texture(tex0, _tex);
})")
}

} // MM::OpenGL::RenderTasks

