#include "./velocity2d_position.hpp"

#include <imgui/imgui.h>

namespace MM::ImGuiWidgets::Components {

void VelocityPosition(MM::Components::Velocity2DPosition& v) {
	ImGui::DragFloat2("velocity (x,y)##Velocity2DPosition", &v.pos_vel.x, 0.1f);
}

} // MM::ImGuiWidgets::Components

namespace MM {

template <>
void ComponentEditorWidget<Components::Velocity2DPosition>(MM::Scene& reg, MM::Entity e) {
	ImGuiWidgets::Components::VelocityPosition(reg.get<Components::Velocity2DPosition>(e));
}

} // MM

