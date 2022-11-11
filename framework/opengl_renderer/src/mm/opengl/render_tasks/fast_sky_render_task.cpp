#include "./fast_sky_render_task.hpp"

#include <mm/opengl/shader.hpp>
#include <mm/opengl/buffer.hpp>
#include <mm/opengl/vertex_array_object.hpp>

#include <mm/fs_const_archiver.hpp>

#include <mm/engine.hpp>

#include <mm/opengl/camera_3d.hpp>
#include <mm/services/scene_service_interface.hpp>
#include <entt/entity/registry.hpp>

#include <tracy/Tracy.hpp>
#ifndef MM_OPENGL_3_GLES
	#include <tracy/TracyOpenGL.hpp>
#else
	#define TracyGpuContext
	#define TracyGpuCollect
	#define TracyGpuZone(...)
#endif

namespace MM::OpenGL::RenderTasks {

FastSky::FastSky(MM::Engine& engine) {
	setupShaderFiles();
	_shader = MM::OpenGL::Shader::createF(engine, vertexPath, fragmentPath);
	assert(_shader != nullptr);

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
}

FastSky::~FastSky(void) {
}

void FastSky::render(MM::Services::OpenGLRenderer& rs, MM::Engine& engine) {
	ZoneScopedN("MM::OpenGL::RenderTasks::FastSky::render");

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
	glDepthFunc(GL_LEQUAL);
	//glDepthFunc(GL_LESS);
	glDisable(GL_BLEND); // i hate my life

	_shader->bind();
	_vertexBuffer->bind(GL_ARRAY_BUFFER);
	_vao->bind();

	{
		auto& cam = scene.ctx().get<MM::OpenGL::Camera3D>();
		MM::OpenGL::Camera3D tmp_cam = cam;
		// create cam with y up, bc shader says so
		tmp_cam.up = {0, 1, 0};
		tmp_cam.updateView();

		_shader->setUniformMat4f("V", tmp_cam.getView());
		_shader->setUniformMat4f("P", cam.getProjection());
	}

	{
		//auto* ctx_ptr =  scene.try_ctx<FastSkyContext>();
		//if (!ctx_ptr) {
			//ctx_ptr = &_default_context;
		//}
		auto* ctx_ptr = &_default_context;
		if (scene.ctx().contains<FastSkyContext>()) {
			ctx_ptr = &scene.ctx().get<FastSkyContext>();
		}

		_shader->setUniform1f("time", ctx_ptr->time);

		_shader->setUniform1f("cirrus", ctx_ptr->cirrus);
		_shader->setUniform1f("cumulus", ctx_ptr->cumulus);

		_shader->setUniform3f("fsun", ctx_ptr->fsun);
	}

	//glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	_shader->unbind();
}

void FastSky::reloadShaders(MM::Engine& engine) {
	auto tmp_shader = MM::OpenGL::Shader::createF(engine, vertexPath, fragmentPath);
	if (tmp_shader) {
		_shader = tmp_shader;
	}
}

// the shaders are based on https://github.com/shff/opengl_sky/
// with the following LICENSE:

// MIT License

//Copyright (c) 2019 Silvio Henrique Ferreira

//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:

//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.

//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

void FastSky::setupShaderFiles(void) {
	FS_CONST_MOUNT_FILE(vertexPath,
GLSL_VERSION_STRING
R"(
#ifdef GL_ES
	precision mediump float;
#endif

in vec2 vertex_pos;

out vec3 pos;

uniform mat4 P;
uniform mat4 V;

void main() {
	vec4 tmp_pos = vec4(vertex_pos, 1.0, 1.0);
	gl_Position = tmp_pos;

	pos = transpose(mat3(V)) * (inverse(P) * tmp_pos).xyz;
})")

	FS_CONST_MOUNT_FILE(fragmentPath,
GLSL_VERSION_STRING
R"(
#ifdef GL_ES
	precision mediump float;
#endif

in vec3 pos;
uniform vec3 fsun;

out vec4 color;

uniform float time;
uniform float cirrus;
uniform float cumulus;

const float Br = 0.0025;
const float Bm = 0.0003;
const float g =  0.9800;
const vec3 nitrogen = vec3(0.650, 0.570, 0.475);
const vec3 Kr = Br / pow(nitrogen, vec3(4.0));
const vec3 Km = Bm / pow(nitrogen, vec3(0.84));

float hash(float n) {
	return fract(sin(n) * 43758.5453123);
}

float noise(vec3 x) {
	vec3 f = fract(x);
	float n = dot(floor(x), vec3(1.0, 157.0, 113.0));
	return mix(mix(mix(hash(n +   0.0), hash(n +   1.0), f.x),
				   mix(hash(n + 157.0), hash(n + 158.0), f.x), f.y),
			   mix(mix(hash(n + 113.0), hash(n + 114.0), f.x),
				   mix(hash(n + 270.0), hash(n + 271.0), f.x), f.y), f.z);
}

const mat3 m = mat3(0.0, 1.60,  1.20, -1.6, 0.72, -0.96, -1.2, -0.96, 1.28);

float fbm(vec3 p) {
	float f = 0.0;
	f += noise(p) / 2.0; p = m * p * 1.1;
	f += noise(p) / 4.0; p = m * p * 1.2;
	f += noise(p) / 6.0; p = m * p * 1.3;
	f += noise(p) / 12.0; p = m * p * 1.4;
	f += noise(p) / 24.0;
	return f;
}

void main() {
	if (pos.y < 0.0) {
		discard;
		//return;
	}

	// Atmosphere Scattering
	float mu = dot(normalize(pos), normalize(fsun));
	vec3 extinction = mix(exp(-exp(-((pos.y + fsun.y * 4.0) * (exp(-pos.y * 16.0) + 0.1) / 80.0) / Br) * (exp(-pos.y * 16.0) + 0.1) * Kr / Br) * exp(-pos.y * exp(-pos.y * 8.0 ) * 4.0) * exp(-pos.y * 2.0) * 4.0, vec3(1.0 - exp(fsun.y)) * 0.2, -fsun.y * 0.2 + 0.5);
	color.rgb = 3.0 / (8.0 * 3.14) * (1.0 + mu * mu) * (Kr + Km * (1.0 - g * g) / (2.0 + g * g) / pow(1.0 + g * g - 2.0 * g * mu, 1.5)) / (Br + Bm) * extinction;

	// Cirrus Clouds
	float density = smoothstep(1.0 - cirrus, 1.0, fbm(pos.xyz / pos.y * 2.0 + time * 0.05)) * 0.3;
	color.rgb = mix(color.rgb, extinction * 4.0, density * max(pos.y, 0.0));

	// Cumulus Clouds
	for (int i = 0; i < 3; i++) {
		float density = smoothstep(1.0 - cumulus, 1.0, fbm((0.7 + float(i) * 0.01) * pos.xyz / pos.y + time * 0.3));
		color.rgb = mix(color.rgb, extinction * density * 5.0, min(density, 1.0) * max(pos.y, 0.0));
	}

	// Dithering Noise
	color.rgb += noise(pos * 1000.0) * 0.01; // needed?
})")
}

} // MM::OpenGL::RenderTasks

