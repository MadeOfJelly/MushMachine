#include "./view_dir2d.hpp"

#include <imgui/imgui.h>

#include <glm/gtc/constants.hpp>

namespace MM::ImGuiWidgets::Components {

	void ViewDir(MM::Components::ViewDir2D& vd) {
		ImGui::SliderFloat("dir##ViewDir2D", &vd.dir, 0.0f, glm::two_pi<float>());
	}

} // MM::ImGuiWidgets::Components

namespace MM {

template <>
void ComponentEditorWidget<Components::ViewDir2D>(MM::Scene& reg, MM::Entity e) {
	ImGuiWidgets::Components::ViewDir(reg.get<Components::ViewDir2D>(e));
}

} // MM

