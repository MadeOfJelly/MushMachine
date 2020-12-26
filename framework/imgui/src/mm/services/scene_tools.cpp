#include "./scene_tools.hpp"

#include <entt/config/version.h>

#include <mm/services/opengl_renderer.hpp>

#include <mm/resource_manager.hpp>

#include <mm/engine.hpp>

#include <imgui/imgui.h>

#include <mm/components/name.hpp>
#include <mm/components/transform2d.hpp>
#include <mm/components/velocity2d.hpp>

#include <mm/imgui/widgets/texture_resource_manager.hpp>
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

#include <mm/imgui/widgets/filesystem.hpp>
#include <mm/services/filesystem.hpp>

#include <mm/logger.hpp>
#define LOGIGS(x) LOG("ImGuiSceneToolsService", x)

namespace MM::Services {

	void ImGuiSceneToolsService::renderImGui(Engine& engine) {
		if (show_menu && ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("Engine")) {
				if (ImGui::MenuItem("Stop Engine")) {
					engine.stop();
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("TextEditor")) {
				if (ImGui::MenuItem("New Editor")) {
					_text_editor_list.emplace_back(engine);
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Windows")) {
				ImGui::MenuItem("MM Metrics", NULL, &_show_mm_metrics);
				ImGui::MenuItem("ImGui Metrics", NULL, &_show_imgui_metrics);
				ImGui::MenuItem("Texture Loader", NULL, &_show_texture_loader);
				ImGui::MenuItem("Texture List", NULL, &_show_texture_list);
				ImGui::MenuItem("Entity Editor", NULL, &_show_entity_editor);
				ImGui::MenuItem("Camera Tool", NULL, &_show_camera_tool);
				ImGui::EndMenu();
			}

			ImGui::Text("%.1fFPS", ImGui::GetIO().Framerate);
			ImGui::EndMainMenuBar();
		}

		auto& scene = engine.tryService<MM::Services::SceneServiceInterface>()->getScene();

		if (_show_mm_metrics) {
			if (ImGui::Begin("Metrics##MM", &_show_mm_metrics)) {
				if (ImGui::CollapsingHeader("ECS")) {
					ImGui::Text("EnTT v%d.%d.%d", ENTT_VERSION_MAJOR, ENTT_VERSION_MINOR, ENTT_VERSION_PATCH);
					ImGui::Text("capacity: %lu", scene.capacity());
					ImGui::Text("size: %lu", scene.size());
					ImGui::Text("alive: %lu", scene.alive());
					size_t orphans = 0;
					scene.orphans([&](auto) { orphans++; });
					ImGui::Text("orphans: %lu", orphans);
				}
			}
			ImGui::End();
		}

		if (_show_imgui_metrics) {
			ImGui::ShowMetricsWindow(&_show_imgui_metrics);
		}

		if (_show_texture_loader) {
			if (ImGui::Begin("Texture Loader", &_show_texture_loader)) {
				ImGuiWidgets::TextureResourceManagerLoader(engine);
			}
			ImGui::End();
		}

		if (_show_texture_list) {
			if (ImGui::Begin("Texture List", &_show_texture_list)) {
				ImGuiWidgets::TextureResourceManagerList();
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

		for (auto it = _text_editor_list.begin(); it != _text_editor_list.end();) {
			bool keep_open = true;
			it->renderImGui(&keep_open);

			if (!keep_open) {
				it = _text_editor_list.erase(it);
				continue;
			}
			it++;
		}
	}

	EntityEditor<Entity>& ImGuiSceneToolsService::getEntityEditor(void) {
		return _entity_editor;
	}

	bool ImGuiSceneToolsService::enable(Engine& engine) {
		if (!engine.tryService<MM::Services::OpenGLRenderer>()) {
			LOGIGS("error: rendering is not in engine");
			return false;
		}

		if (!engine.tryService<MM::Services::FilesystemService>()) {
			LOGIGS("error: filesystem not initialized");
			return false;
		}

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

		auto* sdl_ss = engine.tryService<MM::Services::SDLService>();
		if (sdl_ss) {
			_event_handle = sdl_ss->addEventHandler([this](const SDL_Event& e) -> bool {
				if (e.type == SDL_KEYDOWN && !e.key.repeat && e.key.keysym.sym == this->toggle_key) {
					this->show_menu = !this->show_menu;
				}
				return false; // invis
			});
		} else {
			LOGIGS("Warn: no SDLService, skipping toggle hotkey");
		}

		return true;
	}

	void ImGuiSceneToolsService::disable(Engine& engine) {
		if (_event_handle) {
			auto* sdl_ss = engine.tryService<MM::Services::SDLService>();
			sdl_ss->removeEventHandler(_event_handle);
			_event_handle = nullptr;
		}
	}

	std::vector<UpdateStrategies::UpdateCreationInfo> ImGuiSceneToolsService::registerUpdates(void) {
		return {
			{
				"ImGuiSceneToolsService::render"_hs,
				"ImGuiSceneToolsService::render",
				[this](Engine& e){ renderImGui(e); }
			}
		};
	}
} // namespace MM::Services

