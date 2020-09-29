#pragma once

#include <mm/components/transform2d.hpp>
#include <mm/services/scene_service_interface.hpp>
#include <mm/imgui/imgui_entt_entity_editor.hpp>

namespace MM::ImGuiWidgets::Components {
	void Transform(MM::Components::Transform2D& t);
}

namespace MM {
	template <>
	void ComponentEditorWidget<Components::Transform2D>(MM::Scene& reg, MM::Entity e);
}

