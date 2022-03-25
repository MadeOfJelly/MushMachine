#include <gtest/gtest.h>

#include <mm/engine.hpp>

#include <mm/services/filesystem.hpp>
#include <mm/services/sdl_service.hpp>
#include <mm/services/organizer_scene.hpp>
#include <mm/services/opengl_renderer.hpp>

#include <entt/entity/registry.hpp>
#include <entt/entity/organizer.hpp>

#include <mm/opengl/render_tasks/clear.hpp>
#include <mm/opengl/render_tasks/blit_fb.hpp>
#include <mm/opengl/render_tasks/simple_rect.hpp>
//#include <mm/opengl/render_tasks/bloom_extraction.hpp>
//#include <mm/opengl/render_tasks/blur.hpp>
//#include <mm/opengl/render_tasks/bloom_combine.hpp>
#include <mm/opengl/bloom.hpp>
#include <mm/opengl/render_tasks/composition.hpp>

#include <mm/components/position2d.hpp>
#include <mm/components/position2d_zoffset.hpp>
#include <mm/components/scale2d.hpp>
#include <mm/components/rotation2d.hpp>
#include <mm/components/velocity2d_rotation.hpp>
#include <mm/components/position3d.hpp>
#include <mm/components/transform4x4.hpp>
#include <mm/components/color.hpp>

#include <mm/systems/simple_velocity_system2d.hpp>
#include <mm/systems/transform.hpp>

#include <mm/opengl/fbo_builder.hpp>
#include <mm/opengl/texture_loader.hpp>

#include <mm/random/srng.hpp>

using namespace entt::literals;

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

#if 0
		// albedo
		rm_t.reload<MM::OpenGL::TextureLoaderEmpty>(
			"albedo",
#ifdef MM_OPENGL_3_GLES
			GL_RGB565,
#else
			GL_RGBA8, // waste of A
#endif
			w, h,
			GL_RGB, GL_UNSIGNED_BYTE
		);
#endif

