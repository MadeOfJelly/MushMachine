#include "./view_dir3d.hpp"

#include <imgui/imgui.h>

#include <glm/gtc/constants.hpp>

namespace MM::ImGuiWidgets::Components {

	void ViewDir(MM::Components::ViewDir3D& vd) {
		ImGui::SliderFloat("yaw##ViewDir3D", &vd.yaw, 0.0f, glm::two_pi<float>());
		ImGui::SliderFloat("pitch##ViewDir3D", &vd.pitch, -glm::half_pi<float>(), glm::half_pi<float>());
		ImGui::SliderFloat("roll##ViewDir3D", &vd.roll, -glm::half_pi<float>(), glm::half_pi<float>());
	}

} // MM::ImGuiWidgets::Components

namespace MM {

template <>
void ComponentEditorWidget<Components::ViewDir3D>(MM::Scene& reg, MM::Entity e) {
	ImGuiWidgets::Components::ViewDir(reg.get<Components::ViewDir3D>(e));
}

} // MM

