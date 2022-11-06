#include <gtest/gtest.h>

#include <mm/engine.hpp>

#include <mm/services/filesystem.hpp>
#include <mm/services/sdl_service.hpp>
#include <mm/services/organizer_scene.hpp>
#include <mm/services/opengl_renderer.hpp>
#include <mm/services/count_down.hpp>

#include <entt/entity/registry.hpp>
#include <entt/entity/organizer.hpp>

#include <mm/opengl/camera_3d.hpp>
#include <mm/opengl/render_tasks/tilemap.hpp>

#include <mm/opengl/render_tasks/tilemap_renderable.hpp>
#include <mm/components/position2d.hpp>
#include <mm/components/position2d_zoffset.hpp>
#include <mm/components/position3d.hpp>
#include <mm/components/transform4x4.hpp>

#include <mm/systems/transform.hpp>
#include <mm/opengl/texture_loader.hpp>

using namespace entt::literals;

TEST(tilemap_render_task_test, it) {
	MM::Engine engine;

	auto& sdl_ss = engine.addService<MM::Services::SDLService>();
	ASSERT_TRUE(engine.enableService<MM::Services::SDLService>());

	sdl_ss.createGLWindow("tilemap_render_task_test", 1280, 720);

	engine.addService<MM::Services::OrganizerSceneService>();
	ASSERT_TRUE(engine.enableService<MM::Services::OrganizerSceneService>());

	bool provide_ret = engine.provide<MM::Services::SceneServiceInterface, MM::Services::OrganizerSceneService>();
	ASSERT_TRUE(provide_ret);
	auto& scene = engine.tryService<MM::Services::SceneServiceInterface>()->getScene();

	auto& cam = scene.ctx().emplace<MM::OpenGL::Camera3D>();
	cam.translation = {2.f, -2.f, 0.f};
	cam.horizontalViewPortSize = 20.f;
	cam.setOrthographic();
	cam.updateView();

	engine.addService<MM::Services::FilesystemService>(nullptr, "tilemap_render_task_test");
	ASSERT_TRUE(engine.enableService<MM::Services::FilesystemService>());

	auto& rs = engine.addService<MM::Services::OpenGLRenderer>();
	ASSERT_TRUE(engine.enableService<MM::Services::OpenGLRenderer>());

#ifdef MM_AUTOTEST
	engine.addService<MM::Services::CountDown>(50); // 50 frames
	ASSERT_TRUE(engine.enableService<MM::Services::CountDown>());
#endif

	rs.addRenderTask<MM::OpenGL::RenderTasks::Tilemap>(engine);

	scene.on_construct<MM::Components::Position2D>().connect<&entt::registry::emplace_or_replace<MM::Components::Position2D_ZOffset>>();
	scene.on_construct<MM::Components::Position2D>().connect<&entt::registry::emplace_or_replace<MM::Components::Position3D>>();
	scene.on_construct<MM::Components::Position2D>().connect<&entt::registry::emplace_or_replace<MM::Components::Transform4x4>>();
	scene.on_construct<MM::Components::Position3D>().connect<&entt::registry::emplace_or_replace<MM::Components::DirtyTransformTag>>(); // fun

	// "useless" in this example
	scene.on_update<MM::Components::Position2D>().connect<&entt::registry::emplace_or_replace<MM::Components::DirtyTransformTag>>();
	scene.on_update<MM::Components::Position2D_ZOffset>().connect<&entt::registry::emplace_or_replace<MM::Components::DirtyTransformTag>>();
	scene.on_update<MM::Components::Position3D>().connect<&entt::registry::emplace_or_replace<MM::Components::DirtyTransformTag>>();

	auto& org = scene.ctx().emplace<entt::organizer>();
	org.emplace<MM::Systems::position3d_from_2d>("position3d_from_2d");
	org.emplace<MM::Systems::transform3d_translate>("transform3d_translate");
	org.emplace<MM::Systems::transform_clear_dirty>("transform_clear_dirty");

	// HACK: instead you would switch to this scene
	engine.getService<MM::Services::OrganizerSceneService>().updateOrganizerVertices(scene);


	auto& rm_t = MM::ResourceManager<MM::OpenGL::Texture>::ref();

	{
		auto e = scene.create();
		scene.emplace<MM::Components::Position2D>(e);

		auto& tm = scene.emplace<MM::OpenGL::TilemapRenderable>(e);

		auto& slayer = tm.sprite_layer.emplace_back();

		// fill sprite sheet
		{
			slayer.sprite_sheet.tile_count.x = 1;
			slayer.sprite_sheet.tile_count.y = 1;
			//slayer.sprite_sheet.tex = rm_t.get("default"_hs);
			slayer.sprite_sheet.tex = rm_t.get("errig"_hs);
		}

		// fill tiles
		{
			std::vector<uint32_t> temp_map {
				1, 1, 1, 1, 1,
				1, 0, 0, 0, 1,
				1, 0, 1, 0, 1,
				1, 0, 0, 0, 1,
				1, 1, 1, 1, 1,
			};

			uint32_t width = 5;
			uint32_t height = 5;

			for (uint32_t y = 0; y < height; y++) {
				for (uint32_t x = 0; x < width; x++) {
					if (temp_map[y*width + x] != 0) {
						auto& tile = slayer.map.emplace_back();
						tile.pos[0] = x;
						tile.pos[1] = y;
						tile.sprite_sheet_index = temp_map[y*width + x] - 1;
					}
				}
			}

			slayer.syncMapBuffer(); // send to opengl
		}
	}

	engine.run();
}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

