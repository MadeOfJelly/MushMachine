#pragma once

#include <mm/components/rotation2d.hpp>
#include <mm/services/scene_service_interface.hpp>
#include <mm/imgui/imgui_entt_entity_editor.hpp>

namespace MM::ImGuiWidgets::Components {
	void Rotation(MM::Components::Rotation2D& r);
}

namespace MM {
	template <>
	void ComponentEditorWidget<Components::Rotation2D>(MM::Scene& reg, MM::Entity e);
}

