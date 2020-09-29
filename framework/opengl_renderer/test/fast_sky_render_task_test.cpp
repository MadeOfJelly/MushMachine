#include "mm/opengl/camera_3d.hpp"
#include <gtest/gtest.h>

#include <mm/engine.hpp>

#include <mm/services/filesystem.hpp>
#include <mm/services/sdl_service.hpp>
#include <mm/services/simple_scene.hpp>
#include <mm/services/opengl_renderer.hpp>

#include <entt/entity/registry.hpp>

#include <mm/opengl/render_tasks/fast_sky_render_task.hpp>
#include <mm/systems/fast_sky_sun_system.hpp>

const char* argv0;

TEST(fast_sky_render_task, it) {
	MM::Engine engine;

	auto& sdl_ss = engine.addService<MM::Services::SDLService>();
	ASSERT_TRUE(engine.enableService<MM::Services::SDLService>());

	sdl_ss.createGLWindow("fast_sky_render_task_test", 1280, 720);

	engine.addService<MM::Services::SimpleSceneService>();
	ASSERT_TRUE(engine.enableService<MM::Services::SimpleSceneService>());

	bool provide_ret = engine.provide<MM::Services::SceneServiceInterface, MM::Services::SimpleSceneService>();
	ASSERT_TRUE(provide_ret);
	auto& scene = engine.tryService<MM::Services::SceneServiceInterface>()->getScene();

	engine.addService<MM::Services::FilesystemService>(argv0, "fast_sky_render_task_test");
	ASSERT_TRUE(engine.enableService<MM::Services::FilesystemService>());

	auto& rs = engine.addService<MM::Services::OpenGLRenderer>();
	ASSERT_TRUE(engine.enableService<MM::Services::OpenGLRenderer>());

	rs.addRenderTask<MM::OpenGL::RenderTasks::FastSky>(engine);

	MM::AddSystemToScene(scene, MM::Systems::FastSkySun);

	auto& cam = scene.set<MM::OpenGL::Camera3D>();
	cam.setPerspective();
	cam.updateView();

	scene.set<MM::OpenGL::RenderTasks::FastSkyContext>();

	engine.run();
}

int main(int argc, char** argv) {
	argv0 = argv[0];

	::testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}

