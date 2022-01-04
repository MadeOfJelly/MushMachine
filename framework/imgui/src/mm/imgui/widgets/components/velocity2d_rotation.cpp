#include "./velocity2d_rotation.hpp"

#include <imgui/imgui.h>

namespace MM::ImGuiWidgets::Components {

void VelocityRotation(MM::Components::Velocity2DRotation& v) {
	ImGui::DragFloat("rotation##Velocity2DRotation", &v.rot_vel);
}

} // MM::ImGuiWidgets::Components

namespace MM {

template <>
void ComponentEditorWidget<Components::Velocity2DRotation>(MM::Scene& reg, MM::Entity e) {
	ImGuiWidgets::Components::VelocityRotation(reg.get<Components::Velocity2DRotation>(e));
}

} // MM

