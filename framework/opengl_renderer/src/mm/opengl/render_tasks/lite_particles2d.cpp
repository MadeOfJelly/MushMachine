#include "./lite_particles2d.hpp"

#include <mm/opengl/shader.hpp>
#include <mm/opengl/shader_builder.hpp>
#include <mm/opengl/buffer.hpp>
#include <mm/opengl/instance_buffer.hpp>
#include <mm/opengl/vertex_array_object.hpp>

#include <mm/fs_const_archiver.hpp>

#include <mm/services/scene_service_interface.hpp>
#include <entt/entity/registry.hpp>
#include <mm/components/time_delta.hpp>
#include <mm/opengl/camera_3d.hpp>

#include "../components/lite_particles2d.hpp"

#include <mm/resource_manager.hpp>
#include "../lite_particles2d_type.hpp"
#include <mm/opengl/texture.hpp>
#include <mm/opengl/texture_loader.hpp>

#include <glm/common.hpp>

#include <tracy/Tracy.hpp>
#ifndef MM_OPENGL_3_GLES
	#include <tracy/TracyOpenGL.hpp>
#else
	#define TracyGpuContext
	#define TracyGpuCollect
	#define TracyGpuZone(...)
#endif

#include <mm/logger.hpp>

#include <vector>

namespace MM::OpenGL::RenderTasks {

// this was glsl code
// TODO: refactor this
// packing helper
static uint16_t pack_float_16bit(const float a, const float pack_mult) {
	uint32_t a_bits = uint32_t(abs(a*pack_mult));
	return (a_bits & 0x7fffu) | (uint16_t(a>=0.0 ? 0u : 1u) << 15u);
}

const float pack_vel_multiplier = 1000.0;

static float pack_vel(const glm::vec2 vel) {
	// ez just mult and save as shorts
	return glm::uintBitsToFloat(
		(pack_float_16bit(vel.x, pack_vel_multiplier) << 16u)
		| pack_float_16bit(vel.y, pack_vel_multiplier)
	);
}

const float pack_age_multiplier = 10000.0;

static float pack_age_type(const float age, const uint16_t type) {
	return glm::uintBitsToFloat((pack_float_16bit(age, pack_age_multiplier) << 16u) | (type & 0xffffu));
}

LiteParticles2D::LiteParticles2D(Engine& engine) {
	_particles_0_buffers[0] = std::make_unique<InstanceBuffer<glm::vec4>>();
	_particles_0_buffers[1] = std::make_unique<InstanceBuffer<glm::vec4>>();

	resetBuffers();

	_tf_vao[0] = std::make_unique<VertexArrayObject>();
	_tf_vao[1] = std::make_unique<VertexArrayObject>();

	auto setup_tf_vao = [this](size_t i) {
		_tf_vao[i]->bind();
		_particles_0_buffers[i]->bind();
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
		_particles_0_buffers[i]->unbind();

		glEnableVertexAttribArray(0);

		_tf_vao[i]->unbind();
	};

	setup_tf_vao(0);
	setup_tf_vao(1);


	// rendering

	_render_vao[0] = std::make_unique<VertexArrayObject>();
	_render_vao[1] = std::make_unique<VertexArrayObject>();

	auto setup_rend_vao = [this](size_t i) {
		size_t next_index = i == 0 ? 1 : 0;

		_render_vao[i]->bind();
		// bc we computed into the other one 0->1
		_particles_0_buffers[next_index]->bind();
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
		_particles_0_buffers[next_index]->unbind();

		glEnableVertexAttribArray(0);

		_render_vao[i]->unbind();
	};

	setup_rend_vao(0);
	setup_rend_vao(1);

	resetTypeBuffers();

	setupShaderFiles();

	_tf_shader = ShaderBuilder::start()
		.addStageVertexF(engine, vertexPathTF)
		.addTransformFeedbackVarying("_out_0")
		.addStageFragmentF(engine, fragmentPathTF) // empty stage
		.finish();
	assert(static_cast<bool>(_tf_shader));

	_points_shader = ShaderBuilder::start()
		.addStageVertexF(engine, vertexPathPoints)
		.addStageFragmentF(engine, fragmentPathPoints)
		.finish();
	assert(static_cast<bool>(_points_shader));

	_last_time = clock::now();
}

LiteParticles2D::~LiteParticles2D(void) {
}

void LiteParticles2D::uploadParticles(Services::OpenGLRenderer&, Scene& scene) {
	ZoneScopedN("MM::OpenGL::RenderTasks::LiteParticles2D::uploadParticles");

	if (!scene.ctx().contains<Components::LiteParticles2DUploadQueue>()) {
		// TODO: warn?
		return; // nothing to upload
	}

	auto& queue = scene.ctx().at<Components::LiteParticles2DUploadQueue>();

	while (!queue.queue.empty()) {
		// get range
		auto range_size = queue.queue.size();
		size_t range_first = _particle_buffer_next_index;
		if (_particle_buffer_next_index + (range_size - 1) >= _particle_buffer_size) { // wrap
			range_size = _particle_buffer_size - range_first;
		}

		//std::cerr << "s:" << range_size << " f:" << range_first << "\n";

		//assert(range_first <= range_last);
		assert(range_size >= 1);

		size_t curr_buff_index = first_particles_buffer ? 0 : 1;

#ifdef MM_OPENGL_3_GLES
		// i think invalidating the whole buffer, instead of only the range results in undefined behaviour, but webgl wont let me otherwise and it seems to work
		const auto gl_access = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT;
#else
		// turns out, amd desktop opengl drivers dont like invalidate buffer (everyone else seems to be ok)
		const auto gl_access = GL_MAP_WRITE_BIT;
#endif

		auto* data_0 = _particles_0_buffers[curr_buff_index]->mapRange(range_first, range_size, gl_access);

		for (size_t i = 0; i < range_size; i++) {
			auto& p = queue.queue.front();

			uint16_t type = 0u;
			if (_type_map.count(p.type_id)) {
				type = _type_map.at(p.type_id);
			} else {
				SPDLOG_ERROR("unknown type {}, defaulting to zero.", p.type_id);
			}

			data_0[i] = glm::vec4(
				p.pos.x, p.pos.y,
				pack_vel(p.vel),
				pack_age_type(p.age, type)
			);

			queue.queue.pop();
		}

		_particles_0_buffers[curr_buff_index]->unmap();

		_particle_buffer_next_index = (range_first + range_size) % (_particle_buffer_size);
	}
}

void LiteParticles2D::computeParticles(Services::OpenGLRenderer&, Scene& scene) {
	ZoneScopedN("MM::OpenGL::RenderTasks::LiteParticles2D::computeParticles");
	using namespace entt::literals;
	_tf_shader->bind();

	{ // time
		auto newNow = clock::now();
		std::chrono::duration<double, std::ratio<1, 1>> deltaTime = newNow - _last_time;
		_last_time = newNow;

		float time_delta = deltaTime.count() * scene.ctx().at<MM::Components::TimeDelta>().deltaFactor;
		_time += time_delta;
		_tf_shader->setUniform1f("_time_delta", time_delta);
		_tf_shader->setUniform1f("_time", _time);
	}

	auto& rm_t = MM::ResourceManager<Texture>::ref();
	rm_t.get("MM::LiteParticles2DTypes::Compute"_hs)->bind(0);

	//_tf_shader->setUniform3f("_env_vec", env_vec * env_force);
	//_tf_shader->setUniform1f("_noise_force", noise_force);
	//_tf_shader->setUniform1f("_dampening", dampening);

	const size_t curr_index = first_particles_buffer ? 0 : 1;
	const size_t next_index = first_particles_buffer ? 1 : 0;

	// bind in particles
	_tf_vao[curr_index]->bind();

	// bind out particles
	// the order is the same as given to the ShaderBuilder
	_particles_0_buffers[next_index]->bindBase(0);

	glEnable(GL_RASTERIZER_DISCARD); // compute only rn

	glBeginTransformFeedback(GL_POINTS);
	glDrawArrays(GL_POINTS, 0, _particle_buffer_size);
	glEndTransformFeedback();

	glDisable(GL_RASTERIZER_DISCARD);

	// TODO: move this
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, 0);
	//glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, 0);

