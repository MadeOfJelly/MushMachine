#include "./screen_director_tools.hpp"

#include <mm/engine.hpp>

#include <mm/services/screen_director.hpp>

#include <imgui/imgui.h>

namespace MM::Services {

using UpdateStrategies::update_key_t;

bool ImGuiScreenDirectorTools::enable(Engine& engine, std::vector<UpdateStrategies::TaskInfo>& task_array) {
	auto& menu_bar = engine.getService<MM::Services::ImGuiMenuBar>();

	menu_bar.menu_tree["Screens"]["ShowList"] = [this](Engine&) {
		ImGui::MenuItem("ShowScreenList", NULL, &_show_screen_list);
	};

	// add task
	task_array.push_back(
		UpdateStrategies::TaskInfo{"ImGuiScreenDirectorTools::render"}
		.fn([this](Engine& e){ renderImGui(e); })
		.phase(UpdateStrategies::update_phase_t::PRE)
		.succeed("ImGuiMenuBar::render")
	);

	return true;
}

void ImGuiScreenDirectorTools::disable(Engine& engine) {
	auto& menu_bar = engine.getService<MM::Services::ImGuiMenuBar>();

	menu_bar.menu_tree["Engine"].erase("ShowList");
}

void ImGuiScreenDirectorTools::renderImGui(Engine& engine) {
	if (_show_screen_list) {
		renderScreenList(engine);
	}
}

void ImGuiScreenDirectorTools::renderScreenList(Engine& engine) {
	if (ImGui::Begin("Screens##ScreenDirectorTools", &_show_screen_list)) {
		ImGui::Text("TODO: make sortable");
		//ImGui::Checkbox("edit mode", &_services_edit_mode);

		if (ImGui::BeginTable(
			"screens_table",
			2,
			ImGuiTableFlags_RowBg //|
		)) {
			ImGui::TableSetupColumn("name"/*, ImGuiTableColumnFlags_WidthFixed*/);
			ImGui::TableSetupColumn("##goto", ImGuiTableColumnFlags_WidthFixed);
			//ImGui::TableHeadersRow();

			auto& sd = engine.getService<MM::Services::ScreenDirector>();

			const auto active_screen_color = ImGui::GetColorU32(ImVec4(0.3f, 0.7f, 0.3f, 0.4f));

			for (const auto& it : sd.screens) {
				ImGui::TableNextRow();

				if (it.first == sd.curr_screen_id) {
					ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, active_screen_color);
				}

				ImGui::TableNextColumn();
				ImGui::Text("%s", it.first.c_str());

				ImGui::TableNextColumn();
				ImGui::PushID(it.first.c_str());
				if (ImGui::SmallButton("-> GoTo")) {
					sd.queueChangeScreenTo(it.first);
				}
				//ImGui::SetTooltip("click to toggle!"); the heck?
				ImGui::PopID();
			}

		ImGui::EndTable();
		}
	}
	ImGui::End();
}

} // namespace MM::Services

