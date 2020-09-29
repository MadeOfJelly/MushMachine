#pragma once

#include <mm/components/view_dir2d.hpp>
#include <mm/imgui/imgui_entt_entity_editor.hpp>
#include <mm/services/scene_service_interface.hpp>

namespace MM::ImGuiWidgets::Components {
	void ViewDir(MM::Components::ViewDir2D& t);
}

namespace MM {
	template <>
	void ComponentEditorWidget<Components::ViewDir2D>(MM::Scene& reg, MM::Entity e);
}

