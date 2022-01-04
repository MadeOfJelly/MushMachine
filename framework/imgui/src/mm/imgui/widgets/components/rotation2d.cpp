#include "./rotation2d.hpp"

#include <imgui/imgui.h>

#include <glm/gtc/constants.hpp>

namespace MM::ImGuiWidgets::Components {

void Rotation(MM::Components::Rotation2D& r) {
	ImGui::SliderFloat("rotation##Rotation2D", &r.rot, 0.f, glm::two_pi<float>());
}

} // MM::ImGuiWidgets::Components

namespace MM {

template <>
void ComponentEditorWidget<Components::Rotation2D>(MM::Scene& reg, MM::Entity e) {
	ImGuiWidgets::Components::Rotation(reg.get<Components::Rotation2D>(e));
}

} // MM

