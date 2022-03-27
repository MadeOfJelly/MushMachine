#include "./blur.hpp"

#include <mm/opengl/shader.hpp>
#include <mm/opengl/buffer.hpp>
#include <mm/opengl/vertex_array_object.hpp>

#include <mm/fs_const_archiver.hpp>

#include <mm/engine.hpp>

#include <mm/opengl/texture.hpp>

#include <tracy/Tracy.hpp>

namespace MM::OpenGL::RenderTasks {

Blur::Blur(Engine& engine) {
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

Blur::~Blur(void) {
}

void Blur::render(Services::OpenGLRenderer& rs, Engine&) {
	ZoneScopedN("MM::OpenGL::RenderTasks::Blur::render");

	//rs.targets[io_fbo]->bind(FrameBufferObject::W);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	auto& rm_t = MM::ResourceManager<MM::OpenGL::Texture>::ref();
	auto tex_in = rm_t.get(entt::hashed_string::value(in_tex.c_str())); // TODO: perf problems
	auto tex_out = rm_t.get(entt::hashed_string::value(out_tex.c_str())); // TODO: perf problems
	auto tex_temp = rm_t.get(entt::hashed_string::value(temp_tex.c_str())); // TODO: perf problems

	_shader->bind();
	_vertexBuffer->bind(GL_ARRAY_BUFFER);
	_vao->bind();

	_shader->setUniform2f("tex_offset_factor", tex_offset_factor);

	{ // horizontal
		rs.targets[temp_fbo]->bind(FrameBufferObject::W);

		glViewport(0, 0, tex_temp->width, tex_temp->height);
		tex_in->bind(0); // read

		_shader->setUniform1i("horizontal", 1);

		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	{ // vertical
		rs.targets[out_fbo]->bind(FrameBufferObject::W);

		glViewport(0, 0, tex_out->width, tex_out->height);
		tex_temp->bind(0); // read

		_shader->setUniform1i("horizontal", 0);

		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	_vao->unbind();
}

void Blur::setupShaderFiles(void) {
	FS_CONST_MOUNT_FILE(vertexPath,
GLSL_VERSION_STRING
R"(
#ifdef GL_ES
	precision mediump float;
#endif

in vec2 _vertexPosition;
out vec2 _tex_uv;

void main() {
	gl_Position = vec4(_vertexPosition, 0, 1);
	_tex_uv = _vertexPosition * 0.5 + 0.5;
})")

	FS_CONST_MOUNT_FILE(fragmentPath,
GLSL_VERSION_STRING
R"(
#ifdef GL_ES
	precision mediump float;
#endif

uniform sampler2D tex0;
in vec2 _tex_uv;

uniform bool horizontal;
//const bool horizontal = true;
uniform vec2 tex_offset_factor;

const float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

out vec4 _out_color;

void main() {
	vec3 result = texture(tex0, _tex_uv).rgb * weight[0]; // current fragment's contribution

	vec2 tex_offset = vec2(1.0) / vec2(textureSize(tex0, 0).xy); // gets size of single texel
	tex_offset *= tex_offset_factor;

	if (horizontal) {
		for (int i = 1; i < 5; i++) {
			result += texture(tex0, _tex_uv + vec2(tex_offset.x * float(i), 0.0)).rgb * weight[i];
			result += texture(tex0, _tex_uv - vec2(tex_offset.x * float(i), 0.0)).rgb * weight[i];
		}
	} else {
		for (int i = 1; i < 5; i++) {
			result += texture(tex0, _tex_uv + vec2(0.0, tex_offset.y * float(i))).rgb * weight[i];
			result += texture(tex0, _tex_uv - vec2(0.0, tex_offset.y * float(i))).rgb * weight[i];
		}
	}

	_out_color = vec4(result, 1.0);
})")
}

} // MM::OpenGL::RenderTasks

