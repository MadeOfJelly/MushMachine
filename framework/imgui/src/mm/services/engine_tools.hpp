#pragma once

#include "./imgui_menu_bar.hpp"

namespace MM::Services {

	class ImGuiEngineTools : public Service {
		public:
			const char* name(void) override { return "ImGuiEngineTools"; }

			bool enable(Engine& engine, std::vector<UpdateStrategies::TaskInfo>& task_array) override;
			void disable(Engine& engine) override;

		private:
			bool _show_about = false;
			bool _show_services = false;
			bool _show_update_stategy = false;

			bool _services_edit_mode = false;

		private:
			void renderImGui(Engine& engine);

			void renderAbout(Engine& engine);
			void renderServices(Engine& engine);
			void renderUpdateStrategy(Engine& engine);
	};

} // namespace MM::Services

