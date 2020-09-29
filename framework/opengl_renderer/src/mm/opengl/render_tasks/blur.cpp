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

	rs.targets[target_fbo]->bind(FrameBufferObject::W);
	glDisable(GL_DEPTH_TEST);

	_shader->bind();
	_vertexBuffer->bind(GL_ARRAY_BUFFER);
	_vao->bind();

	rs.targets[src_fbo]->bind(FrameBufferObject::W);
	{
		_shader->setUniform1i("horizontal", 1);
		rs.targets[src_fbo]->_texAttachments.front()->bind(0);

		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	rs.targets[target_fbo]->bind(FrameBufferObject::W);
	{
		_shader->setUniform1i("horizontal", 0);

		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	_vao->unbind();
	_vertexBuffer->unbind(GL_ARRAY_BUFFER);
	_shader->unbind();

	rs.targets[src_fbo]->clear(0.f, 0.f, 0.f, 1.f);
}

void Blur::setupShaderFiles(void) {
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

uniform bool horizontal;

const float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

out vec4 _out_color;

void main() {
	vec2 tex_offset = vec2(1.0) / vec2(textureSize(tex0, 0)); // gets size of single texel
	vec3 result = texture(tex0, _tex).rgb * weight[0]; // current fragment's contribution

	if (horizontal) {
		for (int i = 1; i < 5; i++) {
			result += texture(tex0, _tex + vec2(tex_offset.x * float(i), 0.0)).rgb * weight[i];
			result += texture(tex0, _tex - vec2(tex_offset.x * float(i), 0.0)).rgb * weight[i];
		}
	} else {
		for (int i = 1; i < 5; i++) {
			result += texture(tex0, _tex + vec2(0.0, tex_offset.y * float(i))).rgb * weight[i];
			result += texture(tex0, _tex - vec2(0.0, tex_offset.y * float(i))).rgb * weight[i];
		}
	}

	_out_color = vec4(result, 1.0);

	//float brightness = dot(gl_FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	//if (brightness > 1.0)
})")
}

} // MM::OpenGL::RenderTasks

