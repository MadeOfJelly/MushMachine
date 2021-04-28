#include "./imgui_menu_bar.hpp"
#include "mm/services/service.hpp"

#include <mm/engine.hpp>
#include <entt/core/hashed_string.hpp>

#include <imgui/imgui.h>

#include <unordered_set>

#include <mm/logger.hpp>
#define LOG_CRIT(...)		__LOG_CRIT(	"ImGuiMenuBar", __VA_ARGS__)
#define LOG_ERROR(...)		__LOG_ERROR("ImGuiMenuBar", __VA_ARGS__)
#define LOG_WARN(...)		__LOG_WARN(	"ImGuiMenuBar", __VA_ARGS__)
#define LOG_INFO(...)		__LOG_INFO(	"ImGuiMenuBar", __VA_ARGS__)
#define LOG_DEBUG(...)		__LOG_DEBUG("ImGuiMenuBar", __VA_ARGS__)
#define LOG_TRACE(...)		__LOG_TRACE("ImGuiMenuBar", __VA_ARGS__)

namespace MM::Services {

	bool ImGuiMenuBar::enable(Engine& engine, std::vector<UpdateStrategies::TaskInfo>& task_array) {
		MM::Logger::initSectionLogger("ImGuiMenuBar");

		auto* sdl_ss = engine.tryService<MM::Services::SDLService>();
		if (sdl_ss) {
			_event_handle = sdl_ss->addEventHandler([this](const SDL_Event& e) -> bool {
				if (e.type == SDL_KEYDOWN && !e.key.repeat && e.key.keysym.sym == toggle_key) {
					show_menu_bar = !show_menu_bar;
				}
				return false; // invis
			});
		} else {
			LOG_WARN("no SDLService, skipping toggle hotkey");
		}

		// add task
		task_array.push_back(
			UpdateStrategies::TaskInfo{"ImGuiMenuBar::render"}
			.fn([this](Engine& e){ renderImGui(e); })
		);

		return true;
	}

	void ImGuiMenuBar::disable(Engine& engine) {
		if (_event_handle) {
			auto* sdl_ss = engine.tryService<MM::Services::SDLService>();
			sdl_ss->removeEventHandler(_event_handle);
			_event_handle = nullptr;
		}
	}

	void ImGuiMenuBar::renderImGui(Engine& engine) {
		if (show_menu_bar && ImGui::BeginMainMenuBar()) {

			// TODO: better solution?
			std::unordered_set<std::string> done;

			// we dont check for errors in section_order
			for (const auto& section_key : section_order) {
				if (menu_tree.count(section_key)) {
					renderSection(engine, section_key, menu_tree[section_key]);

					done.emplace(section_key);
				}
			}

			for (auto& [section_key, section_data] : menu_tree) {
				if (done.count(section_key)) {
					continue;
				}

				renderSection(engine, section_key, section_data);

				done.emplace(section_key); // we dont need this
			}


			ImGui::Separator();
			ImGui::Text("%.1fFPS", ImGui::GetIO().Framerate);
			ImGui::EndMainMenuBar();
		}
	}

	void ImGuiMenuBar::renderSection(Engine& engine, const std::string& section_key, section_t& section) {
		if (ImGui::BeginMenu(section_key.c_str())) {
			for (auto& item : section) {
				item.second(engine);
			}
			ImGui::EndMenu();
		}
	}

} // namespace MM::Services

