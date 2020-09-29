#pragma once

#include <mm/opengl/components/texture.hpp>
#include <mm/imgui/imgui_entt_entity_editor.hpp>
#include <mm/engine_fwd.hpp>

namespace MM::ImGuiWidgets::Components {

	void Texture(MM::Components::OpenGL::Texture& tex);

}

namespace MM {
	template <>
	void ComponentEditorWidget<Components::OpenGL::Texture>(MM::Scene& reg, MM::Entity e);
}

