#include <gtest/gtest.h>

#include <mm/engine.hpp>

#include <mm/services/filesystem.hpp>
#include <mm/services/sdl_service.hpp>
#include <mm/services/organizer_scene.hpp>
#include <mm/services/opengl_renderer.hpp>

#include <entt/entity/registry.hpp>
#include <entt/entity/organizer.hpp>

#include <mm/opengl/texture_loader.hpp>

#include <mm/opengl/render_tasks/simple_spritesheet.hpp>

#include <mm/components/position2d.hpp>
#include <mm/components/position2d_zoffset.hpp>
#include <mm/components/scale2d.hpp>
#include <mm/components/position3d.hpp>
#include <mm/components/transform4x4.hpp>
#include <mm/components/color.hpp>
#include <mm/components/time_delta.hpp>
#include <mm/opengl/render_tasks/spritesheet_renderable.hpp>

#include <mm/systems/transform.hpp>

#include <physfs.h>
#include "res/textures.zip.h"

using namespace entt::literals;

void update_spritesheet_animation(entt::view<entt::get_t<MM::OpenGL::SpriteSheetRenderable>> view, float& accu, const MM::Components::TimeDelta& td) {
	accu += td.tickDelta;

	if (accu >= 1.f/10) {
		accu -= 1.f/10;
		view.each([](auto& spr) {
			spr.tile_index = (spr.tile_index+spr.sp.tile_count.x) % (spr.sp.tile_count.x*spr.sp.tile_count.y);
		});
	}
}

TEST(simple_spritesheet_render_task, it) {
	MM::Engine engine;

	auto& sdl_ss = engine.addService<MM::Services::SDLService>();
	ASSERT_TRUE(engine.enableService<MM::Services::SDLService>());

	sdl_ss.createGLWindow("simple_spritesheet_render_task_test", 1280, 720);

	engine.addService<MM::Services::OrganizerSceneService>();
	ASSERT_TRUE(engine.enableService<MM::Services::OrganizerSceneService>());

	bool provide_ret = engine.provide<MM::Services::SceneServiceInterface, MM::Services::OrganizerSceneService>();
	ASSERT_TRUE(provide_ret);
	auto& scene = engine.tryService<MM::Services::SceneServiceInterface>()->getScene();

	engine.addService<MM::Services::FilesystemService>(nullptr, "simple_spritesheet_render_task_test");
	ASSERT_TRUE(engine.enableService<MM::Services::FilesystemService>());

	ASSERT_TRUE(PHYSFS_mountMemory(textures_zip, textures_zip_len, NULL, "", NULL, 0));

	auto& rs = engine.addService<MM::Services::OpenGLRenderer>();
	ASSERT_TRUE(engine.enableService<MM::Services::OpenGLRenderer>());

	auto& cam = scene.set<MM::OpenGL::Camera3D>();
	cam.horizontalViewPortSize = 5;
	cam.setOrthographic();
	cam.updateView();

	rs.addRenderTask<MM::OpenGL::RenderTasks::SimpleSpriteSheet>(engine);

	scene.on_construct<MM::Components::Position2D>().connect<&entt::registry::emplace_or_replace<MM::Components::Position2D_ZOffset>>();
	scene.on_construct<MM::Components::Position2D>().connect<&entt::registry::emplace_or_replace<MM::Components::Position3D>>();
	scene.on_construct<MM::Components::Position2D>().connect<&entt::registry::emplace_or_replace<MM::Components::Transform4x4>>();
	scene.on_construct<MM::Components::Position2D>().connect<&entt::registry::emplace_or_replace<MM::Components::DirtyTransformTag>>();

	scene.on_update<MM::Components::Position2D>().connect<&entt::registry::emplace_or_replace<MM::Components::DirtyTransformTag>>();
	scene.on_update<MM::Components::Position2D_ZOffset>().connect<&entt::registry::emplace_or_replace<MM::Components::DirtyTransformTag>>();
	scene.on_update<MM::Components::Position3D>().connect<&entt::registry::emplace_or_replace<MM::Components::DirtyTransformTag>>();
	scene.on_update<MM::Components::Scale2D>().connect<&entt::registry::emplace_or_replace<MM::Components::DirtyTransformTag>>();

	// setup systems
	scene.set<float>(0.f); // accu
	auto& org = scene.set<entt::organizer>();
	org.emplace<&update_spritesheet_animation>("update_spritesheet_animation");
	org.emplace<MM::Systems::position3d_from_2d>("position3d_from_2d");
	org.emplace<MM::Systems::transform3d_translate>("transform3d_translate");
	org.emplace<MM::Systems::transform3d_rotate2d>("transform3d_rotate2d");
	org.emplace<MM::Systems::transform3d_scale2d>("transform3d_scale2d");
	org.emplace<MM::Systems::transform_clear_dirty>("transform_clear_dirty");


	// HACK: instead you would switch to this scene
	engine.getService<MM::Services::OrganizerSceneService>().updateOrganizerVertices(scene);

	auto& rm_t = MM::ResourceManager<MM::OpenGL::Texture>::ref();
	ASSERT_TRUE(rm_t.load<MM::OpenGL::TextureLoaderFile>("anim_run", engine, "/textures/animation_running-1_ea_0.3.png"));
	ASSERT_TRUE(rm_t.load<MM::OpenGL::TextureLoaderFile>("anim_idle", engine, "/textures/animation_standing-1_ea_0.1.png"));

	{
		auto e = scene.create();
		auto& p = scene.emplace<MM::Components::Position2D>(e);
		p.pos.x = -1.f;
	
		// zoffset is created by event

		auto& s = scene.emplace<MM::Components::Scale2D>(e);
		s.scale = {1.5f,2.f};

		auto& spr = scene.emplace<MM::OpenGL::SpriteSheetRenderable>(e);
		spr.sp.tex = rm_t.get("anim_run"_hs);
		spr.sp.tile_count.x = 8;
		spr.sp.tile_count.y = 8;
		spr.tile_index = 2;
	}

	{
		auto e = scene.create();
		auto& p = scene.emplace<MM::Components::Position2D>(e);
		p.pos.x = 1.f;

		// zoffset is created by event

		auto& s = scene.emplace<MM::Components::Scale2D>(e);
		s.scale = {1.5f,2.f};

		auto& spr = scene.emplace<MM::OpenGL::SpriteSheetRenderable>(e);
		spr.sp.tex = rm_t.get("anim_idle"_hs);
		spr.sp.tile_count.x = 8;
		spr.sp.tile_count.y = 4;
		spr.tile_index = 0;
	}

	engine.run();
}

