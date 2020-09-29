#pragma once

//#include <cfloat>

#include <imgui/imgui.h>

namespace MM::ImGuiWidgets {

void PlotRadar(
	const char* label,
	const float* values, int values_count, // values offset?
	const char** vlabels = nullptr, // list of null-terminated c-strings, same size as values
	float scale_min = FLT_MAX, float scale_max = FLT_MAX,
	ImVec2 graph_size = {100, 100},

	bool draw_net_first = false,

	// polygon
	const ImVec4& poly_line_color = {0.8f, 0.8f, 0.0f, 1.0f},
	const ImVec4& poly_fill_color = {0.8f, 0.8f, 0.0f, 0.4f},
	const float poly_line_thickness = 2.f,

	// net
	const ImVec4& net_line_color = {0.9f, 0.9f, 0.9f, 0.8f},
	const float net_line_thickness = 1.f

	//bool show_tooltip = true
);

} // MM::ImGuiWidgets

