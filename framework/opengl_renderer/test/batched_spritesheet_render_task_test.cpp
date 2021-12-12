#include <gtest/gtest.h>

#include <mm/engine.hpp>

#include <mm/services/filesystem.hpp>
#include <mm/services/sdl_service.hpp>
#include <mm/services/organizer_scene.hpp>
#include <mm/services/opengl_renderer.hpp>

#include <entt/entity/registry.hpp>
#include <entt/entity/organizer.hpp>

#include <mm/opengl/texture_loader.hpp>

#include <mm/opengl/render_tasks/batched_spritesheet.hpp>

#include <mm/components/transform2d.hpp>
#include <mm/components/color.hpp>
#include <mm/components/time_delta.hpp>
#include <mm/opengl/render_tasks/spritesheet_renderable.hpp>

#include <physfs.h>
#include "res/textures.zip.h"

#include <mm/logger.hpp>

using namespace entt::literals;

const char* argv0;

void update_spritesheet_animation(entt::view<entt::exclude_t<>, MM::OpenGL::SpriteSheetRenderable> view, float& accu, const MM::Components::TimeDelta& td) {
	accu += td.tickDelta;

	if (accu >= 1.f/10) {
		accu -= 1.f/10;
		view.each([](auto& spr) {
			spr.tile_index = (spr.tile_index+spr.sp.tile_count.x) % (spr.sp.tile_count.x*spr.sp.tile_count.y);
		});
	}
}

TEST(batched_spritesheet_render_task, it) {
	MM::Engine engine;

	auto& sdl_ss = engine.addService<MM::Services::SDLService>();
	ASSERT_TRUE(engine.enableService<MM::Services::SDLService>());

	sdl_ss.createGLWindow("batched_spritesheet_render_task_test", 1280, 720);

	engine.addService<MM::Services::OrganizerSceneService>();
	ASSERT_TRUE(engine.enableService<MM::Services::OrganizerSceneService>());

	bool provide_ret = engine.provide<MM::Services::SceneServiceInterface, MM::Services::OrganizerSceneService>();
	ASSERT_TRUE(provide_ret);
	auto& scene = engine.tryService<MM::Services::SceneServiceInterface>()->getScene();

	engine.addService<MM::Services::FilesystemService>(argv0, "batched_spritesheet_render_task_test");
	ASSERT_TRUE(engine.enableService<MM::Services::FilesystemService>());

	ASSERT_TRUE(PHYSFS_mountMemory(textures_zip, textures_zip_len, NULL, "", NULL, 0));

	auto& rs = engine.addService<MM::Services::OpenGLRenderer>();
	ASSERT_TRUE(engine.enableService<MM::Services::OpenGLRenderer>());

	auto& cam = scene.set<MM::OpenGL::Camera3D>();
	cam.horizontalViewPortSize = 5;
	cam.setOrthographic();
	cam.updateView();

	rs.addRenderTask<MM::OpenGL::RenderTasks::BatchedSpriteSheet>(engine);

	// setup systems
	scene.set<float>(0.f); // accu
	auto& org = scene.set<entt::organizer>();
	org.emplace<&update_spritesheet_animation>("update_spritesheet_animation");

	// HACK: instead you would switch to this scene
	engine.getService<MM::Services::OrganizerSceneService>().updateOrganizerVertices(scene);

	auto& rm_t = MM::ResourceManager<MM::OpenGL::Texture>::ref();
	ASSERT_TRUE(rm_t.load<MM::OpenGL::TextureLoaderFile>("anim_run", engine, "/textures/animation_running-1_ea_0.3.png"));
	ASSERT_TRUE(rm_t.load<MM::OpenGL::TextureLoaderFile>("anim_idle", engine, "/textures/animation_standing-1_ea_0.1.png"));

	{
		auto e = scene.create();
		auto& t = scene.emplace<MM::Components::Transform2D>(e);
		t.position.x = -1.f;
		t.scale.x = 1.5f;
		t.scale.y = 2.f;

		auto& spr = scene.emplace<MM::OpenGL::SpriteSheetRenderable>(e);
		spr.sp.tex = rm_t.get("anim_run"_hs);
		spr.sp.tile_count.x = 8;
		spr.sp.tile_count.y = 8;
		spr.tile_index = 2;
	}

	{
		auto e = scene.create();
		auto& t = scene.emplace<MM::Components::Transform2D>(e);
		t.position.x = 1.f;
		t.scale.x = 1.5f;
		t.scale.y = 2.f;

		auto& spr = scene.emplace<MM::OpenGL::SpriteSheetRenderable>(e);
		spr.sp.tex = rm_t.get("anim_idle"_hs);
		spr.sp.tile_count.x = 8;
		spr.sp.tile_count.y = 4;
		spr.tile_index = 0;
	}

	engine.run();
}

int main(int argc, char** argv) {
	argv0 = argv[0];

	::testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}

