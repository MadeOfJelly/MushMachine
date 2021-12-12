#include <gtest/gtest.h>

#include <mm/engine.hpp>

#include <mm/services/filesystem.hpp>
#include <mm/services/sdl_service.hpp>
#include <mm/services/organizer_scene.hpp>
#include <mm/services/opengl_renderer.hpp>

#include <entt/entity/registry.hpp>
#include <entt/entity/organizer.hpp>

#include <mm/opengl/render_tasks/simple_sprite.hpp>

#include <mm/components/transform2d.hpp>
#include <mm/opengl/components/texture.hpp>
#include <mm/components/color.hpp>

#include <mm/systems/simple_velocity_system2d.hpp>

#include <random>

using namespace entt::literals;

const char* argv0;

TEST(simple_sprite_render_task, it) {
	MM::Engine engine;

	auto& sdl_ss = engine.addService<MM::Services::SDLService>();
	ASSERT_TRUE(engine.enableService<MM::Services::SDLService>());

	sdl_ss.createGLWindow("simple_sprite_render_task_test", 1280, 720);

	engine.addService<MM::Services::OrganizerSceneService>();
	ASSERT_TRUE(engine.enableService<MM::Services::OrganizerSceneService>());

	bool provide_ret = engine.provide<MM::Services::SceneServiceInterface, MM::Services::OrganizerSceneService>();
	ASSERT_TRUE(provide_ret);
	auto& scene = engine.tryService<MM::Services::SceneServiceInterface>()->getScene();

	engine.addService<MM::Services::FilesystemService>(argv0, "simple_sprite_render_task_test");
	ASSERT_TRUE(engine.enableService<MM::Services::FilesystemService>());

	auto& rs = engine.addService<MM::Services::OpenGLRenderer>();
	ASSERT_TRUE(engine.enableService<MM::Services::OpenGLRenderer>());

	rs.addRenderTask<MM::OpenGL::RenderTasks::SimpleSprite>(engine);

	// setup v system
	auto& org = scene.set<entt::organizer>();
	org.emplace<&MM::Systems::simple_velocity>("simple_velocity");

	// HACK: instead you would switch to this scene
	engine.getService<MM::Services::OrganizerSceneService>().updateOrganizerVertices(scene);

	auto& rm_t = MM::ResourceManager<MM::OpenGL::Texture>::ref();

	std::mt19937 mt(42);

	for (int y = 0; y < 10; y++) {
		for (int i = 0; i < 10; i++) {
			auto e = scene.create();
			auto& t = scene.emplace<MM::Components::Transform2D>(e);
			t.position.x = i * 9.f - 40;
			t.position.y = -y * 6.f + 25;
			t.scale = {5,5};

			auto& v = scene.emplace<MM::Components::Velocity2D>(e);
			v.rotation = i * 0.3f;

			auto& tex = scene.emplace<MM::Components::OpenGL::Texture>(e);
			tex.tex = rm_t.get("errig"_hs);

			if (mt() % 2) {
				auto& col = scene.emplace<MM::Components::Color>(e);
				auto rc = [&mt]() -> float {
					return (mt() % 1001) / 1000.f ;
				};
				col.color = {rc(),rc(),rc(),1};
			}
		}
	}

	engine.run();
}

int main(int argc, char** argv) {
	argv0 = argv[0];

	::testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}

