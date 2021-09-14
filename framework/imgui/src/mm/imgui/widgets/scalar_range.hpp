#pragma once

#include <mm/scalar_range2.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <limits>
#include <cstdint>

namespace MM::ImGuiWidgets {

	// cedits: this code is copied from DearImGui and modified

	// NB: You likely want to specify the ImGuiSliderFlags_AlwaysClamp when using this.
	template<typename T>
	bool DragScalarRange2EXT(const char* label, ImGuiDataType data_type, T* v_current_min, T* v_current_max, float v_speed = 1.f, T v_min = {}, T v_max = {}, const char* format = NULL, const char* format_max = NULL, ImGuiSliderFlags flags = 0)
	{
		ImGuiContext& g = *ImGui::GetCurrentContext();
		auto* window = g.CurrentWindow;
		if (window->SkipItems)
			return false;

		ImGui::PushID(label);
		ImGui::BeginGroup();
		ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());

		T min_min = (v_min >= v_max) ? std::numeric_limits<T>::min() : v_min;
		T min_max = (v_min >= v_max) ? *v_current_max : ImMin(v_max, *v_current_max);
		ImGuiSliderFlags min_flags = flags | ((min_min == min_max) ? ImGuiSliderFlags_ReadOnly : 0);
		bool value_changed = ImGui::DragScalar("##min", data_type, v_current_min, v_speed, &min_min, &min_max, format, min_flags);
		ImGui::PopItemWidth();
		ImGui::SameLine(0, g.Style.ItemInnerSpacing.x);

		T max_min = (v_min >= v_max) ? *v_current_min : ImMax(v_min, *v_current_min);
		T max_max = (v_min >= v_max) ? std::numeric_limits<T>::max() : v_max;
		ImGuiSliderFlags max_flags = flags | ((max_min == max_max) ? ImGuiSliderFlags_ReadOnly : 0);
		value_changed |= ImGui::DragScalar("##max", data_type, v_current_max, v_speed, &max_min, &max_max, format_max ? format_max : format, max_flags);
		ImGui::PopItemWidth();
		ImGui::SameLine(0, g.Style.ItemInnerSpacing.x);

		ImGui::TextEx(label, ImGui::FindRenderedTextEnd(label));
		ImGui::EndGroup();
		ImGui::PopID();
		return value_changed;
	}

	// scalar range
	template<typename T>
	bool DragScalarRange2(const char* label, ScalarRange2<T>& range, T v_min = {}, T v_max = {});

	// specializations
	// unsigned int
	template<>
	bool DragScalarRange2(const char* label, ScalarRange2<uint8_t>& range, uint8_t v_min , uint8_t v_max);
	template<>
	bool DragScalarRange2(const char* label, ScalarRange2<uint16_t>& range, uint16_t v_min , uint16_t v_max);
	template<>
	bool DragScalarRange2(const char* label, ScalarRange2<uint32_t>& range, uint32_t v_min , uint32_t v_max);
	template<>
	bool DragScalarRange2(const char* label, ScalarRange2<uint64_t>& range, uint64_t v_min , uint64_t v_max);

	// signed int
	template<>
	bool DragScalarRange2(const char* label, ScalarRange2<int8_t>& range, int8_t v_min , int8_t v_max);
	template<>
	bool DragScalarRange2(const char* label, ScalarRange2<int16_t>& range, int16_t v_min , int16_t v_max);
	template<>
	bool DragScalarRange2(const char* label, ScalarRange2<int32_t>& range, int32_t v_min , int32_t v_max);
	template<>
	bool DragScalarRange2(const char* label, ScalarRange2<int64_t>& range, int64_t v_min , int64_t v_max);

	// float, double
	template<>
	bool DragScalarRange2(const char* label, ScalarRange2<float>& range, float v_min , float v_max);
	template<>
	bool DragScalarRange2(const char* label, ScalarRange2<double>& range, double v_min , double v_max);

} // MM::ImGuiWidgets

