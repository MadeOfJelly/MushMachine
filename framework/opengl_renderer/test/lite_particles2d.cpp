#include <gtest/gtest.h>

#include <mm/engine.hpp>

#include <mm/services/filesystem.hpp>
#include <mm/services/sdl_service.hpp>
#include <mm/services/organizer_scene.hpp>
#include <mm/services/opengl_renderer.hpp>

#include <mm/fs_const_archiver.hpp>

#include <entt/entity/registry.hpp>
#include <entt/entity/organizer.hpp>
#include <entt/core/hashed_string.hpp>

#include <mm/opengl/render_tasks/clear.hpp>
#include <mm/opengl/render_tasks/lite_particles2d.hpp>
#include <mm/opengl/bloom.hpp>
#include <mm/opengl/render_tasks/composition.hpp>

// ctx
#include <mm/opengl/components/lite_particles2d.hpp>
#include <mm/components/time_delta.hpp>
#include <mm/opengl/camera_3d.hpp>

// components
#include <mm/components/position2d.hpp>

#include <mm/resource_manager.hpp>
#include <mm/opengl/texture_loader.hpp>
#include <mm/opengl/lite_particles2d_type_loader.hpp>

#include <mm/opengl/fbo_builder.hpp>

#include <mm/random/srng.hpp>

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <glm/common.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>

#include <mm/scalar_range2.hpp>

#include <vector>
#include <string>

using namespace entt::literals;

namespace Components {

// or what ever
struct LiteParticles2DEmitter {
	float age {0.f}; // this changes each tick, if >=1, spawn and reset to 0
	float age_delta {1.f}; // times per sec

	MM::ScalarRange2<uint16_t> particle_count {1u, 1u};

	// particles data

	std::string p_type {};

	MM::ScalarRange2<float> p_pos_x {0.f, 0.f};
	MM::ScalarRange2<float> p_pos_y {0.f, 0.f};

	MM::ScalarRange2<float> p_dir {0.f, 0.f}; // 0-1, not rad
	MM::ScalarRange2<float> p_dir_force {0.f, 0.f};

	MM::ScalarRange2<float> p_age {0.f, 0.f};
};

} // Components

namespace Systems {

void lite_particles2d_emit(
	entt::view<entt::get_t<
		const MM::Components::Position2D,
		Components::LiteParticles2DEmitter
	>> view,
	const MM::Components::TimeDelta& td,
	MM::Components::LiteParticles2DUploadQueue& lp_uq,
	MM::Random::SRNG& rng
) {
	view.each([&lp_uq, &td, &rng](const auto& pos, Components::LiteParticles2DEmitter& lpe) {
		lpe.age += lpe.age_delta * td.tickDelta;

		if (lpe.age < 1.f) {
			return;
		}

		lpe.age = 0.f; // TODO: dont discard ?

		const auto type = entt::hashed_string::value(lpe.p_type.c_str());

		const size_t count = rng.range(lpe.particle_count);
		for (size_t i = 0; i < count; i++) {
			float dir = rng.range(lpe.p_dir) * glm::two_pi<float>();
			lp_uq.queue.push(MM::Components::LiteParticle2D{
				type, // type

				//{rng.negOneToOne() * 30.f, rng.negOneToOne() * 30.f}, // pos
				pos.pos + /*lpe.pos_offset +*/ glm::vec2{rng.range(lpe.p_pos_x), rng.range(lpe.p_pos_y)}, // pos
				glm::vec2{glm::cos(dir), glm::sin(dir)} * rng.range(lpe.p_dir_force), // vel

				rng.range(lpe.p_age) // age
			});
		}
	});
}

} // Systems

const char* argv0;

