#include "./scene_tools.hpp"

//#include <mm/services/opengl_renderer.hpp>

//#include <mm/resource_manager.hpp>

#include <mm/engine.hpp>
#include "./imgui_menu_bar.hpp"

#include <imgui/imgui.h>

#include <mm/components/name.hpp>
#include <mm/components/transform2d.hpp>
#include <mm/components/velocity2d.hpp>

//#include <mm/imgui/widgets/texture_resource_manager.hpp>
#include <mm/imgui/widgets/entity.hpp>

#include <mm/imgui/widgets/components/name.hpp>
#include <mm/imgui/widgets/components/transform2d.hpp>
#include <mm/imgui/widgets/components/transform3d.hpp>
#include <mm/imgui/widgets/components/velocity2d.hpp>
#include <mm/imgui/widgets/components/view_dir2d.hpp>
#include <mm/imgui/widgets/components/view_dir3d.hpp>
#include <mm/imgui/widgets/components/color.hpp>
#include <mm/imgui/widgets/components/texture.hpp>

#include <mm/imgui/widgets/camera.hpp>

//#include <mm/imgui/widgets/filesystem.hpp>
//#include <mm/services/filesystem.hpp>

#include <mm/logger.hpp>
#define LOGIGS(x) LOG("ImGuiSceneToolsService", x)

namespace MM::Services {

	bool ImGuiSceneToolsService::enable(Engine& engine) {
		if (!engine.tryService<MM::Services::SceneServiceInterface>()) {
			LOGIGS("error: no SceneServiceInterface");
			return false;
		}

		// setup entity editor defaults
		{
			_entity_editor.show_window = false;

			_entity_editor.registerComponent<MM::Components::Name>("Name");
			_entity_editor.registerComponent<MM::Components::Transform2D>("Transform2D");
			_entity_editor.registerComponent<MM::Components::Transform3D>("Transform3D");
			_entity_editor.registerComponent<MM::Components::Velocity2D>("Velocity2D");
			_entity_editor.registerComponent<MM::Components::ViewDir2D>("ViewDir2D");
			_entity_editor.registerComponent<MM::Components::ViewDir3D>("ViewDir3D");
			_entity_editor.registerComponent<MM::Components::Color>("Color");

			_entity_editor.registerComponent<MM::Components::OpenGL::Texture>("Texture");
		}

		auto& menu_bar = engine.getService<MM::Services::ImGuiMenuBar>();

		menu_bar.menu_tree["Scene"]["Metrics"] = [this](Engine&) {
			ImGui::MenuItem("Metrics", NULL, &_show_scene_metrics);
		};

		menu_bar.menu_tree["Scene"]["EntityEditor"] = [this](Engine&) {
			ImGui::MenuItem("EntityEditor", NULL, &_show_entity_editor);
		};

		menu_bar.menu_tree["Scene"]["CameraTool"] = [this](Engine&) {
			ImGui::MenuItem("Camera3D Tool", NULL, &_show_camera_tool);
		};

		return true;
	}

	void ImGuiSceneToolsService::disable(Engine& engine) {
		auto& menu_bar = engine.getService<MM::Services::ImGuiMenuBar>();

		menu_bar.menu_tree["Scene"].erase("Metrics");
		menu_bar.menu_tree["Scene"].erase("EntityEditor");
	}

	std::vector<UpdateStrategies::UpdateCreationInfo> ImGuiSceneToolsService::registerUpdates(void) {
		using namespace entt::literals;
		return {
			{
				"ImGuiSceneToolsService::render"_hs,
				"ImGuiSceneToolsService::render",
				[this](Engine& e){ renderImGui(e); },
				UpdateStrategies::update_phase_t::MAIN,
				true,
				{
					"ImGuiMenuBar::render"_hs
				}
			}
		};
	}

	void ImGuiSceneToolsService::renderImGui(Engine& engine) {
		//if (show_menu && ImGui::BeginMainMenuBar()) {

			//if (ImGui::BeginMenu("Windows")) {
				//ImGui::MenuItem("Camera Tool", NULL, &_show_camera_tool);
				//ImGui::EndMenu();
			//}

		//}

		auto& scene = engine.tryService<MM::Services::SceneServiceInterface>()->getScene();

		if (_show_scene_metrics) {
			if (ImGui::Begin("Scene Metrics##ImGuiSceneToolsService", &_show_scene_metrics)) {
				ImGui::Text("capacity: %lu", scene.capacity());
				ImGui::Text("size: %lu", scene.size());
				ImGui::Text("alive: %lu", scene.alive());
				size_t orphans = 0;
				scene.orphans([&](auto) { orphans++; });
				ImGui::Text("orphans: %lu", orphans);
			}
			ImGui::End();
		}

		if (_show_entity_editor) {
			ImGui::SetNextWindowSize(ImVec2(750, 500), ImGuiCond_FirstUseEver);
			if (ImGui::Begin("Entity Editor", &_show_entity_editor, ImGuiWindowFlags_MenuBar)) {
				ImGui::BeginMenuBar();
				ImGui::Checkbox("Show List", &_show_entity_list);
				ImGui::EndMenuBar();

				if (_show_entity_list) {
					if (ImGui::BeginChild("list", {300, 0}, true)) {
						ImGuiWidgets::EntityTrashCan(scene); // TODO: remove?
						_entity_editor.renderEntityList(scene, _entity_comp_list);
					}
					ImGui::EndChild();
					ImGui::SameLine();
				}

				if (ImGui::BeginChild("editor")) {
					_entity_editor.renderEditor(scene, _e);
				}
				ImGui::EndChild();
			}
			ImGui::End();
		}

		if (_show_camera_tool) {
			if (ImGui::Begin("Camera3D Tool", &_show_camera_tool)) {
				ImGuiWidgets::Camera3D(scene);
			}
			ImGui::End();
		}
	}

	EntityEditor<Entity>& ImGuiSceneToolsService::getEntityEditor(void) {
		return _entity_editor;
	}

} // namespace MM::Services

