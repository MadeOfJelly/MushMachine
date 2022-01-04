#pragma once

#include <mm/components/position3d.hpp>
#include <mm/imgui/imgui_entt_entity_editor.hpp>
#include <mm/services/scene_service_interface.hpp>

namespace MM::ImGuiWidgets::Components {
	void Transform(MM::Components::Position3D& p);
}

namespace MM {
	template <>
	void ComponentEditorWidget<Components::Position3D>(MM::Scene& reg, MM::Entity e);
}