	_tf_vao[curr_index]->unbind();
	_tf_shader->unbind();

	// TODO: do i need this??
	glFlush();

	first_particles_buffer = !first_particles_buffer;
}

void LiteParticles2D::renderParticles(Services::OpenGLRenderer& rs, Scene& scene) {
	using namespace entt::literals;
	ZoneScopedN("MM::OpenGL::RenderTasks::LiteParticles2D::renderParticles");

	if (!scene.ctx().contains<Camera3D>()) {
		return; // nothing to draw
	}

	//glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE); // dont write to depth
	//glEnable(GL_BLEND);

#ifndef MM_OPENGL_3_GLES
	glEnable(GL_PROGRAM_POINT_SIZE);
#endif

	rs.targets[target_fbo]->bind(FrameBufferObject::RW);

	_points_shader->bind();

	auto& rm_t = MM::ResourceManager<Texture>::ref();
	rm_t.get("MM::LiteParticles2DTypes::Render"_hs)->bind(0);

	Camera3D& cam = scene.ctx().at<Camera3D>();
	_points_shader->setUniformMat4f("_vp", cam.getViewProjection());

	GLint view_port[4];
	glGetIntegerv(GL_VIEWPORT, view_port);

	// width
	_points_shader->setUniform1f("_point_size", view_port[2]/cam.horizontalViewPortSize);

	const size_t curr_index = first_particles_buffer; // 0->1 / 1->0 bc compute phase also switches
	_render_vao[curr_index]->bind();

	glDrawArrays(GL_POINTS, 0, _particle_buffer_size);

	_render_vao[curr_index]->unbind();
	_points_shader->unbind();

	glDepthMask(GL_TRUE);
}

