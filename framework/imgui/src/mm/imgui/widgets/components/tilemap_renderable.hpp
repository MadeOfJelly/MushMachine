#pragma once

#include <mm/imgui/imgui_entt_entity_editor.hpp>
#include <mm/engine_fwd.hpp>

// fwd
namespace MM::OpenGL {
	struct TilemapRenderable;
}

namespace MM::ImGuiWidgets::Components {

void TilemapRenderable(MM::OpenGL::TilemapRenderable& tm_r);

}

namespace MM {
	template <>
	void ComponentEditorWidget<OpenGL::TilemapRenderable>(MM::Scene& reg, MM::Entity e);
}

