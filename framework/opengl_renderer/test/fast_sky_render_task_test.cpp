#include <gtest/gtest.h>

#include <mm/engine.hpp>

#include <mm/services/filesystem.hpp>
#include <mm/services/sdl_service.hpp>
#include <mm/services/organizer_scene.hpp>
#include <mm/services/opengl_renderer.hpp>
#include <mm/services/count_down.hpp>

#include <entt/entity/registry.hpp>
#include <entt/entity/organizer.hpp>

#include <mm/opengl/render_tasks/fast_sky_render_task.hpp>
#include <mm/systems/fast_sky_sun_system.hpp>

#include <mm/opengl/camera_3d.hpp>
#include <mm/components/time_delta.hpp>

const char* argv0;

TEST(fast_sky_render_task, it) {
	MM::Engine engine;

	auto& sdl_ss = engine.addService<MM::Services::SDLService>();
	ASSERT_TRUE(engine.enableService<MM::Services::SDLService>());

	sdl_ss.createGLWindow("fast_sky_render_task_test", 1280, 720);

	engine.addService<MM::Services::OrganizerSceneService>();
	ASSERT_TRUE(engine.enableService<MM::Services::OrganizerSceneService>());

	bool provide_ret = engine.provide<MM::Services::SceneServiceInterface, MM::Services::OrganizerSceneService>();
	ASSERT_TRUE(provide_ret);
	auto& scene = engine.tryService<MM::Services::SceneServiceInterface>()->getScene();

	engine.addService<MM::Services::FilesystemService>(argv0, "fast_sky_render_task_test");
	ASSERT_TRUE(engine.enableService<MM::Services::FilesystemService>());

	auto& rs = engine.addService<MM::Services::OpenGLRenderer>();
	ASSERT_TRUE(engine.enableService<MM::Services::OpenGLRenderer>());

#ifdef MM_AUTOTEST
	engine.addService<MM::Services::CountDown>(50); // 50 frames
	ASSERT_TRUE(engine.enableService<MM::Services::CountDown>());
#endif

	rs.addRenderTask<MM::OpenGL::RenderTasks::FastSky>(engine);

	// setup systems
	auto& org = scene.ctx().emplace<entt::organizer>();
	org.emplace<&MM::Systems::fast_sky_sun>("fast_sky_sun");

	// HACK: instead you would switch to this scene
	engine.getService<MM::Services::OrganizerSceneService>().updateOrganizerVertices(scene);


	auto& cam = scene.ctx().emplace<MM::OpenGL::Camera3D>();
	cam.setPerspective();
	cam.updateView();

	scene.ctx().emplace<MM::OpenGL::RenderTasks::FastSkyContext>();

	engine.run();
}

int main(int argc, char** argv) {
	argv0 = argv[0];

	::testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}