#if 0
		// normal
		rm_t.reload<MM::OpenGL::TextureLoaderEmpty>(
			"normal",
			// prolly fine, but need to remapp to -1,1
#ifdef MM_OPENGL_3_GLES
			GL_RGB565,
#else
			GL_RGBA8, // waste of A
#endif
			w, h,
			GL_RGB, GL_UNSIGNED_BYTE
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

TEST(hdr_bloom_pipeline, it) {
	MM::Engine engine;

	auto& sdl_ss = engine.addService<MM::Services::SDLService>();
	ASSERT_TRUE(engine.enableService<MM::Services::SDLService>());

	sdl_ss.createGLWindow("hdr_bloom_pipeline_example", 1280, 720);

	engine.addService<MM::Services::OrganizerSceneService>();
	ASSERT_TRUE(engine.enableService<MM::Services::OrganizerSceneService>());

	bool provide_ret = engine.provide<MM::Services::SceneServiceInterface, MM::Services::OrganizerSceneService>();
	ASSERT_TRUE(provide_ret);
	auto& scene = engine.tryService<MM::Services::SceneServiceInterface>()->getScene();

	engine.addService<MM::Services::FilesystemService>(argv0, "hdr_bloom_pipeline_example");
	ASSERT_TRUE(engine.enableService<MM::Services::FilesystemService>());

	auto& rs = engine.addService<MM::Services::OpenGLRenderer>();
	ASSERT_TRUE(engine.enableService<MM::Services::OpenGLRenderer>());

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

		{ // render, whatever
			MM::OpenGL::RenderTasks::SimpleRect& bsrr_rend = rs.addRenderTask<MM::OpenGL::RenderTasks::SimpleRect>(engine);
			bsrr_rend.target_fbo = "game_view";
		}

		// rn does rt too
		MM::OpenGL::setup_bloom(engine);

		// not part of setup_bloom
		auto& comp = rs.addRenderTask<MM::OpenGL::RenderTasks::Composition>(engine);
		comp.color_tex = "hdr_color";
		comp.bloom_tex = "blur_tmp1";
		comp.target_fbo = "display";
	}

	scene.on_construct<MM::Components::Position2D>().connect<&entt::registry::emplace_or_replace<MM::Components::Position2D_ZOffset>>();
	scene.on_construct<MM::Components::Position2D>().connect<&entt::registry::emplace_or_replace<MM::Components::Position3D>>();
	scene.on_construct<MM::Components::Position2D>().connect<&entt::registry::emplace_or_replace<MM::Components::Transform4x4>>();
	scene.on_construct<MM::Components::Position2D>().connect<&entt::registry::emplace_or_replace<MM::Components::DirtyTransformTag>>();

	scene.on_update<MM::Components::Position2D>().connect<&entt::registry::emplace_or_replace<MM::Components::DirtyTransformTag>>();
	scene.on_update<MM::Components::Position2D_ZOffset>().connect<&entt::registry::emplace_or_replace<MM::Components::DirtyTransformTag>>();
	scene.on_update<MM::Components::Position3D>().connect<&entt::registry::emplace_or_replace<MM::Components::DirtyTransformTag>>();
	scene.on_update<MM::Components::Scale2D>().connect<&entt::registry::emplace_or_replace<MM::Components::DirtyTransformTag>>();
	scene.on_update<MM::Components::Rotation2D>().connect<&entt::registry::emplace_or_replace<MM::Components::DirtyTransformTag>>(); // in this example only rotation is touched

	auto& cam = scene.ctx().emplace<MM::OpenGL::Camera3D>();
	cam.setOrthographic();
	cam.updateView();

	// setup v system
	auto& org = scene.ctx().emplace<entt::organizer>();
	org.emplace<MM::Systems::simple_rotational_velocity_patching>("simple_rotational_velocity_patching");
	org.emplace<MM::Systems::position3d_from_2d>("position3d_from_2d");
	org.emplace<MM::Systems::transform3d_translate>("transform3d_translate");
	org.emplace<MM::Systems::transform3d_rotate2d>("transform3d_rotate2d");
	org.emplace<MM::Systems::transform3d_scale2d>("transform3d_scale2d");
	org.emplace<MM::Systems::transform_clear_dirty>("transform_clear_dirty");


	// HACK: instead you would switch to this scene
	engine.getService<MM::Services::OrganizerSceneService>().updateOrganizerVertices(scene);

	MM::Random::SRNG rng{42};

	for (int i = 0; i < 5; i++) {
		auto e = scene.create();
		auto& p = scene.emplace<MM::Components::Position2D>(e);
		p.pos.x = 0.f;
		p.pos.y = 25.f - i * 10.f;

		auto& s = scene.emplace<MM::Components::Scale2D>(e);
		s.scale = {50.f, i*0.2f + 0.1f};

		auto& col = scene.emplace<MM::Components::Color>(e);
		col.color = {3.f, 3.f, 3.f, 1.f};
	}

	for (int i = 0; i < 10; i++) {
		auto e = scene.create();
		auto& p = scene.emplace<MM::Components::Position2D>(e);
		p.pos.x = i * 9.f - 40;

		// zoffset is created by event

		auto& s = scene.emplace<MM::Components::Scale2D>(e);
		s.scale = {5,5};

		scene.emplace<MM::Components::Rotation2D>(e);

		auto& v = scene.emplace<MM::Components::Velocity2DRotation>(e);
		v.rot_vel = i * 0.3f;

		auto& col = scene.emplace<MM::Components::Color>(e);
		col.color = {rng.zeroToOne()*2.f, rng.zeroToOne()*2.f, rng.zeroToOne()*2.f, 1.f};
	}

	engine.run();
}

int main(int argc, char** argv) {
	argv0 = argv[0];

	::testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}