static void setup_textures(MM::Engine& engine) {
	auto& rm_t = MM::ResourceManager<MM::OpenGL::Texture>::ref();
	auto [w, h] = engine.getService<MM::Services::SDLService>().getWindowSize();

	// we dont have a gbuffer in this example
	{ // gbuffer
		// depth
#ifdef MM_OPENGL_3_GLES
		rm_t.reload<MM::OpenGL::TextureLoaderEmpty>(
			"depth",
			GL_DEPTH_COMPONENT24, // d16 is the onlyone for gles 2 (TODO: test for 3)
			w, h,
			GL_DEPTH_COMPONENT, GL_UNSIGNED_INT
		);
#else
		rm_t.reload<MM::OpenGL::TextureLoaderEmpty>(
			"depth",
			GL_DEPTH_COMPONENT32F, // TODO: stencil?
			w, h,
			GL_DEPTH_COMPONENT, GL_FLOAT
		);
#endif
	}

	const float render_scale = 1.f;

	// hdr color gles3 / webgl2
	rm_t.reload<MM::OpenGL::TextureLoaderEmpty>(
		"hdr_color",
		GL_RGBA16F,
		w * render_scale, h * render_scale,
		GL_RGBA,
		GL_HALF_FLOAT
	);
	{ // filter
		rm_t.get("hdr_color"_hs)->bind(0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
}

static void setup_fbos(MM::Engine& engine) {
	auto& rs = engine.getService<MM::Services::OpenGLRenderer>();
	auto& rm_t = MM::ResourceManager<MM::OpenGL::Texture>::ref();

	const float render_scale = 1.f;

	rs.targets["game_view"] = MM::OpenGL::FBOBuilder::start()
		.attachTexture(rm_t.get("hdr_color"_hs), GL_COLOR_ATTACHMENT0)
		.attachTexture(rm_t.get("depth"_hs), GL_DEPTH_ATTACHMENT)
		.setResizeFactors(render_scale, render_scale)
		.setResize(true)
		.finish();
	assert(rs.targets["game_view"]);
}

void setup_particles_types(MM::Engine& engine) {
	auto& lpt_rm = MM::ResourceManager<MM::OpenGL::LiteParticles2DType>::ref();

	lpt_rm.load<MM::OpenGL::LiteParticles2DTypeLoaderJson>("MM::spark1",
R"({
  "compute": {
    "age_delta": 2.0,
    "force_vec": { "x": 0.0, "y": -5.0 },
    "turbulence": 1.0,
    "turbulence_individuality": 1.0,
    "turbulence_noise_scale": 1.0,
    "turbulence_time_scale": 1.0,
    "dampening": 1.0
  },
  "render": {
    "color_start": {
      "x": 6.0,
      "y": 6.0,
      "z": 1.8,
      "w": 1.0
    },
    "color_end": {
      "x": 1.5,
      "y": 1.0,
      "z": 0.3,
      "w": 1.0
    },
    "size_start": 0.01,
    "size_end": 0.002
  }
})"_json);

	// mount into vfx
	FS_CONST_MOUNT_FILE("/particles/lite_particles2d/fire1.json",
R"({
  "compute": {
    "age_delta": 1.0,
    "force_vec": { "x": 0.0, "y": 5.0 },
    "turbulence": 5.0,
    "turbulence_individuality": 1.0,
    "turbulence_noise_scale": 1.0,
    "turbulence_time_scale": 1.0,
    "dampening": 0.0
  },
  "render": {
    "color_start": {
      "x": 3.0,
      "y": 2.1,
      "z": 1.5,
      "w": 0.8
    },
    "color_end": {
      "x": 3.0,
      "y": 1.1,
      "z": 1.0,
      "w": 0.0
    },
    "size_start": 0.15,
    "size_end": 0.4
  }
})");
	// and load it
	lpt_rm.load<MM::OpenGL::LiteParticles2DTypeLoaderFile>("MM::fire1", engine, "/particles/lite_particles2d/fire1.json");

}

