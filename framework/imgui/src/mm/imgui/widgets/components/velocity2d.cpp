#include "./velocity2d.hpp"

#include <imgui/imgui.h>

#include <glm/gtc/constants.hpp>

namespace MM::ImGuiWidgets::Components {

void Velocity(MM::Components::Velocity2D& v) {
	ImGui::DragFloat2("velocity (x,y)##Velocity", &v.velocity.x, 0.1f);
	ImGui::SliderFloat("rotation##Velocity", &v.rotation, -glm::two_pi<float>(), glm::two_pi<float>());
}

}

namespace MM {

template <>
void ComponentEditorWidget<Components::Velocity2D>(MM::Scene& reg, MM::Entity e) {
	ImGuiWidgets::Components::Velocity(reg.get<Components::Velocity2D>(e));
}

} // MM

