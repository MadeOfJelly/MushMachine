#include "color.hpp"

#include <imgui/imgui.h>

namespace MM::ImGuiWidgets::Components {

void Color(MM::Components::Color& c) {
	ImGui::ColorEdit4("color##Color", &c.color[0]);
}

}

namespace MM {

template <>
void ComponentEditorWidget<Components::Color>(MM::Scene& reg, MM::Entity e) {
	ImGuiWidgets::Components::Color(reg.get<Components::Color>(e));
}

} // MM

