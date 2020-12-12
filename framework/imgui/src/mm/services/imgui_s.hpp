#pragma once

#include <mm/engine.hpp>
#include <mm/services/sdl_service.hpp>

namespace MM::Services {

	// responsable for event handling and setup
	// enable requires an open sdl window (and context)
	class ImGuiService : public Service {
		private:
			MM::Services::SDLService::EventHandlerHandle _event_handle = nullptr;

			// new frame needs to start AFTER the events have been processed (and obv bf rendt)

		public:
			bool enable(Engine& engine) override;
			void disable(Engine& engine) override;

			const char* name(void) override { return "ImGuiService"; }

			std::vector<UpdateStrategies::UpdateCreationInfo> registerUpdates(void) override;

		private:
			void imgui_new_frame(Engine& engine);

			bool handle_sdl_event(const SDL_Event& e);
	};

} // namespace MM::Services

