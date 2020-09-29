#include "name.hpp"

#include <imgui/imgui.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

namespace MM::ImGuiWidgets::Components {

// TODO: make editable
void Name(MM::Components::Name& n) {
	if (ImGui::InputText("str##Name", &n.str)) {
		n.str = n.str.substr(0, MM::Components::Name::max_str_len); // limit size
	}
}

}

namespace MM {

template <>
void ComponentEditorWidget<Components::Name>(MM::Scene& reg, MM::Entity e) {
	ImGuiWidgets::Components::Name(reg.get<Components::Name>(e));
}

} // MM

