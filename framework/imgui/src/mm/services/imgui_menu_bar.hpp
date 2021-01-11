#pragma once

#include <mm/services/sdl_service.hpp>

#include <map>
#include <variant>
#include <functional>

namespace MM::Services {

	class ImGuiMenuBar : public Service {
		public:
			const char* name(void) override { return "ImGuiMenuBar"; }

			bool enable(Engine& engine) override;
			void disable(Engine& engine) override;

			std::vector<UpdateStrategies::UpdateCreationInfo> registerUpdates(void) override;

		public:
			bool show_menu_bar = true;

			SDL_Keycode toggle_key = SDLK_F2;

			using menu_item_callback_t = std::function<void(MM::Engine&)>;

			using section_t = std::map<std::string, menu_item_callback_t>;

			// menu tree
			// section -> section_t -> menu_item_t
			// section is stable id key
			// menus contain stable id keys
			std::map<std::string, section_t> menu_tree;

			std::vector<std::string> section_order {
				"Engine",
			};

		private:
			MM::Services::SDLService::EventHandlerHandle _event_handle = nullptr;

		private:
			void renderImGui(Engine& engine);

			void renderSection(Engine& engine, const std::string& section_key, section_t& section);
	};

} // namespace MM::Services

