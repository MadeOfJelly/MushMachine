#include <gtest/gtest.h>

#include <mm/engine.hpp>

// services
#include <mm/services/sdl_service.hpp>
#include <mm/services/filesystem.hpp>
#include <mm/services/opengl_renderer.hpp>
#include <mm/services/imgui_s.hpp>

#include <mm/opengl/render_tasks/imgui.hpp>

#include <imgui/imgui.h>

#include <mm/imgui/widgets/knob.hpp>

const char* argv0;

//class ImGuiSpeechy {
	//private:
		//SoLoud::Speech speech;
		//SoLoud::Sfxr sfxr;

	//public:
		//ImGuiSpeechy(void) {
			//speech.setText("Test text. 1. 2. 3.");
			//sfxr.loadPreset(SoLoud::Sfxr::COIN, 0);
		//}

		//void renderImGui(MM::Engine& engine) {
			//if (ImGui::Begin("Inputs")) {
				//auto& sound = *engine.tryGetService<MM::Services::SoundService>();

				//if (ImGui::Button("play sfx")) {
					//sound.engine.play(sfxr);
				//}

				////ImGui::Text("Active Voice Count: %d", sound.engine.getActiveVoiceCount());
				//if (ImGui::Button("Read")) {
					//sound.engine.play(speech);
				//}
			//}
			//ImGui::End();

		//}
//};

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

	{
		//ImGuiSpeechy speechy;

		engine.addUpdate([&](MM::Engine&) {
			if (ImGui::Begin("test window")) {

				static float knob_test = 0.f;
				MM::ImGuiWidgets::KnobFloat("knob1", &knob_test, 0.f, 1.f);
				ImGui::SameLine();
				MM::ImGuiWidgets::KnobFloat("knob2", &knob_test, 0.f, 1.f, 0.f, false);

			}
			ImGui::End();
		});

		engine.run();
	}

	sdl_ss.destroyWindow();
}

int main(int argc, char** argv) {
	argv0 = argv[0];

	::testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}

