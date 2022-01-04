#pragma once

#include <mm/components/position2d.hpp>
#include <mm/services/scene_service_interface.hpp>
#include <mm/imgui/imgui_entt_entity_editor.hpp>

namespace MM::ImGuiWidgets::Components {
	void Position(MM::Components::Position2D& p);
}

namespace MM {
	template <>
	void ComponentEditorWidget<Components::Position2D>(MM::Scene& reg, MM::Entity e);
}

