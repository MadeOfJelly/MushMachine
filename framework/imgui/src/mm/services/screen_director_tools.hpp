#pragma once

#include "./imgui_menu_bar.hpp"

namespace MM::Services {

	class ImGuiScreenDirectorTools : public Service {
		public:
			const char* name(void) override { return "ImGuiScreenDirectorTools"; }

			bool enable(Engine& engine, std::vector<UpdateStrategies::TaskInfo>& task_array) override;
			void disable(Engine& engine) override;

		private:
			bool _show_screen_list = false;

			//bool _edit_mode = false;

		private:
			void renderImGui(Engine& engine);

			void renderScreenList(Engine& engine);
	};

} // namespace MM::Services

