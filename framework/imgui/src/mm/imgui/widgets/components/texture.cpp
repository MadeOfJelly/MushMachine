#include "texture.hpp"
#include "../texture.hpp"

#include <imgui/imgui.h>

namespace MM::ImGuiWidgets::Components {

void Texture(MM::Components::OpenGL::Texture& tex) {
	LabelTexture("tex##Texture", tex.tex);
}

}

namespace MM {

template <>
void ComponentEditorWidget<Components::OpenGL::Texture>(MM::Scene& reg, MM::Entity e) {
	ImGuiWidgets::Components::Texture(reg.get<Components::OpenGL::Texture>(e));
}

} // MM

