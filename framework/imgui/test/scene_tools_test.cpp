#include <gtest/gtest.h>

#include <mm/resource_manager.hpp>

#include <mm/engine.hpp>

#include <mm/services/sdl_service.hpp>
#include <mm/services/filesystem.hpp>
#include <mm/services/simple_scene.hpp>
#include <mm/services/imgui_s.hpp>

#include <mm/services/opengl_renderer.hpp>
#include <mm/opengl/render_tasks/imgui.hpp>

#include <mm/imgui/fps_overlay.hpp>

#include <mm/services/scene_tools.hpp>

static char* argv0;

TEST(imgui_scene_tools, it) {
	MM::Engine engine;

	auto& sdl_ss = engine.addService<MM::Services::SDLService>(SDL_INIT_VIDEO);
	ASSERT_TRUE(engine.enableService<MM::Services::SDLService>());

	sdl_ss.createGLWindow("imgui_scene_tools_test", 1280, 720);

	engine.addService<MM::Services::FilesystemService>(argv0, "imgui_scene_tools_test");
	ASSERT_TRUE(engine.enableService<MM::Services::FilesystemService>());

	engine.addService<MM::Services::SimpleSceneService>();
	ASSERT_TRUE(engine.enableService<MM::Services::SimpleSceneService>());

	bool provide_ret = engine.provide<MM::Services::SceneServiceInterface, MM::Services::SimpleSceneService>();
	ASSERT_TRUE(provide_ret);

	engine.addService<MM::Services::ImGuiService>();
	ASSERT_TRUE(engine.enableService<MM::Services::ImGuiService>());

	engine.addService<MM::Services::ImGuiSceneToolsService>();

	auto& rs = engine.addService<MM::Services::OpenGLRenderer>();
	ASSERT_TRUE(engine.enableService<MM::Services::OpenGLRenderer>());

	ASSERT_TRUE(engine.enableService<MM::Services::ImGuiSceneToolsService>());

	rs.addRenderTask<MM::OpenGL::RenderTasks::ImGuiRT>(engine);

	//InitializeYojimbo();

	{
		MM::ImGuiSimpleFPSOverlay fps_overlay;

		engine.addUpdate([&](MM::Engine&) {
				fps_overlay.renderImGui();
			}
		);

		engine.run();
	}


	// TODO: clear asset manager

	sdl_ss.destroyWindow();

	//ShutdownYojimbo();
}

int main(int argc, char** argv) {
	argv0 = argv[0];

	::testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}

