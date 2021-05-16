#pragma once

#include "./imgui_menu_bar.hpp"

namespace MM::Services {

	class ImGuiSoundTools : public Service {
		public:
			const char* name(void) override { return "ImGuiSoundTools"; }

			bool enable(Engine& engine, std::vector<UpdateStrategies::TaskInfo>& task_array) override;
			void disable(Engine& engine) override;

		private:
			bool _show_info = false;

		private:
			void renderImGui(Engine& engine);
	};

} // namespace MM::Services

