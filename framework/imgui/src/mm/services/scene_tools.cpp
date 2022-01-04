#include "./scene_tools.hpp"
#include "mm/components/velocity2d_rotation.hpp"

#include <mm/engine.hpp>

#include <mm/services/imgui_menu_bar.hpp>

#include <imgui/imgui.h>

#include <mm/components/time_delta.hpp>

#include <mm/imgui/widgets/entity.hpp>

#include <mm/imgui/widgets/components/name.hpp>
#include <mm/imgui/widgets/components/position2d.hpp>
#include <mm/imgui/widgets/components/position2d_zoffset.hpp>
#include <mm/imgui/widgets/components/position3d.hpp>
#include <mm/imgui/widgets/components/rotation2d.hpp>
#include <mm/imgui/widgets/components/scale2d.hpp>
#include <mm/imgui/widgets/components/velocity2d_position.hpp>
#include <mm/imgui/widgets/components/velocity2d_rotation.hpp>
#include <mm/imgui/widgets/components/view_dir2d.hpp>
#include <mm/imgui/widgets/components/view_dir3d.hpp>
#include <mm/imgui/widgets/components/color.hpp>
#include <mm/imgui/widgets/components/texture.hpp>

#include <mm/imgui/widgets/camera.hpp>

#include <mm/logger.hpp>
#define LOGIGS(x) LOG("ImGuiSceneToolsService", x)

namespace MM::Services {

	bool ImGuiSceneToolsService::enable(Engine& engine, std::vector<UpdateStrategies::TaskInfo>& task_array) {
		if (!engine.tryService<MM::Services::SceneServiceInterface>()) {
			LOGIGS("error: no SceneServiceInterface");
			return false;
		}

		// setup entity editor defaults
		{
			_entity_editor.show_window = false;

			_entity_editor.registerComponent<MM::Components::Name>("Name");
			_entity_editor.registerComponent<MM::Components::Position2D>("Position2D");
			_entity_editor.registerComponent<MM::Components::Position2D_ZOffset>("Position2D_ZOffset");
			_entity_editor.registerComponent<MM::Components::Position3D>("Position3D");
			_entity_editor.registerComponent<MM::Components::Rotation2D>("Rotation2D");
			_entity_editor.registerComponent<MM::Components::Scale2D>("Scale2D");
			_entity_editor.registerComponent<MM::Components::Velocity2DPosition>("Velocity2DPosition");
			_entity_editor.registerComponent<MM::Components::Velocity2DRotation>("Velocity2DRotation");
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

		menu_bar.menu_tree["Scene"]["TimeCtx"] = [this](Engine& e) {
			MM::Components::TimeDelta* td_ptr = nullptr;

			if (auto* ssi_ptr = e.tryService<MM::Services::SceneServiceInterface>()) {
				auto& scene = ssi_ptr->getScene();
				td_ptr = scene.try_ctx<MM::Components::TimeDelta>();
			}

			ImGui::MenuItem("TimeDelta Context", NULL, &_show_time_delta_ctx, td_ptr);
		};

		// add task
		task_array.push_back(
			UpdateStrategies::TaskInfo{"ImGuiSceneToolsService::render"}
			.fn([this](Engine& e){ renderImGui(e); })
			.phase(UpdateStrategies::update_phase_t::PRE)
			.succeed("ImGuiMenuBar::render")
		);

		return true;
	}

	void ImGuiSceneToolsService::disable(Engine& engine) {
		auto& menu_bar = engine.getService<MM::Services::ImGuiMenuBar>();

		menu_bar.menu_tree["Scene"].erase("Metrics");
		menu_bar.menu_tree["Scene"].erase("EntityEditor");
		menu_bar.menu_tree["Scene"].erase("CameraTool");
		menu_bar.menu_tree["Scene"].erase("TimeCtx");
	}

	void ImGuiSceneToolsService::renderImGui(Engine& engine) {
		auto& scene = engine.tryService<MM::Services::SceneServiceInterface>()->getScene();

		if (_show_scene_metrics) {
			if (ImGui::Begin("Scene Metrics##ImGuiSceneToolsService", &_show_scene_metrics)) {
				ImGui::Text("capacity: %lu", scene.capacity());
				ImGui::Text("size: %lu", scene.size());
				ImGui::Text("alive: %lu", scene.alive());
				size_t orphans = 0;
				scene.each([&orphans, &scene](auto entity) {
					if (scene.orphan(entity)) {
						orphans++;
					}
				});
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

		if (_show_time_delta_ctx) {
			if (ImGui::Begin("Scene TimeDelta Context", &_show_time_delta_ctx)) {
				auto* td_ptr = scene.try_ctx<MM::Components::TimeDelta>();
				ImGui::Value("tickDelta", td_ptr->tickDelta);
				ImGui::SliderFloat("deltaFactor", &td_ptr->deltaFactor, 0.f, 10.f, "%.5f", ImGuiSliderFlags_Logarithmic);

			}
			ImGui::End();
		}
	}

	EntityEditor<Entity>& ImGuiSceneToolsService::getEntityEditor(void) {
		return _entity_editor;
	}

} // namespace MM::Services

