#pragma once

#include <mm/components/velocity2d_rotation.hpp>
#include <mm/imgui/imgui_entt_entity_editor.hpp>
#include <mm/services/scene_service_interface.hpp>

namespace MM::ImGuiWidgets::Components {
	void VelocityRotation(MM::Components::Velocity2DRotation& v);
}

namespace MM {
	template <>
	void ComponentEditorWidget<Components::Velocity2DRotation>(MM::Scene& reg, MM::Entity e);
}

