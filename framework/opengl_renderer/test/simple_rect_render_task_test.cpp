#include <gtest/gtest.h>

#include <mm/engine.hpp>

#include <mm/services/filesystem.hpp>
#include <mm/services/sdl_service.hpp>
#include <mm/services/organizer_scene.hpp>
#include <mm/services/opengl_renderer.hpp>
#include <mm/services/count_down.hpp>

#include <entt/entity/registry.hpp>
#include <entt/entity/organizer.hpp>

#include <mm/opengl/render_tasks/simple_rect.hpp>

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

#ifdef MM_AUTOTEST
	engine.addService<MM::Services::CountDown>(50); // 50 frames
	ASSERT_TRUE(engine.enableService<MM::Services::CountDown>());
#endif

	rs.addRenderTask<MM::OpenGL::RenderTasks::SimpleRect>(engine);

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

	for (int y = 0; y < 10; y++) {
		for (int i = 0; i < 10; i++) {
			auto e = scene.create();
			auto& p = scene.emplace<MM::Components::Position2D>(e);
			p.pos.x = i * 9.f - 40;
			p.pos.y = -y * 6.f + 25;

			// zoffset is created by event

			auto& s = scene.emplace<MM::Components::Scale2D>(e);
			s.scale = {5,5};

			scene.emplace<MM::Components::Rotation2D>(e);

			auto& v = scene.emplace<MM::Components::Velocity2DRotation>(e);
			v.rot_vel = i * 0.3f;

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

