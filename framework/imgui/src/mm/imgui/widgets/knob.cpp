#include "./knob.hpp"

#include <imgui/imgui.h>

#include <cmath>

namespace MM::ImGuiWidgets {

// this implementation is base on:
// https://github.com/Flix01/imgui/blob/imgui_with_addons/addons/imguivariouscontrols/imguivariouscontrols.cpp#L4119
// and has been modified.
//
// LICENSE of original code:
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
bool KnobFloat(const char* label, float* v, float v_min, float v_max, float step, bool show_tooltip) {
	//@ocornut https://github.com/ocornut/imgui/issues/942
	ImGuiIO& io = ImGui::GetIO();
	ImGuiStyle& style = ImGui::GetStyle();

	float radius_outer = 20.0f;
	ImVec2 pos = ImGui::GetCursorScreenPos();
	ImVec2 center = ImVec2(pos.x + radius_outer, pos.y + radius_outer);
	float line_height = ImGui::GetTextLineHeight();
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	float ANGLE_MIN = 3.141592f * 0.75f;
	float ANGLE_MAX = 3.141592f * 2.25f;

	ImGui::InvisibleButton(label, ImVec2(radius_outer*2, radius_outer*2 + line_height + style.ItemInnerSpacing.y));
	bool value_changed = false;
	bool is_active = ImGui::IsItemActive();
	bool is_hovered = ImGui::IsItemHovered();
	if (is_active && io.MouseDelta.x != 0.0f)   {
		if (step<=0) step=50.f;
		float tmp_step = (v_max - v_min) / step;
		*v += io.MouseDelta.x * tmp_step;

		if (*v < v_min) *v = v_min;
		if (*v > v_max) *v = v_max;
		value_changed = true;
	} else if (is_hovered && (io.MouseDoubleClicked[0] || io.MouseClicked[2])) {
		*v = (v_max + v_min) * 0.5f;  // reset value
		value_changed = true;
	}

	float t = (*v - v_min) / (v_max - v_min);
	float angle = ANGLE_MIN + (ANGLE_MAX - ANGLE_MIN) * t;
	float angle_cos = cosf(angle), angle_sin = sinf(angle);
	float radius_inner = radius_outer*0.60f;

	draw_list->AddCircleFilled(center, radius_outer, ImGui::GetColorU32(ImGuiCol_FrameBg), 16);
	draw_list->AddLine(ImVec2(center.x + angle_cos*radius_inner, center.y + angle_sin*radius_inner), ImVec2(center.x + angle_cos*(radius_outer-2), center.y + angle_sin*(radius_outer-2)), ImGui::GetColorU32(ImGuiCol_SliderGrabActive), 2.0f);
	draw_list->AddCircleFilled(center, radius_inner, ImGui::GetColorU32(is_active ? ImGuiCol_FrameBgActive : is_hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg), 16);
	draw_list->AddText(ImVec2(pos.x, pos.y + radius_outer * 2 + style.ItemInnerSpacing.y), ImGui::GetColorU32(ImGuiCol_Text), label);

	if (show_tooltip && (is_active || is_hovered))	{
		ImGui::SetNextWindowPos(ImVec2(pos.x - style.WindowPadding.x, pos.y - line_height - style.ItemInnerSpacing.y - style.WindowPadding.y));
		ImGui::BeginTooltip();
		ImGui::Text("%.3f", *v);
		ImGui::EndTooltip();
	}

	return value_changed;
}

} // MM::ImGuiWidgets

