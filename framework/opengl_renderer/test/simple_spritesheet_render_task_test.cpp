#include <gtest/gtest.h>

#include <mm/engine.hpp>

#include <mm/services/filesystem.hpp>
#include <mm/services/sdl_service.hpp>
#include <mm/services/simple_scene.hpp>
#include <mm/services/opengl_renderer.hpp>

#include <entt/entity/registry.hpp>

#include <mm/opengl/texture_loader.hpp>

#include <mm/opengl/render_tasks/simple_spritesheet.hpp>

#include <mm/components/transform2d.hpp>
#include <mm/components/color.hpp>
#include <mm/opengl/render_tasks/spritesheet_renderable.hpp>

#include <physfs.h>
#include "res/textures.zip.h"

using namespace entt::literals;

TEST(simple_spritesheet_render_task, it) {
	MM::Engine engine;

	auto& sdl_ss = engine.addService<MM::Services::SDLService>();
	ASSERT_TRUE(engine.enableService<MM::Services::SDLService>());

	sdl_ss.createGLWindow("simple_spritesheet_render_task_test", 1280, 720);

	engine.addService<MM::Services::SimpleSceneService>();
	ASSERT_TRUE(engine.enableService<MM::Services::SimpleSceneService>());

	bool provide_ret = engine.provide<MM::Services::SceneServiceInterface, MM::Services::SimpleSceneService>();
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

	float accu = 0.f;
	MM::AddSystemToScene(scene, [&accu](auto& scene, float delta) {
		accu += delta;

		if (accu >= 1.f/10) {
			accu -= 1.f/10;
			scene.template view<MM::OpenGL::SpriteSheetRenderable>()
				.each([](auto, auto& spr) {
						spr.tile_index = (spr.tile_index+spr.sp.tile_count.x) % (spr.sp.tile_count.x*spr.sp.tile_count.y);
					}
				);
		}
	});

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

