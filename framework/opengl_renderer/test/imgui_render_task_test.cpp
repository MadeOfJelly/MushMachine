#include <gtest/gtest.h>

#include <mm/engine.hpp>

#include <mm/services/sdl_service.hpp>
#include <mm/services/opengl_renderer.hpp>
#include <mm/services/imgui_s.hpp>
#include <mm/opengl/render_tasks/imgui.hpp>

#include <imgui/imgui.h>

TEST(imgui_render_task, demowindow) {
	MM::Engine engine;

	engine.addService<MM::Services::SDLService>();
	ASSERT_TRUE(engine.enableService<MM::Services::SDLService>());

	auto& rs = engine.addService<MM::Services::OpenGLRenderer>();
	ASSERT_TRUE(engine.enableService<MM::Services::OpenGLRenderer>());

	// needs a open window (and opengl context) which the OpenGLRenderer creates in this case
	engine.addService<MM::Services::ImGuiService>();
	ASSERT_TRUE(engine.enableService<MM::Services::ImGuiService>());

	rs.addRenderTask<MM::OpenGL::RenderTasks::ImGuiRT>(engine);

	auto handle = engine.addUpdate([](MM::Engine&) {
			ImGui::ShowDemoWindow();
		}
	);

	{
		auto tmp_lock = handle.lock();
		tmp_lock->priority = 0;
		tmp_lock->name = "imgui demo window";
	}

	engine.run();
}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}

