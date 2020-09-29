#pragma once

#include <mm/components/transform3d.hpp>
#include <mm/imgui/imgui_entt_entity_editor.hpp>
#include <mm/services/scene_service_interface.hpp>

namespace MM::ImGuiWidgets::Components {
	void Transform(MM::Components::Transform3D& t);
}

namespace MM {
	template <>
	void ComponentEditorWidget<Components::Transform3D>(MM::Scene& reg, MM::Entity e);
}

