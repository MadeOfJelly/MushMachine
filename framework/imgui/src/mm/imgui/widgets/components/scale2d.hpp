#pragma once

#include <mm/components/scale2d.hpp>
#include <mm/services/scene_service_interface.hpp>
#include <mm/imgui/imgui_entt_entity_editor.hpp>

namespace MM::ImGuiWidgets::Components {
	void Scale(MM::Components::Scale2D& s);
}

namespace MM {
	template <>
	void ComponentEditorWidget<Components::Scale2D>(MM::Scene& reg, MM::Entity e);
}