void LiteParticles2D::render(Services::OpenGLRenderer& rs, Engine& engine) {
	ZoneScopedN("MM::OpenGL::RenderTasks::LiteParticles2D::render");

	auto* scene_service = engine.tryService<MM::Services::SceneServiceInterface>();
	if (scene_service == nullptr) {
		return;
	}

	Scene& scene = scene_service->getScene();

	uploadParticles(rs, scene);
	computeParticles(rs, scene);
	renderParticles(rs, scene);
}

void LiteParticles2D::resetBuffers(void) {
	const auto gl_buffer_type = GL_DYNAMIC_COPY;
	auto reset_buffer_0 = [this](size_t i) {
		auto* data = _particles_0_buffers[i]->map(_particle_buffer_size, gl_buffer_type);
		for (size_t x = 0; x < _particle_buffer_size; x++) {
			data[x] = glm::vec4{
				0.f, 0.f, // pos
				0, // vel (pack, but 0 should mean 0)
				pack_age_type(2.f, 0)
			};
		}
		_particles_0_buffers[i]->unmap();
	};

	reset_buffer_0(0);
	reset_buffer_0(1);

	_particle_buffer_next_index = 0; // only matters if resize
}

void LiteParticles2D::resetTypeBuffers(void) {
	using namespace entt::literals;

	// clear
	_type_map.clear();

	// build data
	std::vector<float> type_compute_upload_buffer {};
	std::vector<float> type_render_upload_buffer {};

	// TODO: replace with "default"
	{ // 0
		// TODO: propper 0 particle
		LiteParticles2DType {
			{
				0.04f, //age_delta;

				{0.f, 0.f}, //force_vec

				5.f, //turbulence;
				0.1f, //turbulence_noise_scale;
				0.01f, //turbulence_individuality;
				0.5f, //turbulence_time_scale;

				1.0f, //dampening;
			},
			{
				{1.8f, 3.f, 6.f, 1.f}, //color_start;
				{2.3f, 1.2f, 0.5f, 1.f}, //color_end;

				0.1f, //size_start;
				0.02f //size_end;
			}
		}.upload(type_compute_upload_buffer, type_render_upload_buffer);
	}
	// defaults
	_type_map[0u] = 0u;
	_type_map[""_hs] = 0u;
	_type_map["0"_hs] = 0u;
	_type_map["default"_hs] = 0u;

	auto& rm_lpt = MM::ResourceManager<LiteParticles2DType>::ref();
	uint16_t curr_idx = 1; // 0 is reserved for now :P
	rm_lpt.each([this, &curr_idx, &type_compute_upload_buffer, &type_render_upload_buffer](const auto type_id, const auto& res) {
		_type_map[type_id] = curr_idx++;
		res->upload(type_compute_upload_buffer, type_render_upload_buffer);
	});
	SPDLOG_INFO("have {} LiteParticles2D types.", curr_idx);

	// upload / create textures
	// HACK: refactor loader
	auto& rm_t = MM::ResourceManager<Texture>::ref();
	{ // compute
		auto r = rm_t.reload<MM::OpenGL::TextureLoaderEmpty>(
			"MM::LiteParticles2DTypes::Compute",
			GL_R32F,
			LiteParticles2DType::Compute::_member_count, curr_idx,
			GL_RED,
			GL_FLOAT
		);
		assert(r);

		// and re-"create"
		rm_t.get("MM::LiteParticles2DTypes::Compute"_hs)->bind(0); // hack
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_R32F,
			LiteParticles2DType::Compute::_member_count, curr_idx,
			0,
			GL_RED,
			GL_FLOAT,
			type_compute_upload_buffer.data()
		);
		// just in case
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	{ // render
		auto r = rm_t.reload<MM::OpenGL::TextureLoaderEmpty>(
			"MM::LiteParticles2DTypes::Render",
			GL_R32F,
			LiteParticles2DType::Render::_member_count, curr_idx,
			GL_RED,
			GL_FLOAT
		);
		assert(r);

		// and re-"create"
		rm_t.get("MM::LiteParticles2DTypes::Render"_hs)->bind(0); // hack
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_R32F,
			LiteParticles2DType::Render::_member_count, curr_idx,
			0,
			GL_RED,
			GL_FLOAT,
			type_render_upload_buffer.data()
		);
		// just in case
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
}

