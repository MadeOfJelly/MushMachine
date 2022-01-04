#include "./position2d.hpp"

#include <imgui/imgui.h>

namespace MM::ImGuiWidgets::Components {

void Position(MM::Components::Position2D& p) {
	ImGui::DragFloat2("position (x,y)##Position2D", &p.pos.x, 0.1f);
}

} // MM::ImGuiWidgets::Components

namespace MM {

template <>
void ComponentEditorWidget<Components::Position2D>(MM::Scene& reg, MM::Entity e) {
	ImGuiWidgets::Components::Position(reg.get<Components::Position2D>(e));
}

} // MM

