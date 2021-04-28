#include "mm/services/service.hpp"
#include <gtest/gtest.h>

#include <mm/engine.hpp>

#include <mm/services/sdl_service.hpp>
#include <mm/services/opengl_renderer.hpp>
#include <mm/services/imgui_s.hpp>
#include <mm/opengl/render_tasks/imgui.hpp>

#include <entt/core/hashed_string.hpp>

#include <imgui/imgui.h>

using namespace entt::literals;

TEST(imgui_render_task, demowindow) {
	MM::Engine engine;

	engine.addService<MM::Services::SDLService>();
	ASSERT_TRUE(engine.enableService<MM::Services::SDLService>());

	auto& rs = engine.addService<MM::Services::OpenGLRenderer>();
	ASSERT_TRUE(engine.enableService<MM::Services::OpenGLRenderer>());

	// needs a open window (and opengl context) which the OpenGLRenderer creates in this case
	engine.addService<MM::Services::ImGuiService>();
	ASSERT_TRUE(engine.enableService<MM::Services::ImGuiService>());

	class ImGuiDemoWindowService : public MM::Services::Service {
		public:
			const char* name(void) override { return "ImGuiDemoWindowService"; }

			bool enable(MM::Engine&, std::vector<MM::UpdateStrategies::TaskInfo>& task_array) override {
				task_array.push_back(
					MM::UpdateStrategies::TaskInfo{"ImGuiDemoWindow"}
					.fn([](MM::Engine&) { ImGui::ShowDemoWindow(); })
				);
				return true;
			}

			void disable(MM::Engine&) override {}
	};
	engine.addService<ImGuiDemoWindowService>();
	ASSERT_TRUE(engine.enableService<ImGuiDemoWindowService>());

	rs.addRenderTask<MM::OpenGL::RenderTasks::ImGuiRT>(engine);

	engine.run();
}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}

