#pragma once

#include <functional>

#include <imgui/imgui.h>

namespace MM::ImGuiWidgets {

template<typename Iterator>
void ListWrap(Iterator begin, Iterator end, std::function<void(Iterator)> fn, float item_with) {
	ImGuiStyle& style = ImGui::GetStyle();

	float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;

	for (auto it = begin; it != end; it++) {
		//ImGui::PushID(n);
		//ImGui::Button("Box", button_sz);

		fn(it);

		float last_item_x2 = ImGui::GetItemRectMax().x;

		float next_item_x2 = last_item_x2 + style.ItemSpacing.x + item_with; // Expected position if next item was on same line

		if (it + 1 != end && next_item_x2 < window_visible_x2) {
			ImGui::SameLine();
		}

		//ImGui::PopID();
	}
}

}

