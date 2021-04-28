#pragma once

#include "mm/services/service.hpp"
#include <SDL.h>

#include <mm/engine.hpp>

#include <mm/services/sdl_service.hpp>

#include <mm/simple_sdl_renderer/target.hpp>

namespace MM::Services {

	class SimpleSDLRendererService : public Service {
		public:
			SimpleSDLRendererService(void);
			~SimpleSDLRendererService(void);

			SDL_Renderer* renderer = nullptr;

			std::unordered_map<std::string, SimpleSDLRenderer::Target> targets;

			std::vector<std::function<void(SimpleSDLRendererService&, Engine&)>> processors;


		public:
			bool enable(Engine& engine, std::vector<UpdateStrategies::TaskInfo>& task_array) override;
			void disable(Engine& engine) override;

			const char* name(void) override { return "SimpleSDLServiceSystem"; };

		private:
			void render(Engine& engine);
	};

} // namespace MM::Services

