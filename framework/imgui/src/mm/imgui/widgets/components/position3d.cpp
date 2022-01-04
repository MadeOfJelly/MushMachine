#include "./position3d.hpp"

#include <imgui/imgui.h>

//#include <glm/gtc/constants.hpp>

namespace MM::ImGuiWidgets::Components {

void Transform(MM::Components::Position3D& p) {
	//ImGui::DragFloat3("position (x,y,z)##Transform3D", &t.position.x, 0.1f);
	ImGui::DragFloat3("position (x,y,z)##Position3D", &p.pos.x, 0.1f);
	//ImGui::SliderFloat("rotation##Transform3D", &t.rotation, 0.f, glm::pi<float>() * 2.f);
	//ImGui::DragFloat3("scale (x,y,z)##Transform3D", &t.scale.x, 1.f, 0.f);
}

} // MM::ImGuiWidgets::Components

namespace MM {

template <>
void ComponentEditorWidget<Components::Position3D>(MM::Scene& reg, MM::Entity e) {
	ImGuiWidgets::Components::Transform(reg.get<Components::Position3D>(e));
}

} // MM

