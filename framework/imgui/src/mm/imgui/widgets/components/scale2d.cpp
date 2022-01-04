#include "./scale2d.hpp"

#include <imgui/imgui.h>

namespace MM::ImGuiWidgets::Components {

void Scale(MM::Components::Scale2D& s) {
	ImGui::DragFloat2("scale (x,y)##Scale2D", &s.scale.x, 0.01f);
}

} // MM::ImGuiWidgets::Components

namespace MM {

template <>
void ComponentEditorWidget<Components::Scale2D>(MM::Scene& reg, MM::Entity e) {
	ImGuiWidgets::Components::Scale(reg.get<Components::Scale2D>(e));
}

} // MM

