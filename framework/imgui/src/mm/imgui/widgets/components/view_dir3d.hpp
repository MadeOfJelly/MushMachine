#pragma once

#include <mm/components/view_dir3d.hpp>
#include <mm/imgui/imgui_entt_entity_editor.hpp>
#include <mm/services/scene_service_interface.hpp>

namespace MM::ImGuiWidgets::Components {
	void ViewDir(MM::Components::ViewDir3D& t);
}

namespace MM {
	template <>
	void ComponentEditorWidget<Components::ViewDir3D>(MM::Scene& reg, MM::Entity e);
}

