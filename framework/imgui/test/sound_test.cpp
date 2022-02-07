#include <gtest/gtest.h>

#include <mm/engine.hpp>

// services
#include <mm/services/sdl_service.hpp>
#include <mm/services/filesystem.hpp>
#include <mm/services/sound_service.hpp>
#include <mm/services/opengl_renderer.hpp>
#include <mm/services/imgui_s.hpp>
#include <mm/services/imgui_menu_bar.hpp>
#include <mm/services/engine_tools.hpp>

#include <mm/opengl/render_tasks/imgui.hpp>

#include <mm/services/sound_tools.hpp>

#include <mm/imgui/sound_info.hpp>
#include <mm/imgui/sound_pref.hpp>
#include <mm/imgui/widgets/soloud.hpp>

#include <imgui/imgui.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <soloud_speech.h>
#include <soloud_sfxr.h>
#include <soloud_freeverbfilter.h>
#include <soloud_echofilter.h>
#include <soloud_lofifilter.h>

#include <mm/soloud_json.hpp>

#include <nlohmann/json.hpp>

const char* argv0;

using namespace entt::literals;

class ImGuiSpeechy : public MM::Services::Service {
	private:
		SoLoud::Speech speech;
		SoLoud::Sfxr sfxr;

		SoLoud::FreeverbFilter freeverb;
		SoLoud::EchoFilter echo;
		SoLoud::LofiFilter lofi;

	public:
		const char* name(void) override { return "TestWindow"; }

		bool enable(MM::Engine& engine, std::vector<MM::UpdateStrategies::TaskInfo>& task_array) override {
			auto& sound = engine.getService<MM::Services::SoundService>().engine;

			speech.setText("Test text. 1. 2. 3.");
			sfxr.loadPreset(SoLoud::Sfxr::COIN, 0);

			sound.setGlobalFilter(0, &lofi);
			sound.setGlobalFilter(1, &echo);
			sound.setGlobalFilter(2, &freeverb);

			task_array.push_back(
				MM::UpdateStrategies::TaskInfo{"testwindow"}
				.fn([this](MM::Engine& engine) { renderImGui(engine); })
			);

			return true;
		}

		void disable(MM::Engine&) override {}

		void renderImGui(MM::Engine& engine) {
			auto& sound = *engine.tryService<MM::Services::SoundService>();


			if (ImGui::Begin("Inputs")) {
				if (ImGui::Button("play sfx")) {
					sound.engine.play(sfxr);
				}

				//ImGui::Text("Active Voice Count: %d", sound.engine.getActiveVoiceCount());
				if (ImGui::Button("Read")) {
					sound.engine.play(speech);
				}
			}
			ImGui::End();

			if (ImGui::Begin("Sfxr plain")) {
				MM::ImGuiWidgets::SoLoudSfxrPlain("plain_sfxr", &sfxr.mParams);
			}
			ImGui::End();

			if (ImGui::Begin("Sfxr fancy")) {
				MM::ImGuiWidgets::SoLoudSfxrFancy("fancy_sfxr", &sfxr.mParams);
			}
			ImGui::End();

			if (ImGui::Begin("filter lofi")) {
				MM::ImGuiWidgets::SoLoudFilterLiveParams("filter lofi", &sound.engine, &lofi, 0);
			}
			ImGui::End();

			if (ImGui::Begin("filter echo")) {
				MM::ImGuiWidgets::SoLoudFilterLiveParams("filter echo", &sound.engine, &echo, 1);
			}
			ImGui::End();

			if (ImGui::Begin("filter freeverb")) {
				MM::ImGuiWidgets::SoLoudFilterLiveParams("filter freeverb", &sound.engine, &freeverb, 2);
			}
			ImGui::End();

			if (ImGui::Begin("filter fancy lofi")) {
				ImGui::TextDisabled("LoFi");
				MM::ImGuiWidgets::SoLoudFilterLiveParamsFancy("filter fancy lofi", &sound.engine, &lofi, 0);
			}
			ImGui::End();

			if (ImGui::Begin("filter fancy echo")) {
				ImGui::TextDisabled("Echo");
				MM::ImGuiWidgets::SoLoudFilterLiveParamsFancy("filter fancy echo", &sound.engine, &echo, 1);
			}
			ImGui::End();

			if (ImGui::Begin("filter fancy freeverb")) {
				ImGui::TextDisabled("Rev");
				MM::ImGuiWidgets::SoLoudFilterLiveParamsFancy("filter fancy freeverb", &sound.engine, &freeverb, 2);
			}
			ImGui::End();

			if (ImGui::Begin("Sfxr json")) {
				std::string json_text = nlohmann::json(sfxr).dump(2);
				ImGui::InputTextMultiline("json", &json_text);
			}
			ImGui::End();
		}
};

TEST(imgui_sound, basic) {
	MM::Engine engine;

	auto& sdl_ss = engine.addService<MM::Services::SDLService>();
	ASSERT_TRUE(engine.enableService<MM::Services::SDLService>());

	sdl_ss.createGLWindow("imgui_sound_test", 800, 600);

	engine.addService<MM::Services::FilesystemService>(argv0, "imgui_sound_test");
	ASSERT_TRUE(engine.enableService<MM::Services::FilesystemService>());

	auto& sound = engine.addService<MM::Services::SoundService>();
	ASSERT_TRUE(engine.enableService<MM::Services::SoundService>());

	sound.engine.setVisualizationEnable(true);

	engine.addService<MM::Services::ImGuiService>();
	ASSERT_TRUE(engine.enableService<MM::Services::ImGuiService>());

	engine.addService<MM::Services::ImGuiMenuBar>();
	ASSERT_TRUE(engine.enableService<MM::Services::ImGuiMenuBar>());

	engine.addService<MM::Services::ImGuiEngineTools>();
	ASSERT_TRUE(engine.enableService<MM::Services::ImGuiEngineTools>());

	engine.addService<MM::Services::ImGuiSoundTools>();
	ASSERT_TRUE(engine.enableService<MM::Services::ImGuiSoundTools>());

	auto& rs = engine.addService<MM::Services::OpenGLRenderer>();
	ASSERT_TRUE(engine.enableService<MM::Services::OpenGLRenderer>());

	rs.addRenderTask<MM::OpenGL::RenderTasks::ImGuiRT>(engine);

	engine.addService<ImGuiSpeechy>();
	ASSERT_TRUE(engine.enableService<ImGuiSpeechy>());

	engine.run();

	sdl_ss.destroyWindow();
}

int main(int argc, char** argv) {
	argv0 = argv[0];

	::testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}

