#include "./plot_radar.hpp"

#include <glm/common.hpp> // min max
#include <glm/trigonometric.hpp> // sin cos
#include <glm/gtc/constants.hpp> // two_pi

namespace MM::ImGuiWidgets {

void PlotRadar(
	const char* label,
	const float* values, int values_count,
	const char** vlabels,
	float scale_min, float scale_max,
	ImVec2 graph_size,

	bool draw_net_first,

	// polygon
	const ImVec4& poly_line_color,
	const ImVec4& poly_fill_color,
	const float poly_line_thickness,

	// net
	const ImVec4& net_line_color,
	const float net_line_thicknes
) {
	if (values == nullptr || values_count == 0) {
		return;
	}

	// set up scale
	if (scale_min == FLT_MAX) {
		scale_min = values[0];
		for (int i = 1; i < values_count; i++) {
			scale_min = glm::min(scale_min, values[i]);
		}
	}
	if (scale_max == FLT_MAX) {
		scale_max = values[0];
		for (int i = 1; i < values_count; i++) {
			scale_max = glm::max(scale_max, values[i]);
		}
	}
	if (scale_min == scale_max) {
		return;
	}

	ImGui::PushID(label);

	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	const ImVec2 p = ImGui::GetCursorScreenPos();

	const ImVec2 center_p {
		p.x + graph_size.x/2,
		p.y + graph_size.y/2,
	};

	const float inner_spacing_from_center = (graph_size.x/2) * 0.15f; // TODO: expose?
	const float outer_spacing_from_center = (graph_size.x/2) * 0.8f; // TODO: expose?

	auto draw_ngon = [&](const ImVec2& center, float radius, ImU32 col, int num_segments, float thickness) {
		if ((col & IM_COL32_A_MASK) == 0 || num_segments <= 2)
			return;

		for (int i = 0; i < num_segments; i++) {
			const float a0 = (float(i) / (num_segments-1)) * (glm::two_pi<float>()) * (num_segments-1 / float(num_segments)) - glm::half_pi<float>();
			const float a1 = (float(i+1) / (num_segments-1)) * (glm::two_pi<float>()) * (num_segments-1 / float(num_segments)) - glm::half_pi<float>();

			draw_list->AddLine(
				{center.x + glm::cos(a0) * radius, center.y + glm::sin(a0) * radius},
				{center.x + glm::cos(a1) * radius, center.y + glm::sin(a1) * radius},
				col, thickness
			);
		}
	};

	auto draw_net = [&]() {
		// inner ring
		draw_ngon(center_p, inner_spacing_from_center, ImColor(net_line_color), values_count, net_line_thicknes);

		// depends on min and max
		draw_ngon(center_p, glm::mix(inner_spacing_from_center, outer_spacing_from_center, 0.5), ImColor(net_line_color), values_count, net_line_thicknes);

		// outer ring
		draw_ngon(center_p, outer_spacing_from_center, ImColor(net_line_color), values_count, net_line_thicknes);

		// axies
		for (int i = 0; i < values_count; i++) {
			const float a = (float(i) / (values_count-1)) * (glm::two_pi<float>()) * (values_count-1 / float(values_count)) - glm::half_pi<float>();
			draw_list->AddLine(
				{center_p.x + glm::cos(a) * inner_spacing_from_center, center_p.y + glm::sin(a) * inner_spacing_from_center},
				{center_p.x + glm::cos(a) * outer_spacing_from_center, center_p.y + glm::sin(a) * outer_spacing_from_center},
				ImColor(net_line_color), net_line_thicknes
			);
		}

		// lables
		// TODO
	};

	auto value_to_radius = [&](float v) -> float {
		//v -= scale_min;
		//v /= scale_max;
		//return v;
		return (v - scale_min) / scale_max;
	};

	auto draw_polygon_fills = [&]() {
		// fills
		for (int i = 0; i < values_count; i++) {
			const float a0 = (float(i) / (values_count-1)) * (glm::two_pi<float>()) * (values_count-1 / float(values_count)) - glm::half_pi<float>();
			const float a1 = (float(i+1) / (values_count-1)) * (glm::two_pi<float>()) * (values_count-1 / float(values_count)) - glm::half_pi<float>();

			float v0 = values[i];
			float v1 = values[(i+1) % values_count];

			v0 = value_to_radius(v0);
			v1 = value_to_radius(v1);

			float r0 = glm::mix(inner_spacing_from_center, outer_spacing_from_center, v0);
			float r1 = glm::mix(inner_spacing_from_center, outer_spacing_from_center, v1);

			draw_list->AddQuadFilled(
				{center_p.x + glm::cos(a0) * r0, center_p.y + glm::sin(a0) * r0},
				{center_p.x + glm::cos(a1) * r1, center_p.y + glm::sin(a1) * r1},

				{center_p.x + glm::cos(a1) * inner_spacing_from_center, center_p.y + glm::sin(a1) * inner_spacing_from_center},
				{center_p.x + glm::cos(a0) * inner_spacing_from_center, center_p.y + glm::sin(a0) * inner_spacing_from_center},
				ImColor(poly_fill_color)
			);
		}
	};
	auto draw_polygon_lines = [&]() {
		// lines
		for (int i = 0; i < values_count; i++) {
			const float a0 = (float(i) / (values_count-1)) * (glm::two_pi<float>()) * (values_count-1 / float(values_count)) - glm::half_pi<float>();
			const float a1 = (float(i+1) / (values_count-1)) * (glm::two_pi<float>()) * (values_count-1 / float(values_count)) - glm::half_pi<float>();

			float v0 = values[i];
			float v1 = values[(i+1) % values_count];

			v0 = value_to_radius(v0);
			v1 = value_to_radius(v1);

			float r0 = glm::mix(inner_spacing_from_center, outer_spacing_from_center, v0);
			float r1 = glm::mix(inner_spacing_from_center, outer_spacing_from_center, v1);

			draw_list->AddLine(
				{center_p.x + glm::cos(a0) * r0, center_p.y + glm::sin(a0) * r0},
				{center_p.x + glm::cos(a1) * r1, center_p.y + glm::sin(a1) * r1},
				ImColor(poly_line_color), poly_line_thickness
			);
		}
	};

	auto draw_text = [&]() {
		if (vlabels == nullptr) {
			return;
		}

		for (int i = 0; i < values_count; i++) {
			const float a0 = (float(i) / (values_count-1)) * (glm::two_pi<float>()) * (values_count-1 / float(values_count)) - glm::half_pi<float>();

			const float text_spacing_from_center = (graph_size.x/2) * 0.9f;
			draw_list->AddText(
				{
					center_p.x + glm::cos(a0) * text_spacing_from_center,
					center_p.y + glm::sin(a0) * text_spacing_from_center - ImGui::GetTextLineHeight()/2
				},
				ImColor(1.f,1.f,1.f,1.f), vlabels[i]
			);
		}
	};

	if (draw_net_first) {
		draw_net();
		draw_polygon_fills();
		draw_polygon_lines();
		draw_text();
	} else {
		draw_polygon_fills();
		draw_net();
		draw_polygon_lines();
		draw_text();
	}

	ImGui::PopID();
}

} // MM::ImGuiWidgets

