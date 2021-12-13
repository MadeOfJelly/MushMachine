#include "./entity.hpp"

#include <mm/imgui/imgui_entt_entity_editor.hpp>

#include <mm/components/name.hpp>

#include <IconsIonicons.h>

#include <entt/entity/registry.hpp>

namespace MM::ImGuiWidgets {

void Entity(MM::Entity& e, MM::Scene& ecs, bool dropTarget) {
	ImGui::PushID(static_cast<int>(entt::to_integral(e)));

	if (ecs.valid(e)) {
		if (ecs.all_of<MM::Components::Name>(e)) {
			ImGui::Text(ICON_II_CUBE "E: %d v%d (%s)", entt::to_entity(e), entt::to_version(e), ecs.get<MM::Components::Name>(e).str.c_str());
		} else {
			ImGui::Text(ICON_II_CUBE "E: %d v%d", entt::to_entity(e), entt::to_version(e));
		}
	} else {
		ImGui::Text(ICON_II_CUBE "E: invalid");
	}

	if (ecs.valid(e)) {
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
			ImGui::SetDragDropPayload(MM_IEEE_IMGUI_PAYLOAD_TYPE_ENTITY, &e, sizeof(e));
			ImGui::Text(ICON_II_CUBE "E: %d v%d", entt::to_entity(e), entt::to_version(e));
			ImGui::EndDragDropSource();
		}
	}

	if (dropTarget && ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(MM_IEEE_IMGUI_PAYLOAD_TYPE_ENTITY)) {
			e = *(MM::Entity*)payload->Data;
		}

		ImGui::EndDragDropTarget();
	}

	ImGui::PopID();
}

void EntityTrashCan(MM::Scene& ecs) {
	ImGui::TextColored({1.f, 0.2f, 0.2f, 1.f}, ICON_II_TRASH_A " Entity TrashCan");

	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(MM_IEEE_IMGUI_PAYLOAD_TYPE_ENTITY)) {
			auto e = *(MM::Entity*)payload->Data;
			ecs.destroy(e);
		}

		ImGui::EndDragDropTarget();
	}
}

//void EntityList(MM::Scene& ecs, const std::vector<component_type>& comps) {
	//auto view = ecs.runtime_view(comps.begin(), comps.end());

	//ImGui::Text("%lu Entities matching components:", view.size());

	//if (ImGui::BeginChild("entity list")) {
		//ImGui::Indent();
		//for (auto e : view) {
			//MM::ImGuiWidgets::Entity(e, ecs, false);
		//}
		//ImGui::Unindent();
	//}
	//ImGui::EndChild();
//}

//void EntityListByComponents(MM::Scene& ecs, std::map<component_type, std::pair<std::string, bool>>& named_comps) {
	//ImGui::Text("Components:");
	//ImGui::Indent();
	//std::vector<component_type> comps;

	//for (auto& it : named_comps) {
		//ImGui::Checkbox(it.second.first.c_str(), &it.second.second);

		//if (it.second.second) {
			//comps.push_back(it.first);
		//}

	//}

	//ImGui::Unindent();

	//ImGui::Separator();
	//EntityList(ecs, comps);
//}

}

