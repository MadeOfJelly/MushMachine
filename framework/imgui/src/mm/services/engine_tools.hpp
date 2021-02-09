#pragma once

#include "./imgui_menu_bar.hpp"

namespace MM::Services {

	class ImGuiEngineTools : public Service {
		public:
			const char* name(void) override { return "ImGuiEngineTools"; }

			bool enable(Engine& engine) override;
			void disable(Engine& engine) override;

			std::vector<UpdateStrategies::UpdateCreationInfo> registerUpdates(void) override;

		private:
			bool _show_about = false;
			bool _show_services = false;

			bool _services_edit_mode = false;

		private:
			void renderImGui(Engine& engine);

			void renderAbout(void);
			void renderServices(Engine& engine);
	};

} // namespace MM::Services

