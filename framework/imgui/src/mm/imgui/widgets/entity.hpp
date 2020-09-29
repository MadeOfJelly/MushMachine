#pragma once

#include <imgui/imgui.h>

#include <mm/services/scene_service_interface.hpp>

namespace MM::ImGuiWidgets {

void Entity(MM::Entity& e, MM::Scene& ecs, bool dropTarget = true);

// just a drop target
void EntityTrashCan(MM::Scene& ecs);

//void EntityList(MM::Scene& ecs, const std::vector<component_type>& comps);

//void EntityListByComponents(MM::Scene& ecs, std::map<component_type, std::pair<std::string, bool>>& named_comps);

}
