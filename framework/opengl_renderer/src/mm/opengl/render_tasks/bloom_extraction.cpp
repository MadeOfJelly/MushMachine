#include "./bloom_extraction.hpp"

#include <mm/services/opengl_renderer.hpp>
#include <mm/fs_const_archiver.hpp>
#include <mm/opengl/texture.hpp>

#include <tracy/Tracy.hpp>

namespace MM::OpenGL::RenderTasks {

BloomExtraction::BloomExtraction(Engine& engine) {
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

BloomExtraction::~BloomExtraction(void) {
}

void BloomExtraction::render(Services::OpenGLRenderer& rs, Engine&) {
	ZoneScopedN("RenderTasks::BloomExtraction::render");

	auto& target_fbo_ = rs.targets[target_fbo];
	target_fbo_->bind(FrameBufferObject::W);
	auto& target_fbo_tex = target_fbo_->_texAttachments.front();
	glViewport(0, 0, target_fbo_tex->width, target_fbo_tex->height);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	_shader->bind();
	_vertexBuffer->bind(GL_ARRAY_BUFFER);
	_vao->bind();

	auto& rm = MM::ResourceManager<Texture>::ref();

	auto tex = rm.get(entt::hashed_string::value(src_tex.c_str()));
	tex->bind(0);

	_shader->setUniform1i("color_tex", 0);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	_vao->unbind();
	_vertexBuffer->unbind(GL_ARRAY_BUFFER);
	_shader->unbind();
}

void BloomExtraction::reloadShaders(Engine& engine) {
	auto tmp_shader = Shader::createF(engine, vertexPath, fragmentPath);
	if (tmp_shader) {
		_shader = tmp_shader;
	}
}

void BloomExtraction::setupShaderFiles(void) {
	FS_CONST_MOUNT_FILE(vertexPath,
GLSL_VERSION_STRING
R"(
#ifdef GL_ES
	precision mediump float;
#endif

in vec2 _vertexPosition;
out vec2 _uv;

void main() {
	gl_Position = vec4(_vertexPosition, 0, 1);
	_uv = vec2(_vertexPosition.x * 0.5 + 0.5, _vertexPosition.y * 0.5 + 0.5);
})")

	FS_CONST_MOUNT_FILE(fragmentPath,
GLSL_VERSION_STRING
R"(
#ifdef GL_ES
	precision mediump float;
#endif

uniform sampler2D color_tex;

in vec2 _uv;

out vec3 _out_color;

// global config, keep it constant
const float fake_halation_strenth = 1.0;

void main() {
	vec3 color = texture(color_tex, _uv).rgb;

	// TODO: expose threshold
	//_out_color = max(vec3(0.0), color - vec3(1.0));

	//_out_color = mix(
		//color, // < 0.0
		//max(vec3(0.0), color - vec3(1.0)), // > 0.0
		//step(vec3(0.0), color)
	//);

	const vec3 fake_halation_base = vec3(0.03, 0.01, 0.0) * vec3(fake_halation_strenth);
	const vec3 fake_halation_extraction_offset = vec3(1.0) - fake_halation_base;
	const vec3 fake_halation_bloom_fact = vec3(1.0) + fake_halation_base;

	if (fake_halation_strenth <= 0.001) {
		_out_color = max(min(color, vec3(0.0)), color - vec3(1.0));
	} else {
		_out_color = max(min(color, vec3(0.0)), color - fake_halation_extraction_offset) * fake_halation_bloom_fact;
		//_out_color = max(min(color, vec3(0.0)), color - vec3(0.95, 0.97, 1.0)) * vec3(1.10, 1.05, 1.0);
	}
})")
}

} // MM::OpenGL::RenderTasks

