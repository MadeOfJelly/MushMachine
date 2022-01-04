#include "./tilemap_renderable.hpp"

#include <mm/opengl/render_tasks/tilemap_renderable.hpp>

#include <imgui/imgui.h>

#include <mm/imgui/widgets/spritesheet.hpp>

namespace MM::ImGuiWidgets::Components {

void TilemapRenderable(MM::OpenGL::TilemapRenderable& tm_r) {
	for (size_t i = 0; i < tm_r.sprite_layer.size(); i++) {
		ImGui::Separator();
		std::string label = "sprite_sheet##";
		label += std::to_string(i);
		MM::ImGuiWidgets::LabelSpriteSheet(label.c_str(), tm_r.sprite_layer[i].sprite_sheet);
	}
}

} // MM::ImGuiWidgets::Components

namespace MM {

template <>
void ComponentEditorWidget<OpenGL::TilemapRenderable>(MM::Scene& reg, MM::Entity e) {
	ImGuiWidgets::Components::TilemapRenderable(reg.get<OpenGL::TilemapRenderable>(e));
}

} // MM

