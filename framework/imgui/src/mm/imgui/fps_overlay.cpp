#include "./fps_overlay.hpp"

#include <imgui/imgui.h>
#include <imgui_plot_var.hpp>

namespace MM {

void ImGuiSimpleFPSOverlay::renderImGui(void) {
	const float DISTANCE = 10.0f;

	ImVec2 window_pos = ImVec2(DISTANCE, DISTANCE + 15.f);

	ImVec2 window_pos_pivot = ImVec2(0.f, 0.f);
	ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);

	ImGui::SetNextWindowBgAlpha(0.2f);
	ImGui::Begin("SimpleFPSOverlay", NULL,
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoFocusOnAppearing |
		ImGuiWindowFlags_NoNav
	);

	ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);

	if (_show_plot) {
		ImGui::Separator();
		ImGui::PlotVar("frame time", ImGui::GetIO().DeltaTime, 0.f, 0.05f, 120);
	}

	if (ImGui::BeginPopupContextWindow()) {
		if (ImGui::MenuItem("Show Plot", NULL, _show_plot)) {
			_show_plot = !_show_plot;
		}

		ImGui::EndPopup();
	}


	ImGui::End();

	ImGui::PlotVarFlushOldEntries();
}

} // namespace MM

