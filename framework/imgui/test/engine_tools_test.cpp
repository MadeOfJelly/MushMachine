#include "mm/services/imgui_menu_bar.hpp"
#include <gtest/gtest.h>

#include <mm/engine.hpp>

#include <mm/services/sdl_service.hpp>
#include <mm/services/filesystem.hpp>
#include <mm/services/imgui_s.hpp>

#include <mm/services/opengl_renderer.hpp>
#include <mm/opengl/render_tasks/imgui.hpp>

#include <mm/services/engine_tools.hpp>

static char* argv0;

TEST(imgui_scene_tools, it) {
	MM::Engine engine;

	auto& sdl_ss = engine.addService<MM::Services::SDLService>(SDL_INIT_VIDEO);
	ASSERT_TRUE(engine.enableService<MM::Services::SDLService>());

	sdl_ss.createGLWindow("imgui_engine_tools_test", 1280, 720);

	engine.addService<MM::Services::FilesystemService>(argv0, "imgui_engine_tools_test");
	ASSERT_TRUE(engine.enableService<MM::Services::FilesystemService>());

	engine.addService<MM::Services::ImGuiService>();
	ASSERT_TRUE(engine.enableService<MM::Services::ImGuiService>());

	engine.addService<MM::Services::ImGuiMenuBar>();
	ASSERT_TRUE(engine.enableService<MM::Services::ImGuiMenuBar>());

	engine.addService<MM::Services::ImGuiEngineTools>();
	ASSERT_TRUE(engine.enableService<MM::Services::ImGuiEngineTools>());

	auto& rs = engine.addService<MM::Services::OpenGLRenderer>();
	ASSERT_TRUE(engine.enableService<MM::Services::OpenGLRenderer>());

	rs.addRenderTask<MM::OpenGL::RenderTasks::ImGuiRT>(engine);

	engine.run();

	sdl_ss.destroyWindow();

}

int main(int argc, char** argv) {
	argv0 = argv[0];

	::testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}

