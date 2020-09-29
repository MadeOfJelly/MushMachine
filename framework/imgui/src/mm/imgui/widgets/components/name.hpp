#pragma once

#include <mm/components/name.hpp>
#include <mm/imgui/imgui_entt_entity_editor.hpp>
#include <mm/services/scene_service_interface.hpp>

namespace MM::ImGuiWidgets::Components {
	void Name(MM::Components::Name& n);
}

namespace MM {
	template <>
	void ComponentEditorWidget<Components::Name>(MM::Scene& reg, MM::Entity e);
}

