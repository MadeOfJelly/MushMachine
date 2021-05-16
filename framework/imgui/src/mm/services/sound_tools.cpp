#include "./sound_tools.hpp"

#include <mm/engine.hpp>
#include <mm/services/sound_service.hpp>
#include <mm/imgui/sound_info.hpp>

#include <imgui/imgui.h>

#include <mm/logger.hpp>
#define LOGIGS(x) LOG("ImGuiSceneToolsService", x)

namespace MM::Services {

	bool ImGuiSoundTools::enable(Engine& engine, std::vector<UpdateStrategies::TaskInfo>& task_array) {
		auto& menu_bar = engine.getService<MM::Services::ImGuiMenuBar>();

		//menu_bar.menu_tree["Engine"]["Stop Engine"] = [](Engine& e) {
			//ImGui::Separator();
			//if (ImGui::MenuItem("Stop Engine")) {
				//e.stop();
			//}
		//};

		menu_bar.menu_tree["Sound"]["GlobalVolume"] = [](Engine& e) {
			auto& sound_e = e.getService<MM::Services::SoundService>().engine;

			auto gvolume = sound_e.getGlobalVolume();
			ImGui::SliderFloat("Global Volume", &gvolume, 0.f, 1.f);
			sound_e.setGlobalVolume(gvolume);
		};

		menu_bar.menu_tree["Sound"]["Info"] = [this](Engine&) {
			ImGui::MenuItem("Info", NULL, &_show_info);
		};

		// add task
		task_array.push_back(
			UpdateStrategies::TaskInfo{"ImGuiSoundTools::render"}
			.fn([this](Engine& e){ renderImGui(e); })
			.phase(UpdateStrategies::update_phase_t::PRE)
			.succeed("ImGuiMenuBar::render")
		);

		return true;
	}

	void ImGuiSoundTools::disable(Engine& engine) {
		auto& menu_bar = engine.getService<MM::Services::ImGuiMenuBar>();

		menu_bar.menu_tree["Sound"].erase("GlobalVolume");
		menu_bar.menu_tree["Sound"].erase("Info");
	}

	void ImGuiSoundTools::renderImGui(Engine& engine) {
		if (_show_info) {
			MM::ImGuiSoundInfo(engine, &_show_info);
		}

		//if (_show_services) {
			//renderServices(engine);
		//}
	}

} // namespace MM::Services

