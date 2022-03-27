#include "./bloom_combine.hpp"

#include <mm/fs_const_archiver.hpp>
#include <mm/services/opengl_renderer.hpp>
#include <mm/opengl/texture.hpp>

#include <tracy/Tracy.hpp>

namespace MM::OpenGL::RenderTasks {

BloomCombine::BloomCombine(Engine& engine) {
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

BloomCombine::~BloomCombine(void) {
}

void BloomCombine::render(Services::OpenGLRenderer& rs, Engine& engine) {
	ZoneScopedN("RenderTasks::BloomCombine::render");

	rs.targets[target_fbo]->bind(FrameBufferObject::W);
	{ // TODO: move to fbo
		GLsizei width {0};
		GLsizei height {0};
		{ // get size of fb <.<
			auto& fbo_tex_arr = rs.targets[target_fbo]->_texAttachments;
			if (fbo_tex_arr.empty()) {
				//GLint o_type {0};
				//glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &o_type);
				//if (o_type == GL_NONE) {
					//// default framebuffer or error
					//SPDLOG_INFO("gl none");
				//}

				// nah, just assume screen res
				std::tie(width, height) = engine.getService<Services::SDLService>().getWindowSize();
			} else {
				auto& target_fbo_tex = rs.targets[target_fbo]->_texAttachments.front();
				width = target_fbo_tex->width;
				height = target_fbo_tex->height;
			}
		}
		glViewport(0, 0, width, height);
	}


	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	_shader->bind();
	_vertexBuffer->bind(GL_ARRAY_BUFFER);
	_vao->bind();

	auto& rm = MM::ResourceManager<Texture>::ref();

	auto tex_0 = rm.get(entt::hashed_string::value(tex0.c_str()));
	tex_0->bind(0);

	auto tex_1 = rm.get(entt::hashed_string::value(tex1.c_str()));
	tex_1->bind(1);

	// assign image units
	_shader->setUniform1i("_tex0", 0);
	_shader->setUniform1i("_tex1", 1);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	_vao->unbind();
	_vertexBuffer->unbind(GL_ARRAY_BUFFER);
	_shader->unbind();
}

void BloomCombine::reloadShaders(Engine& engine) {
	auto tmp_shader = Shader::createF(engine, vertexPath, fragmentPath);
	if (tmp_shader) {
		_shader = tmp_shader;
	}
}

void BloomCombine::setupShaderFiles(void) {
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

// tent sampling
#include "/shaders/builtin/sampling.glsl"

uniform sampler2D _tex0;
uniform sampler2D _tex1;

in vec2 _uv;

out vec3 _out_color;

vec3 tentSampling() {
	// i hope the pipeline caches this
	ivec2 tex0_size = textureSize(_tex0, 0); // TODO: lod
	vec2 tex0_texel_step = vec2(1.0) / vec2(tex0_size);

	return sampling2D_tent3x3_vec3(
		_tex0,
		_uv,
		tex0_texel_step
	);
}

vec3 simpleSampling() {
	return texture(_tex0, _uv).rgb;
}

void main() {
#ifdef SIMPLE_SAMPLING
	_out_color = texture(_tex1, _uv).rgb + simpleSampling();
#else
	_out_color = texture(_tex1, _uv).rgb + tentSampling();
#endif
})")
}

} // MM::OpenGL::RenderTasks

