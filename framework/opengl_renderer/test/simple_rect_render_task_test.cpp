#include <gtest/gtest.h>

#include <mm/engine.hpp>

#include <mm/services/filesystem.hpp>
#include <mm/services/sdl_service.hpp>
#include <mm/services/organizer_scene.hpp>
#include <mm/services/opengl_renderer.hpp>

#include <entt/entity/registry.hpp>
#include <entt/entity/organizer.hpp>

#include <mm/opengl/render_tasks/simple_rect.hpp>

#include <mm/components/transform2d.hpp>
#include <mm/components/color.hpp>

#include <mm/systems/simple_velocity_system2d.hpp>

#include <mm/random/srng.hpp>

const char* argv0;

TEST(simple_rect_render_task, it) {
	MM::Engine engine;

	auto& sdl_ss = engine.addService<MM::Services::SDLService>();
	ASSERT_TRUE(engine.enableService<MM::Services::SDLService>());

	sdl_ss.createGLWindow("simple_rect_render_task_test", 1280, 720);

	engine.addService<MM::Services::OrganizerSceneService>();
	ASSERT_TRUE(engine.enableService<MM::Services::OrganizerSceneService>());

	bool provide_ret = engine.provide<MM::Services::SceneServiceInterface, MM::Services::OrganizerSceneService>();
	ASSERT_TRUE(provide_ret);
	auto& scene = engine.tryService<MM::Services::SceneServiceInterface>()->getScene();

	engine.addService<MM::Services::FilesystemService>(argv0, "simple_rect_render_task_test");
	ASSERT_TRUE(engine.enableService<MM::Services::FilesystemService>());

	auto& rs = engine.addService<MM::Services::OpenGLRenderer>();
	ASSERT_TRUE(engine.enableService<MM::Services::OpenGLRenderer>());

	rs.addRenderTask<MM::OpenGL::RenderTasks::SimpleRect>(engine);

	// setup v system
	auto& org = scene.set<entt::organizer>();
	org.emplace<&MM::Systems::simple_velocity>("simple_velocity");

	// HACK: instead you would switch to this scene
	engine.getService<MM::Services::OrganizerSceneService>().updateOrganizerVertices(scene);

	MM::Random::SRNG rng{42};

	for (int y = 0; y < 10; y++) {
		for (int i = 0; i < 10; i++) {
			auto e = scene.create();
			auto& t = scene.emplace<MM::Components::Transform2D>(e);
			t.position.x = i * 9.f - 40;
			t.position.y = -y * 6.f + 25;
			t.scale = {5,5};

			auto& v = scene.emplace<MM::Components::Velocity2D>(e);
			v.rotation = i * 0.3f;

			if (rng.roll(0.5f)) {
				auto& col = scene.emplace<MM::Components::Color>(e);
				col.color = {rng.zeroToOne(), rng.zeroToOne(), rng.zeroToOne(), 1.f};
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

