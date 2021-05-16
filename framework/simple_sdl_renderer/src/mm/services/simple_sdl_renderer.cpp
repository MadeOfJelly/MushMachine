#include "simple_sdl_renderer.hpp"

#include <mm/logger.hpp>
#define LOG_CRIT(...)		__LOG_CRIT(	"SimpleSDLRenderer", __VA_ARGS__)
#define LOG_ERROR(...)		__LOG_ERROR("SimpleSDLRenderer", __VA_ARGS__)
#define LOG_WARN(...)		__LOG_WARN(	"SimpleSDLRenderer", __VA_ARGS__)
#define LOG_INFO(...)		__LOG_INFO(	"SimpleSDLRenderer", __VA_ARGS__)
#define LOG_DEBUG(...)		__LOG_DEBUG("SimpleSDLRenderer", __VA_ARGS__)
#define LOG_TRACE(...)		__LOG_TRACE("SimpleSDLRenderer", __VA_ARGS__)

namespace MM::Services {

SimpleSDLRendererService::SimpleSDLRendererService(void) {
	MM::Logger::initSectionLogger("SimpleSDLRenderer");
}

SimpleSDLRendererService::~SimpleSDLRendererService(void) {
}

bool SimpleSDLRendererService::enable(Engine& engine, std::vector<UpdateStrategies::TaskInfo>& task_array) {
	auto* sdl_ss = engine.tryService<SDLService>();
	if (!sdl_ss) {
		LOG_ERROR("SimpleSDLRendererService requires SDLService in engine!");
		return false;
	}

	if (!sdl_ss->win) {
		sdl_ss->createWindow("SimpleSDLRendererService Window", 800, 600);
	}

	renderer = SDL_CreateRenderer(
			sdl_ss->win,
			-1,
			SDL_RENDERER_ACCELERATED |
			SDL_RENDERER_PRESENTVSYNC |
			SDL_RENDERER_TARGETTEXTURE
		);

	if (!renderer) {
		LOG_ERROR("could not create sdl_renderer");
		return false;
	}

	targets["display"].reset(renderer, 800, 600);

	// add tasks
	task_array.push_back(
		UpdateStrategies::TaskInfo{"SimpleSDLRendererService::render"}
		.phase(UpdateStrategies::update_phase_t::POST)
		.fn([this](Engine& e){ this->render(e); })
	);

	return true;
}

void SimpleSDLRendererService::disable(Engine&) {
	processors.clear();

	targets.clear();

	SDL_DestroyRenderer(renderer);
}

void SimpleSDLRendererService::render(Engine& engine) {
	targets["display"].clear(renderer, {40, 40, 40, 255});

	for (auto& p : processors) {
		p(*this, engine);
	}

	SDL_SetRenderTarget(renderer, NULL);
	SDL_RenderCopy(renderer, targets["display"].texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}

} // namespace MM::Services

