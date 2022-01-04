#include "./position2d_zoffset.hpp"

#include <imgui/imgui.h>

namespace MM::ImGuiWidgets::Components {

void Position_ZOffset(MM::Components::Position2D_ZOffset& z) {
	ImGui::SliderFloat("zoffset##Position2D_ZOffset", &z.z_offset, 0.f, 1000.f); // TODO: this depends on the camera far and near plane... somewhat
}

} // MM::ImGuiWidgets::Components

namespace MM {

template <>
void ComponentEditorWidget<Components::Position2D_ZOffset>(MM::Scene& reg, MM::Entity e) {
	ImGuiWidgets::Components::Position_ZOffset(reg.get<Components::Position2D_ZOffset>(e));
}

} // MM

