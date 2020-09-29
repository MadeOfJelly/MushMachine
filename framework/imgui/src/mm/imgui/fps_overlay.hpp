#pragma once

namespace MM {
	class ImGuiSimpleFPSOverlay {
		private:
			bool _show_plot = false;

		public:
			void renderImGui(void);
	};
} // namespace MM