void LiteParticles2D::setupShaderFiles(void) {
	FS_CONST_MOUNT_FILE(commonPathTF,
R"(

#include "/shaders/builtin/noise.glsl"

// variants, based on offset, might be faster
vec2 var_noise22(vec2 x) {
	return vec2(noise12(x), noise12(x + 13037.0));
}
vec2 var_noise23(vec3 x) {
	return vec2(noise13(x), noise13(x + 13037.0));
}
vec2 var_noise24(vec4 x) {
	return vec2(noise14(x), noise14(x + 13037.0));
}

// packing helper
uint pack_float_16bit(in float a, in float pack_mult) {
	uint a_bits = uint(abs(a*pack_mult));

	return (a_bits & 0x7fffu) | (uint(a>=0.0 ? 0u : 1u) << 15u);
}

float unpack_float_16bit(in uint a, in float pack_mult) {
	return (float(a & 0x7fffu) / pack_mult) * ((a >> 15u) != 0u ? -1.0 : 1.0);
}

const float pack_vel_multiplier = 1000.0;

float pack_vel(in vec2 vel) {
	return uintBitsToFloat(
		(pack_float_16bit(vel.x, pack_vel_multiplier) << 16u)
		| pack_float_16bit(vel.y, pack_vel_multiplier)
	);
}

vec2 unpack_vel(in float pack) {
	uint pack_bits = floatBitsToUint(pack);
	return vec2(
		unpack_float_16bit(pack_bits >> 16u, pack_vel_multiplier),
		unpack_float_16bit(pack_bits & 0xffffu, pack_vel_multiplier)
	);
}

const float pack_age_multiplier = 10000.0;

float pack_age_type(in float age, in uint type) {
	return uintBitsToFloat(
		(pack_float_16bit(age, pack_age_multiplier) << 16u)
		| (type & 0xffffu)
	);
}

)")
	FS_CONST_MOUNT_FILE(vertexPathTF,
GLSL_VERSION_STRING
R"(
#ifdef GL_ES
	precision highp float;
	precision highp int;
#endif

#include "./common.glsl"

uniform float _time_delta;
uniform float _time;

uniform sampler2D _type_buffer;

layout(location = 0) in vec4 _in_0;

out vec4 _out_0;

void set_out(in vec2 pos, in vec2 vel, in float age, in uint type) {
	_out_0 = vec4(
		pos.x, pos.y,
		pack_vel(vel),
		pack_age_type(age, type)
	);
}

void get_in(out vec2 pos, out vec2 vel, out float age, out uint type) {
	pos = _in_0.xy;
	vel = unpack_vel(_in_0.z);

	uint age_type_bits = floatBitsToUint(_in_0.w);
	age = unpack_float_16bit(age_type_bits >> 16u, pack_age_multiplier);
	type = age_type_bits & 0xffffu;
}

struct sim_config_type {
	highp float age_delta;

	//highp float gravity;
	vec2 force_vec;

	highp float turbulence;
	highp float turbulence_noise_scale;
	highp float turbulence_individuality;
	highp float turbulence_time_scale;

	highp float dampening;
};

vec2 fetch_vec2(in uint offset, in uint type) {
	return vec2(
		texelFetch(_type_buffer, ivec2(offset+0u, type), 0).r,
		texelFetch(_type_buffer, ivec2(offset+1u, type), 0).r
	);
}

sim_config_type fetch_type_config(uint type) {
	return sim_config_type(
		texelFetch(_type_buffer, ivec2(0, type), 0).r,
		fetch_vec2(1u, type),
		texelFetch(_type_buffer, ivec2(3, type), 0).r,
		texelFetch(_type_buffer, ivec2(4, type), 0).r,
		texelFetch(_type_buffer, ivec2(5, type), 0).r,
		texelFetch(_type_buffer, ivec2(6, type), 0).r,
		texelFetch(_type_buffer, ivec2(7, type), 0).r
	);
}

vec2 calc_turbulence_vec(
	in vec2 pos,
	in float turbulence,
	in float turbulence_noise_scale,
	in float turbulence_individuality,
	in float turbulence_time_scale
) {
	vec2 turbulence_vec =
		(
			noise24(vec4(
				pos * turbulence_noise_scale,
				float(gl_VertexID & 0x0fff) * turbulence_individuality,
				_time * turbulence_time_scale
			))
			- vec2(0.5)
		)
		* 2.0
		* turbulence
	;

	return turbulence_vec;
}

void calc_particle_sim(
// inout (particle mem)
	inout vec2 pos,
	inout vec2 vel,

	inout float age,
	inout uint type,

// type specific config
	in sim_config_type conf
) {
	vec2 turbulence_vec = calc_turbulence_vec(
		pos,
		conf.turbulence,
		conf.turbulence_noise_scale,
		conf.turbulence_individuality,
		conf.turbulence_time_scale
	);

	vel = (vel + (
		turbulence_vec +
		//vec2(0.0, -10.0) * conf.gravity
		conf.force_vec
	) * _time_delta);

	// TODO: this does not behave correctly
	// TODO: experiment with formula
	vel = mix(vel, vec2(0.0), conf.dampening * _time_delta);

	pos = pos + (vel * _time_delta);

	age = min(age + (conf.age_delta * _time_delta), 2.0);
}

void main() {
	vec2 pos;
	vec2 vel;
	float age;
	uint type;
	get_in(pos, vel, age, type);

	sim_config_type conf = fetch_type_config(type);

	calc_particle_sim(
		pos,
		vel,
		age,
		type,

		conf
	);

	set_out(pos, vel, age, type);
})")

	FS_CONST_MOUNT_FILE(fragmentPathTF,
GLSL_VERSION_STRING
R"(
#ifdef GL_ES
	precision mediump float;
#endif

// i am never executed

out vec4 _out_color;

void main() {
	_out_color = vec4(1.0);
})")

	FS_CONST_MOUNT_FILE(vertexPathPoints,
GLSL_VERSION_STRING
R"(
#ifdef GL_ES
	precision highp float;
	precision highp int;
#endif

#include "./common.glsl"

uniform mat4 _vp;
uniform float _point_size;

uniform sampler2D _type_buffer;

layout(location = 0) in vec4 _in_0;

void get_in(out vec2 pos, out vec2 vel, out float age, out uint type) {
	pos = _in_0.xy;
	vel = unpack_vel(_in_0.z);

	uint age_type_bits = floatBitsToUint(_in_0.w);
	age = unpack_float_16bit(age_type_bits >> 16u, pack_age_multiplier);
	type = age_type_bits & 0xffffu;
}

out vec4 _frag_color;
flat out int _vertex_id;

struct config_type {
	highp vec4 color_start;
	highp vec4 color_end;

	highp float size_start;
	highp float size_end;
};

vec4 fetch_vec4(in uint offset, in uint type) {
	return vec4(
		texelFetch(_type_buffer, ivec2(offset+0u, type), 0).r,
		texelFetch(_type_buffer, ivec2(offset+1u, type), 0).r,
		texelFetch(_type_buffer, ivec2(offset+2u, type), 0).r,
		texelFetch(_type_buffer, ivec2(offset+3u, type), 0).r
	);
}

config_type fetch_config_type(uint type) {
	return config_type(
		fetch_vec4(0u, type),
		fetch_vec4(4u, type),
		texelFetch(_type_buffer, ivec2(8, type), 0).r,
		texelFetch(_type_buffer, ivec2(9, type), 0).r
	);
}

void main() {
	vec2 in_pos;
	vec2 in_vel;

	float in_age;
	uint in_type;

	get_in(in_pos, in_vel, in_age, in_type);

	// skip dead
	if (in_age > 1.0) {
		_frag_color = vec4(1.0, 0.0, 1.0, 0.0); // so it does not get rendered (alpha)
		gl_Position.z = 100000000000.0; // clip (does not allways work <.<)
		// gl_PointSize = 0.0; // ub
		return;
	}

	config_type conf = fetch_config_type(in_type);

	//_frag_color = mix(conf.color_start, conf.color_end, in_age);
	_frag_color = mix(conf.color_start, conf.color_end, smoothstep(0.0, 1.0, in_age));

	gl_Position = _vp * vec4(in_pos, 0.0, 1.0);

	// TODO: fix before merge
	gl_Position.z = -1.0; // hack for ortho ??

	gl_PointSize = max(mix(conf.size_start, conf.size_end, in_age) * _point_size, 1.0);

	_vertex_id = gl_VertexID;
})")

	FS_CONST_MOUNT_FILE(fragmentPathPoints,
GLSL_VERSION_STRING
R"(
#ifdef GL_ES
	precision mediump float;
#endif

in vec4 _frag_color;
flat in int _vertex_id;

out vec4 _out_color;

bool should_discard(vec2 screen_pos, float alpha) {
	const int dither[8 * 8] = int[8 * 8](
		 0, 32,  8, 40,  2, 34, 10, 42,
		48, 16, 56, 24, 50, 18, 58, 26,
		12, 44,  4, 36, 14, 46,  6, 38,
		60, 28, 52, 20, 62, 30, 54, 22,
		 3, 35, 11, 43,  1, 33,  9, 41,
		51, 19, 59, 27, 49, 17, 57, 25,
		15, 47,  7, 39, 13, 45,  5, 37,
		63, 31, 55, 23, 61, 29, 53, 21
	);

	int mat_value = dither[int(screen_pos.x)%8 + 8 * (int(screen_pos.y)%8)];
	float n_mat_value = float(mat_value) * (1.0/64.0);

	return n_mat_value < 1.0 - alpha;
}

void main() {
	if (should_discard(vec2(gl_FragCoord.x + float(_vertex_id%8), gl_FragCoord.y + float(_vertex_id%7)), _frag_color.a)) {
		discard;
	}

	_out_color = vec4(_frag_color.rgb, 1.0);
})")

}

} // MM::OpenGL::RenderTasks

