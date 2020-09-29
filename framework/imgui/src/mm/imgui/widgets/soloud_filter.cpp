#include "./soloud.hpp"

#include <mm/imgui/widgets/knob.hpp>

#include <imgui/imgui.h>

#include <soloud.h>
#include <soloud_filter.h>

namespace MM::ImGuiWidgets {

//bool SoLoudFilterLiveParams(const char* label, SoLoud::Filter* filter, SoLoud::FilterInstance* filter_instance) {
	//if (!filter || !filter_instance) return false;
	//ImGui::PushID(label);

	//bool value_changed = false;

	//size_t param_count = filter->getParamCount();
	//for (size_t i = 0; i < param_count; i++) {
		//if (filter->getParamType(i) == SoLoud::Filter::PARAMTYPE::FLOAT_PARAM) {
			//float tmp = filter_instance->getFilterParameter(i);

			//if (ImGui::SliderFloat(
				//filter->getParamName(i),
				//&tmp,
				//filter->getParamMin(i),
				//filter->getParamMax(i)
			//)) {
				//filter_instance->setFilterParameter(i, tmp);
				//value_changed = true;
			//}


		//} else if (filter->getParamType(i) == SoLoud::Filter::PARAMTYPE::INT_PARAM) {
			//ImGui::Text("int %s min:%f max%f", filter->getParamName(i), filter->getParamMin(i), filter->getParamMax(i));
		//} else if (filter->getParamType(i) == SoLoud::Filter::PARAMTYPE::BOOL_PARAM) {
			//bool tmp = filter_instance->getFilterParameter(i) != filter->getParamMin(i);
			//if (ImGui::Checkbox(filter->getParamName(i), &tmp)) {
				//filter_instance->setFilterParameter(i, tmp ? filter->getParamMax(i) : filter->getParamMin(i));
				//value_changed = true;
			//}

		//}

	//}

	//ImGui::PopID();
	//return value_changed;
//}

bool SoLoudFilterLiveParams(const char* label, SoLoud::Soloud* soloud, SoLoud::Filter* filter, unsigned int filter_id, SoLoud::handle voice_handle) {
	if (!filter) return false;
	ImGui::PushID(label);

	bool value_changed = false;

	unsigned int param_count = static_cast<unsigned int>(filter->getParamCount());
	for (unsigned int i = 0; i < param_count; i++) {
		if (filter->getParamType(i) == SoLoud::Filter::PARAMTYPE::FLOAT_PARAM) {
			float tmp = soloud->getFilterParameter(voice_handle, filter_id, i);

			if (ImGui::SliderFloat(
				filter->getParamName(i),
				&tmp,
				filter->getParamMin(i),
				filter->getParamMax(i)
			)) {
				soloud->setFilterParameter(voice_handle, filter_id, i, tmp);
				value_changed = true;
			}


		} else if (filter->getParamType(i) == SoLoud::Filter::PARAMTYPE::INT_PARAM) {
			ImGui::Text("int %s min:%f max%f", filter->getParamName(i), filter->getParamMin(i), filter->getParamMax(i));
		} else if (filter->getParamType(i) == SoLoud::Filter::PARAMTYPE::BOOL_PARAM) {
			bool tmp = soloud->getFilterParameter(voice_handle, filter_id, i) != filter->getParamMin(i);
			if (ImGui::Checkbox(filter->getParamName(i), &tmp)) {
				soloud->setFilterParameter(voice_handle, filter_id, i, tmp ? filter->getParamMax(i) : filter->getParamMin(i));
				value_changed = true;
			}

		}

	}

	ImGui::PopID();
	return value_changed;
}

bool SoLoudFilterLiveParamsFancy(const char* label, SoLoud::Soloud* soloud, SoLoud::Filter* filter, unsigned int filter_id, SoLoud::handle voice_handle, bool same_line) {
	if (!filter) return false;
	ImGui::PushID(label);

	bool value_changed = false;

	unsigned int param_count = static_cast<unsigned int>(filter->getParamCount());
	for (unsigned int i = 0; i < param_count; i++) {
		if (filter->getParamType(i) == SoLoud::Filter::PARAMTYPE::FLOAT_PARAM) {
			float tmp = soloud->getFilterParameter(voice_handle, filter_id, i);

			if (MM::ImGuiWidgets::KnobFloat(
				filter->getParamName(i),
				&tmp,
				filter->getParamMin(i),
				filter->getParamMax(i),
				400.f
			)) {
				soloud->setFilterParameter(voice_handle, filter_id, i, tmp);
				value_changed = true;
			}


		} else if (filter->getParamType(i) == SoLoud::Filter::PARAMTYPE::INT_PARAM) {
			ImGui::Text("int %s min:%f max%f", filter->getParamName(i), filter->getParamMin(i), filter->getParamMax(i));
		} else if (filter->getParamType(i) == SoLoud::Filter::PARAMTYPE::BOOL_PARAM) {
			bool tmp = soloud->getFilterParameter(voice_handle, filter_id, i) != filter->getParamMin(i);
			if (ImGui::Checkbox(filter->getParamName(i), &tmp)) {
				soloud->setFilterParameter(voice_handle, filter_id, i, tmp ? filter->getParamMax(i) : filter->getParamMin(i));
				value_changed = true;
			}

		}

		if (same_line && (i+1 != param_count)) {
			ImGui::SameLine();
		}
	}

	ImGui::PopID();
	return value_changed;
}

} // MM::ImGuiWidgets