TEST(lite_particles2d, it) {
	MM::Engine engine;

	auto& sdl_ss = engine.addService<MM::Services::SDLService>();
	ASSERT_TRUE(engine.enableService<MM::Services::SDLService>());

	sdl_ss.createGLWindow("lite_particles2d", 1280, 720);

	engine.addService<MM::Services::OrganizerSceneService>();
	ASSERT_TRUE(engine.enableService<MM::Services::OrganizerSceneService>());

	bool provide_ret = engine.provide<MM::Services::SceneServiceInterface, MM::Services::OrganizerSceneService>();
	ASSERT_TRUE(provide_ret);
	auto& scene = engine.tryService<MM::Services::SceneServiceInterface>()->getScene();

	engine.addService<MM::Services::FilesystemService>(argv0, "lite_particles2d");
	ASSERT_TRUE(engine.enableService<MM::Services::FilesystemService>());

	auto& rs = engine.addService<MM::Services::OpenGLRenderer>();
	ASSERT_TRUE(engine.enableService<MM::Services::OpenGLRenderer>());

	// load particle types
	// before addRenderTask<LiteParticle2D>
	// OR
	// call LiteParticle2D::resetTypeBuffers()
	setup_particles_types(engine);

	{ // setup rendering
		// TODO: split vertically
		setup_textures(engine);
		setup_fbos(engine);

		// clear
		auto& clear_opaque = rs.addRenderTask<MM::OpenGL::RenderTasks::Clear>(engine);
		clear_opaque.target_fbo = "game_view";
		// clears all color attachments
		clear_opaque.r = 0.f;
		clear_opaque.g = 0.f;
		clear_opaque.b = 0.f;
		clear_opaque.a = 1.f;
		clear_opaque.mask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;

		{ // render
			//MM::OpenGL::RenderTasks::SimpleRect& bsrr_rend = rs.addRenderTask<MM::OpenGL::RenderTasks::SimpleRect>(engine);
			//bsrr_rend.target_fbo = "game_view";
			auto& lprt = rs.addRenderTask<MM::OpenGL::RenderTasks::LiteParticles2D>(engine);
			lprt.target_fbo = "game_view";
		}

		// rn does rt too
		MM::OpenGL::setup_bloom(engine);

		// not part of setup_bloom
		auto& comp = rs.addRenderTask<MM::OpenGL::RenderTasks::Composition>(engine);
		comp.color_tex = "hdr_color";
		comp.bloom_tex = "blur_tmp1";
		comp.target_fbo = "display";
	}


	// setup scene

	auto& cam = scene.ctx().emplace<MM::OpenGL::Camera3D>();
	cam.horizontalViewPortSize = 30.f;
	cam.setOrthographic();
	cam.updateView();

	scene.ctx().emplace<MM::Components::LiteParticles2DUploadQueue>();

	scene.ctx().emplace<MM::Random::SRNG>(42u);

	// setup v system
	auto& org = scene.ctx().emplace<entt::organizer>();
	org.emplace<Systems::lite_particles2d_emit>("lite_particles2d_emit");

	// HACK: instead you would switch to this scene
	engine.getService<MM::Services::OrganizerSceneService>().updateOrganizerVertices(scene);

	{ // default
		auto e = scene.create();
		auto& p = scene.emplace<MM::Components::Position2D>(e);
		p.pos.x = -10.f;

		auto& lpe = scene.emplace<Components::LiteParticles2DEmitter>(e);
		lpe.age_delta = 60.f;
		lpe.particle_count = {1, 1};
		lpe.p_type = "default";
		lpe.p_dir = {-0.1f, +0.1f};
		lpe.p_dir_force = {0.f, 8.f};
		lpe.p_age = {0.f, 0.7f};
	}

	{ // sparks
		auto e = scene.create();
		auto& p = scene.emplace<MM::Components::Position2D>(e);
		p.pos.x = 0.f;

		auto& lpe = scene.emplace<Components::LiteParticles2DEmitter>(e);
		lpe.age_delta = 1.f;
		lpe.particle_count = {25, 40};
		lpe.p_type = "MM::spark1";
		lpe.p_dir = {0.f, 1.f};
		lpe.p_dir_force = {0.f, 1.f}; // m/s
		lpe.p_age = {0.f, 0.7f};
	}

	{ // fire
		auto e = scene.create();
		auto& p = scene.emplace<MM::Components::Position2D>(e);
		p.pos.x = 10.f;

		auto& lpe = scene.emplace<Components::LiteParticles2DEmitter>(e);
		lpe.age_delta = 60.f;
		lpe.particle_count = {1, 1};
		lpe.p_type = "MM::fire1";
		lpe.p_dir = {0.f, 1.f};
		lpe.p_dir_force = {0.f, 1.f};
		lpe.p_age = {0.f, 0.5f};
	}

	engine.run();
}

int main(int argc, char** argv) {
	argv0 = argv[0];

	::testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}

