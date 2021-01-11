#include "./engine_tools.hpp"

#include <entt/config/version.h>

#include <mm/engine.hpp>
#include <entt/core/hashed_string.hpp>

#include <imgui/imgui.h>

namespace MM::Services {

	bool ImGuiEngineTools::enable(Engine& engine) {
		auto& menu_bar = engine.getService<MM::Services::ImGuiMenuBar>();

		menu_bar.menu_tree["Engine"]["Stop Engine"] = [](Engine& e) {
			ImGui::Separator();
			if (ImGui::MenuItem("Stop Engine")) {
				e.stop();
			}
		};

		menu_bar.menu_tree["Engine"]["About"] = [this](Engine&) {
			ImGui::MenuItem("About", NULL, &_show_about);
		};

		menu_bar.menu_tree["Engine"]["Services"] = [this](Engine&) {
			ImGui::MenuItem("Services", NULL, &_show_services);
		};

		return true;
	}

	void ImGuiEngineTools::disable(Engine& engine) {
		auto& menu_bar = engine.getService<MM::Services::ImGuiMenuBar>();

		menu_bar.menu_tree["Engine"].erase("Stop Engine");
		menu_bar.menu_tree["Engine"].erase("About");
		menu_bar.menu_tree["Engine"].erase("Services");
	}

	std::vector<UpdateStrategies::UpdateCreationInfo> ImGuiEngineTools::registerUpdates(void) {
	using namespace entt::literals;
		return {
			{
				"ImGuiEngineTools::render"_hs,
				"ImGuiEngineTools::render",
				[this](Engine& e){ renderImGui(e); },
				UpdateStrategies::update_phase_t::MAIN,
				true,
				{
					"ImGuiMenuBar::render"_hs
				}
			}
		};
	}

	void ImGuiEngineTools::renderImGui(Engine& engine) {
		if (_show_about) {
			renderAbout();
		}

		if (_show_services) {
			renderServices(engine);
		}
	}

	void ImGuiEngineTools::renderAbout(void) {
		if (ImGui::Begin("About##EngineTools", &_show_about)) {
			ImGui::Text("TODO");
			ImGui::Text("MushMachine");
			ImGui::Text("EnTT v%d.%d.%d", ENTT_VERSION_MAJOR, ENTT_VERSION_MINOR, ENTT_VERSION_PATCH);
		}
		ImGui::End();
	}

	void ImGuiEngineTools::renderServices(Engine& engine) {
		if (ImGui::Begin("Services##EngineTools", &_show_services)) {
			ImGui::Text("TODO: use new table api");

			for (auto& it : engine._services) {
				ImGui::Text("[%d|%s]: %s", it.first, it.second->second->name(), it.second->first ? "enabled" : "disabled");
			}
		}
		ImGui::End();
	}

} // namespace MM::Services

