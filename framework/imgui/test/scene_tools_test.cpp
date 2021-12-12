#include <gtest/gtest.h>

#include <mm/resource_manager.hpp>

#include <mm/engine.hpp>

#include <mm/services/sdl_service.hpp>
#include <mm/services/filesystem.hpp>
#include <mm/services/organizer_scene.hpp>
#include <mm/services/imgui_s.hpp>
#include <mm/services/imgui_menu_bar.hpp>
#include <mm/services/engine_tools.hpp>

#include <mm/services/opengl_renderer.hpp>
#include <mm/opengl/render_tasks/imgui.hpp>

#include <mm/services/scene_tools.hpp>

static char* argv0;

using namespace entt::literals;

TEST(imgui_scene_tools, it) {
	MM::Engine engine;

	auto& sdl_ss = engine.addService<MM::Services::SDLService>(SDL_INIT_VIDEO);
	ASSERT_TRUE(engine.enableService<MM::Services::SDLService>());

	sdl_ss.createGLWindow("imgui_scene_tools_test", 1280, 720);

	engine.addService<MM::Services::FilesystemService>(argv0, "imgui_scene_tools_test");
	ASSERT_TRUE(engine.enableService<MM::Services::FilesystemService>());

	engine.addService<MM::Services::OrganizerSceneService>();
	ASSERT_TRUE(engine.enableService<MM::Services::OrganizerSceneService>());

	bool provide_ret = engine.provide<MM::Services::SceneServiceInterface, MM::Services::OrganizerSceneService>();
	ASSERT_TRUE(provide_ret);

	engine.addService<MM::Services::ImGuiService>();
	ASSERT_TRUE(engine.enableService<MM::Services::ImGuiService>());

	engine.addService<MM::Services::ImGuiMenuBar>();
	ASSERT_TRUE(engine.enableService<MM::Services::ImGuiMenuBar>());

	engine.addService<MM::Services::ImGuiEngineTools>();
	ASSERT_TRUE(engine.enableService<MM::Services::ImGuiEngineTools>());

	engine.addService<MM::Services::ImGuiSceneToolsService>();
	//engine.getUpdateStrategy().depend("ImGuiSceneToolsService::render"_hs, "SimpleSceneService::scene_tick"_hs);

	auto& rs = engine.addService<MM::Services::OpenGLRenderer>();
	ASSERT_TRUE(engine.enableService<MM::Services::OpenGLRenderer>());

	ASSERT_TRUE(engine.enableService<MM::Services::ImGuiSceneToolsService>());

	rs.addRenderTask<MM::OpenGL::RenderTasks::ImGuiRT>(engine);

	engine.run();


	// TODO: clear asset manager

	sdl_ss.destroyWindow();
}

int main(int argc, char** argv) {
	argv0 = argv[0];

	::testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}

