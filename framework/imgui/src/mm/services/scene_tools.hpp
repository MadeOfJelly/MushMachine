#pragma once

#define MM_IEEE_ENTITY_WIDGET ::MM::ImGuiWidgets::Entity // evil
#define MM_IEEE_ASSERT(x)
#include <mm/imgui/widgets/entity.hpp>
#include <mm/imgui/imgui_entt_entity_editor.hpp>

//#include <mm/imgui/file_text_editor.hpp>

namespace MM::Services {

	class ImGuiSceneToolsService : public Service {
		public:
			bool enable(Engine& engine) override;
			void disable(Engine& engine) override;

			const char* name(void) override { return "ImGuiSceneToolsService"; }

			std::vector<UpdateStrategies::UpdateCreationInfo> registerUpdates(void) override;

		private:
			bool _show_scene_metrics = false;
			bool _show_entity_editor = false;
			bool _show_entity_list = true;
			bool _show_camera_tool = false;
			bool _show_time_delta_ctx = false;

			EntityEditor<Entity> _entity_editor;
			// for list
			std::set<EntityEditor<Entity>::ComponentTypeID> _entity_comp_list;

			//std::vector<MM::FileTextEditor> _text_editor_list;

		private:
			void renderImGui(Engine& engine);

		public:
			EntityEditor<Entity>& getEntityEditor(void);
			Entity _e = entt::null; // entity currently in editor
	};

} // namespace MM::Services

