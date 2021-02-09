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
			ImGui::Text("TODO: make sortable");
			ImGui::Checkbox("edit mode", &_services_edit_mode);

			if (ImGui::BeginTable(
				"services_table",
				3,
				ImGuiTableFlags_RowBg //|
			)) {
				ImGui::TableSetupColumn("id", ImGuiTableColumnFlags_WidthFixed);
				ImGui::TableSetupColumn("name");
				ImGui::TableSetupColumn("status");
				ImGui::TableHeadersRow();

				for (auto& it : engine._services) {
					ImGui::TableNextRow();

					ImGui::TableNextColumn();
					ImGui::Text("%d", it.first);

					ImGui::TableNextColumn();
					ImGui::Text("%s", it.second->second->name());

					ImGui::TableNextColumn();
					if (_services_edit_mode) {
						ImGui::PushID(it.first);
						if (ImGui::SmallButton(it.second->first ? "enabled" : "disabled")) {
							if (it.second->first) {
								engine.disableService(it.first);
							} else {
								engine.enableService(it.first);
							}
						}
						//ImGui::SetTooltip("click to toggle!"); the heck?
						ImGui::PopID();
					} else {
						ImGui::Text("%s", it.second->first ? "enabled" : "disabled");
					}
				}

			ImGui::EndTable();
			}
		}
		ImGui::End();
	}

} // namespace MM::Services

