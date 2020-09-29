#include "./transform2d.hpp"
#include "mm/components/transform2d.hpp"

#include <imgui/imgui.h>

#include <glm/gtc/constants.hpp>

namespace MM::ImGuiWidgets::Components {

void Transform(MM::Components::Transform2D& t) {
	ImGui::DragFloat2("position (x,y)##Transform", &t.position.x, 0.1f);
	ImGui::SliderFloat("rotation##Transform", &t.rotation, 0.f, glm::pi<float>() * 2.f);
	ImGui::DragFloat2("scale (x,y)##Transform", &t.scale.x, 1.f, 0.f);
}

}

namespace MM {

template <>
void ComponentEditorWidget<Components::Transform2D>(MM::Scene& reg, MM::Entity e) {
	ImGuiWidgets::Components::Transform(reg.get<Components::Transform2D>(e));
}

} // MM

