#include "./sound_tools.hpp"
#include "mm/imgui/sound_info.hpp"

#include <mm/engine.hpp>
#include <entt/core/hashed_string.hpp>

#include <imgui/imgui.h>

#include <mm/services/sound_service.hpp>

#include <mm/logger.hpp>
#define LOGIGS(x) LOG("ImGuiSceneToolsService", x)

namespace MM::Services {

	bool ImGuiSoundTools::enable(Engine& engine) {
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

		return true;
	}

	void ImGuiSoundTools::disable(Engine& engine) {
		auto& menu_bar = engine.getService<MM::Services::ImGuiMenuBar>();

		menu_bar.menu_tree["Sound"].erase("GlobalVolume");
		menu_bar.menu_tree["Sound"].erase("Info");
	}

	std::vector<UpdateStrategies::UpdateCreationInfo> ImGuiSoundTools::registerUpdates(void) {
	using namespace entt::literals;
		return {
			{
				"ImGuiSoundTools::render"_hs,
				"ImGuiSoundTools::render",
				[this](Engine& e){ renderImGui(e); },
				UpdateStrategies::update_phase_t::MAIN,
				true,
				{
					"ImGuiMenuBar::render"_hs
				}
			}
		};
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

