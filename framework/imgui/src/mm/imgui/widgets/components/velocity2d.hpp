#pragma once

#include <mm/components/velocity2d.hpp>
#include <mm/imgui/imgui_entt_entity_editor.hpp>
#include <mm/services/scene_service_interface.hpp>

namespace MM::ImGuiWidgets::Components {
	void Velocity(MM::Components::Velocity2D& v);
}

namespace MM {
	template <>
	void ComponentEditorWidget<Components::Velocity2D>(MM::Scene& reg, MM::Entity e);
}

