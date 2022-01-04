#pragma once

#include <mm/components/velocity2d_position.hpp>
#include <mm/imgui/imgui_entt_entity_editor.hpp>
#include <mm/services/scene_service_interface.hpp>

namespace MM::ImGuiWidgets::Components {
	void VelocityPosition(MM::Components::Velocity2DPosition& v);
}

namespace MM {
	template <>
	void ComponentEditorWidget<Components::Velocity2DPosition>(MM::Scene& reg, MM::Entity e);
}

