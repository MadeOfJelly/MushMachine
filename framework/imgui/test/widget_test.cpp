#include <gtest/gtest.h>

#include <mm/engine.hpp>

#include <entt/core/hashed_string.hpp>

// services
#include <mm/services/sdl_service.hpp>
#include <mm/services/filesystem.hpp>
#include <mm/services/opengl_renderer.hpp>
#include <mm/services/imgui_s.hpp>

#include <mm/opengl/render_tasks/imgui.hpp>

#include <imgui/imgui.h>

#include <mm/imgui/widgets/knob.hpp>

const char* argv0;

using namespace entt::literals;

TEST(imgui_widgets, basic) {
	MM::Engine engine;

	auto& sdl_ss = engine.addService<MM::Services::SDLService>();
	ASSERT_TRUE(engine.enableService<MM::Services::SDLService>());

	sdl_ss.createGLWindow("imgui_widget_test", 1280, 720);

	engine.addService<MM::Services::FilesystemService>(argv0, "imgui_widget_test");
	ASSERT_TRUE(engine.enableService<MM::Services::FilesystemService>());

	engine.addService<MM::Services::ImGuiService>();
	ASSERT_TRUE(engine.enableService<MM::Services::ImGuiService>());

	auto& rs = engine.addService<MM::Services::OpenGLRenderer>();
	ASSERT_TRUE(engine.enableService<MM::Services::OpenGLRenderer>());

	rs.addRenderTask<MM::OpenGL::RenderTasks::ImGuiRT>(engine);

	class TestWindow : public MM::Services::Service {
		public:
			const char* name(void) override { return "TestWindow"; }
			bool enable(MM::Engine&) override { return true; }
			void disable(MM::Engine&) override {}

			std::vector<MM::UpdateStrategies::UpdateCreationInfo> registerUpdates(void) override {
				return {{
					"testwindow"_hs,
					"testwindow",
					[](MM::Engine&) {
						if (ImGui::Begin("test window")) {

							static float knob_test = 0.f;
							MM::ImGuiWidgets::KnobFloat("knob1", &knob_test, 0.f, 1.f);
							ImGui::SameLine();
							MM::ImGuiWidgets::KnobFloat("knob2", &knob_test, 0.f, 1.f, 0.f, false);

						}
						ImGui::End();
					}
				}};
			}
	};

	engine.addService<TestWindow>();
	ASSERT_TRUE(engine.enableService<TestWindow>());

	engine.run();

	sdl_ss.destroyWindow();
}

int main(int argc, char** argv) {
	argv0 = argv[0];

	::testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}

