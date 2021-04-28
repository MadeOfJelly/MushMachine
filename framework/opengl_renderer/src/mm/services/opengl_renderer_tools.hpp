#pragma once

#include <mm/services/imgui_menu_bar.hpp>

namespace MM::Services {

	class ImGuiOpenGLRendererTools : public Service {
		public:
			const char* name(void) override { return "ImGuiOpenGLRendererTools"; }

			bool enable(Engine& engine, std::vector<UpdateStrategies::TaskInfo>& task_array) override;
			void disable(Engine& engine) override;

		private:
			bool _show_render_tasks = false;
			bool _show_texture_cache_legacy = false;

		private:
			void renderImGui(Engine& engine);

			void renderRenderTasks(Engine& engine);
			void renderTextureCacheLegacy(Engine& engine);
	};

} // namespace MM::Services

