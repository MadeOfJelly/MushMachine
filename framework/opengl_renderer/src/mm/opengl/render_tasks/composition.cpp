#include "./composition.hpp"

#include <mm/fs_const_archiver.hpp>
#include <mm/services/opengl_renderer.hpp>
#include <mm/opengl/texture.hpp>

#include <tracy/Tracy.hpp>

namespace MM::OpenGL::RenderTasks {

Composition::Composition(MM::Engine& engine) {
	float vertices[] = {
		-1.f, 1.f,
		-1.f, -1.f,
		1.f, -1.f,
		1.f, -1.f,
		1.f, 1.f,
		-1.f, 1.f,
	};

	_vertexBuffer = std::make_unique<MM::OpenGL::Buffer>(vertices, 2 * 6 * sizeof(float), GL_STATIC_DRAW);
	_vao = std::make_unique<MM::OpenGL::VertexArrayObject>();
	_vao->bind();
	_vertexBuffer->bind(GL_ARRAY_BUFFER);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);

	_vertexBuffer->unbind(GL_ARRAY_BUFFER);
	_vao->unbind();

	setupShaderFiles();
	_shader = MM::OpenGL::Shader::createF(engine, vertexPath, fragmentPath);
	assert(_shader != nullptr);
}

Composition::~Composition(void) {
}

void Composition::render(MM::Services::OpenGLRenderer& rs, MM::Engine& engine) {
	ZoneScopedN("RenderTasks::Composition::render");

	rs.targets[target_fbo]->bind(MM::OpenGL::FrameBufferObject::W);
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
				std::tie(width, height) = engine.getService<MM::Services::SDLService>().getWindowSize();
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

	auto& rm = MM::ResourceManager<MM::OpenGL::Texture>::ref();

	auto tex_a = rm.get(entt::hashed_string::value(color_tex.c_str()));
	tex_a->bind(0);

	auto tex_n = rm.get(entt::hashed_string::value(bloom_tex.c_str()));
	tex_n->bind(1);

	// assign image units
	_shader->setUniform1i("color_tex", 0);
	_shader->setUniform1i("bloom_tex", 1);

	_shader->setUniform1f("bloom_factor", bloom_factor);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	_vao->unbind();
	_vertexBuffer->unbind(GL_ARRAY_BUFFER);
	_shader->unbind();
}

void Composition::reloadShaders(MM::Engine& engine) {
	auto tmp_shader = MM::OpenGL::Shader::createF(engine, vertexPath, fragmentPath);
	if (tmp_shader) {
		_shader = tmp_shader;
	}
}

void Composition::setupShaderFiles(void) {
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

#include "/shaders/builtin/tonemapping.glsl"
#include "/shaders/builtin/hashing.glsl"

uniform sampler2D color_tex;
uniform sampler2D bloom_tex;

// high bc 32bit on cpu
uniform highp float bloom_factor;

in vec2 _uv;

out vec3 _out_color;

void main() {
	vec3 color = texture(color_tex, _uv).rgb;
	vec3 bloom = texture(bloom_tex, _uv).rgb;

	vec3 comp = max(vec3(0.0), color + bloom * vec3(bloom_factor));

#if 0
	const vec3 tint = vec3(1.5, 0.8, 1.1);
	comp *= tint;
#endif

	//// reinhart + gamma
	//_out_color = pow(comp / (comp + vec3(1.0)), vec3(1.0 / 2.2));

	// makes more sense pre bloom
	//comp *= vec3(pow(2.0, -1.0)); // pre expose, -1 fstops

	//_out_color = tonemapReinhard(comp);
	_out_color = tonemapACESFilm(comp); // looks right
	//_out_color = pow(tonemapACESFilm(pow(comp, vec3(2.2))), vec3(1.0/2.2)); // insane saturation o.O
	//_out_color = pow(tonemapACESFilm(comp), vec3(1.0/2.2)); // looks just wrong

	// noise dither for 8bit displays
	_out_color += (hash32(gl_FragCoord.xy) * vec3(2.0) - vec3(1.0)) / vec3(255.0);
})")
}

} // MM::OpenGL::RenderTasks

