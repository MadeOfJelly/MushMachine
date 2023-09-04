// for the license, see the end of the file
#pragma once

#include <imgui.h>
#include <nlohmann/json.hpp>
#include <imgui/misc/cpp/imgui_stdlib.h>
#include <cinttypes>

namespace MM::ImGuiWidgets {

static const ImVec4 bool_true_color{0.2f, 1.0f, 0.3f, 1.f};
static const ImVec4 bool_false_color{1.0f, 0.2f, 0.3f, 1.f};
static const ImVec4 string_color{0.8f, 0.8f, 0.3f, 1.f};
static const ImVec4 int_color{0.6f, 0.3f, 0.8f, 1.f};
static const ImVec4 unsigned_color{0.3f, 0.6f, 0.8f, 1.f};
static const ImVec4 float_color{0.3f, 0.3f, 0.8f, 1.f};

// takes const, only displays, no tree, super simple
template <typename JsonObjectType = nlohmann::ordered_json>
void JsonViewerSimple(const char* name, const JsonObjectType& json) {
	ImGui::PushID(name);

	ImGui::Text("%s:", name);

	if (ImGui::IsItemHovered()) {
		ImGui::BeginTooltip();
		ImGui::TextUnformatted(json.type_name());
		ImGui::EndTooltip();
	}

	ImGui::SameLine();

	switch (json.type()) {
		case JsonObjectType::value_t::null:
			ImGui::TextDisabled("NULL");
			break;
		case JsonObjectType::value_t::boolean:
			if (json.template get<bool>()) {
				ImGui::TextColored(bool_true_color, "true");
			} else {
				ImGui::TextColored(bool_false_color, "false");
			}

			//if (ImGui::IsItemClicked()) {
				//json = !json.template get<bool>();
			//}
			break;
		case JsonObjectType::value_t::string:
			{
				const std::string& str_ref = json.template get_ref<const std::string&>();
				if (str_ref.size() < 100) {
					ImGui::TextColored(string_color, "\"%s\"", str_ref.c_str());
				} else {
					ImGui::TextDisabled("!! text too long to display !!");
				}
			}
			break;
		case JsonObjectType::value_t::number_integer:
			ImGui::TextColored(int_color, "%" PRId64, json.template get<int64_t>());
			break;
		case JsonObjectType::value_t::number_unsigned:
			ImGui::TextColored(unsigned_color, "%" PRIu64, json.template get<uint64_t>());
			break;
		case JsonObjectType::value_t::number_float:
			ImGui::TextColored(float_color, "%f", json.template get<float>());
			break;
		case JsonObjectType::value_t::object:
			ImGui::TextDisabled("{%zu}", json.size());

			ImGui::Indent();
			for (auto& [key, value] : json.items()) {
				JsonViewerSimple(key.c_str(), value);
			}
			ImGui::Unindent();

			break;
		case JsonObjectType::value_t::array:
			ImGui::TextDisabled("[%zu]", json.size());

			ImGui::Indent();
			for (auto& [key, value] : json.items()) {
				JsonViewerSimple(key.c_str(), value);
			}
			ImGui::Unindent();

			break;
		case JsonObjectType::value_t::binary:
			ImGui::TextDisabled("(...) TODO: display binary");
			break;
		case JsonObjectType::value_t::discarded:
			ImGui::Text("%s DISCARDED", name); // whats this?
			break;
	}

	ImGui::PopID();
}

// takes const, only displays, tree
template <typename JsonObjectType = nlohmann::ordered_json>
void JsonViewerTree(const char* name, const JsonObjectType& json) {
	// if array or object, tree node
	if (json.is_structured()) {
		const bool tree_open = ImGui::TreeNode(name, "%s:", name);

		ImGui::SameLine();
		if (json.is_object()) {
			ImGui::TextDisabled("{%zu}", json.size());
		} else { // is_array()
			ImGui::TextDisabled("[%zu]", json.size());
		}

		if (tree_open) {
			for (auto& [key, value] : json.items()) {
				JsonViewerTree(key.c_str(), value);
			}

			ImGui::TreePop();
		}
	} else {
		ImGui::PushID(name);

		ImGui::Text("%s:", name);
		ImGui::SameLine();

		switch (json.type()) {
			case JsonObjectType::value_t::null:
				ImGui::TextDisabled("NULL");
				break;
			case JsonObjectType::value_t::boolean:
				if (json.template get<bool>()) {
					ImGui::TextColored(bool_true_color, "true");
				} else {
					ImGui::TextColored(bool_false_color, "false");
				}
				break;
			case JsonObjectType::value_t::string:
				{
					const std::string& str_ref = json.template get_ref<const std::string&>();
					if (str_ref.size() < 100) {
						ImGui::TextColored(string_color, "\"%s\"", str_ref.c_str());
					} else {
						ImGui::TextDisabled("!! text too long to display !!");
					}
				}
				break;
			case JsonObjectType::value_t::number_integer:
				ImGui::TextColored(int_color, "%" PRId64, json.template get<int64_t>());
				break;
			case JsonObjectType::value_t::number_unsigned:
				ImGui::TextColored(unsigned_color, "%" PRIu64, json.template get<uint64_t>());
				break;
			case JsonObjectType::value_t::number_float:
				ImGui::TextColored(float_color, "%f", json.template get<float>());
				break;
			case JsonObjectType::value_t::binary:
				ImGui::TextDisabled("(...) TODO: display binary");
				break;
			case JsonObjectType::value_t::discarded:
				ImGui::Text("%s DISCARDED", name); // whats this?
				break;
			default:
				ImGui::Text("you should not be here....");
				break;
		}
		ImGui::PopID();
	}
}

namespace detail {

template <typename JsonObjectType = nlohmann::ordered_json>
void JsonTypeCombo(typename JsonObjectType::value_t& type) {
	const std::array<const char*, 10> type_strings {
		"null",
		"object",
		"array",
		"string",
		"boolean",
		"number_integer",
		"number_unsigned",
		"number_float",
		"binary",
		"discarded"
	};

	int tmp_index = static_cast<uint8_t>(type);

	ImGui::Combo("type", &tmp_index, type_strings.data(), type_strings.size());

	type = static_cast<typename JsonObjectType::value_t>(tmp_index);
}

} // detail

// edits, tree
template <typename JsonObjectType = nlohmann::ordered_json>
void JsonEditor(const char* name, JsonObjectType& json) {
	// if array or object, tree node
	if (json.is_structured()) {
		const bool tree_open = ImGui::TreeNode(name, "%s:", name);

		ImGui::SameLine();
		if (json.is_object()) {
			ImGui::TextDisabled("{%zu}", json.size());
		} else { // is_array()
			ImGui::TextDisabled("[%zu]", json.size());
		}

		if (tree_open) {
			size_t index = 0; // for array, and id

			// for each entry in array or object
			for (auto it = json.begin(); it != json.end(); index++) {
				ImGui::PushID(index);

				if (ImGui::SmallButton("-")) {
					it = json.erase(it);
					ImGui::PopID();
					continue;
				}

				ImGui::SameLine();

				if (json.is_object()) {
					JsonEditor(it.key().c_str(), it.value());
				} else { // is_array()
					auto tmp_key_str = std::to_string(index);
					JsonEditor(tmp_key_str.c_str(), it.value());
				}

				ImGui::PopID();
				it++;
			}

			{ // the add button
				const bool add_button = ImGui::SmallButton("+");

				static char key_buff[256] = {};
				ImGui::SameLine();
				if (json.is_object()) {
					// TODO: dynamically limit item width
					ImGui::SetNextItemWidth(200.f);
					ImGui::InputText("key", key_buff, 255);
					ImGui::SameLine();
				}

				static typename JsonObjectType::value_t type = JsonObjectType::value_t::null;
				// TODO: dynamically limit item width
				ImGui::SetNextItemWidth(200.f);
				detail::JsonTypeCombo(type);

				if (add_button) {
					if (json.is_object()) {
						json[key_buff] = type;
					} else { // is_array()
						json.push_back(type);
					}
				}
			}

			ImGui::TreePop();
		}
	} else { // not array or object
		ImGui::PushID(name);

		ImGui::Text("%s:", name);
		ImGui::SameLine();

		switch (json.type()) {
			case JsonObjectType::value_t::null:
				ImGui::TextDisabled("NULL");
				break;
			case JsonObjectType::value_t::boolean:
				if (json.template get<bool>()) {
					ImGui::TextColored(bool_true_color, "true");
				} else {
					ImGui::TextColored(bool_false_color, "false");
				}

				if (ImGui::IsItemClicked()) {
					json = !json.template get<bool>();
				}
				break;
			case JsonObjectType::value_t::string:
				{
					std::string& str_ref = json.template get_ref<std::string&>();
					ImGui::InputText("string", &str_ref);
					//if (str_ref.size() < 100) {
						//ImGui::TextColored(string_color, "\"%s\"", str_ref.c_str());
					//} else {
						//ImGui::TextDisabled("!! text too long to display !!");
					//}
				}
				break;
			case JsonObjectType::value_t::number_integer:
				//ImGui::TextColored(int_color, "%ld", json.template get_ref<int64_t&>());
				ImGui::InputScalar("integer", ImGuiDataType_S64, &json.template get_ref<int64_t&>());
				break;
			case JsonObjectType::value_t::number_unsigned:
				//ImGui::TextColored(unsigned_color, "%lu", json.template get<uint64_t>());
				ImGui::InputScalar("unsigned", ImGuiDataType_U64, &json.template get_ref<int64_t&>());
				break;
			case JsonObjectType::value_t::number_float:
				//ImGui::TextColored(float_color, "%f", json.template get<float>());
				ImGui::InputDouble("float", &(json.template get_ref<double&>()));
				break;
			case JsonObjectType::value_t::binary:
				ImGui::TextDisabled("(...) TODO: display binary");
				break;
			case JsonObjectType::value_t::discarded:
				ImGui::Text("%s DISCARDED", name); // whats this?
				break;
			default:
				ImGui::Text("you should not be here....");
				break;
		}
		ImGui::PopID();
	}
}

} // MM::ImGuiWidgets

// MIT License

// Copyright (c) 2021 Erik Scholz

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

