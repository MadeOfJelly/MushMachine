#pragma once

#include <mm/components/color.hpp>
#include <mm/imgui/imgui_entt_entity_editor.hpp>
#include <mm/engine_fwd.hpp>

namespace MM::ImGuiWidgets::Components {
	void Color(MM::Components::Color& c);
}

namespace MM {
	template <>
	void ComponentEditorWidget<Components::Color>(MM::Scene& reg, MM::Entity e);
}

