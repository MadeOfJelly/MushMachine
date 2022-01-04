#pragma once

#include <mm/components/position2d_zoffset.hpp>
#include <mm/services/scene_service_interface.hpp>
#include <mm/imgui/imgui_entt_entity_editor.hpp>

namespace MM::ImGuiWidgets::Components {
	void Position_ZOffset(MM::Components::Position2D_ZOffset& t);
}

namespace MM {
	template <>
	void ComponentEditorWidget<Components::Position2D_ZOffset>(MM::Scene& reg, MM::Entity e);
}

