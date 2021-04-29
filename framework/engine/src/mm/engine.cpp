#include "./engine.hpp"

#include <mm/update_strategies/default_strategy.hpp>

#include <chrono>
#include <algorithm>

#include <memory>
#include <tracy/Tracy.hpp>

#include <mm/logger.hpp>
#define LOG_CRIT(...)		__LOG_CRIT(	"Engine", __VA_ARGS__)
#define LOG_ERROR(...)		__LOG_ERROR("Engine", __VA_ARGS__)
#define LOG_WARN(...)		__LOG_WARN(	"Engine", __VA_ARGS__)
#define LOG_INFO(...)		__LOG_INFO(	"Engine", __VA_ARGS__)
#define LOG_DEBUG(...)		__LOG_DEBUG("Engine", __VA_ARGS__)
#define LOG_TRACE(...)		__LOG_TRACE("Engine", __VA_ARGS__)

namespace MM {

void Engine::setup(void) {
	if (!MM::Logger::initialized) {
		MM::Logger::init();
	}

	MM::Logger::initSectionLogger("Engine");
}

Engine::Engine(void) {
	setup();

	_update_strategy = std::make_unique<MM::UpdateStrategies::SingleThreadedDefault>();
	LOG_INFO("defaulting to SingleThreadedDefault UpdateStrategy");
}

Engine::~Engine(void) {
	cleanup();
}

void Engine::cleanup(void) {
	// disable all enabled services
	// in reverse order
	for (auto s_it = _service_enable_order.rbegin(); s_it != _service_enable_order.rend(); s_it++) {
		auto& s_e = _services[*s_it];
		if (s_e.get()->first) {
			if (auto* ss = s_e.get()->second.get(); ss != nullptr) {
				ss->disable(*this);
				LOG_INFO("- disabled Service '{}'", ss->name());
			}
		}
	}

	_service_enable_order.clear();

	_update_strategy.reset();

	spdlog::get("Engine")->flush();
}

void Engine::update(void) {
	FrameMarkStart("update")
	//if (_update_functions_modified) {
		//ZoneScopedN("MM::Engine::update::sort_update_functions")
		//std::sort(_update_functions.begin(), _update_functions.end(), [](const auto& a, const auto& b) { return a->priority > b->priority; });
		//_update_functions_modified = false;
	//}

	//{
		//ZoneScopedN("MM::Engine::update::traverseUpdateFunctions")
		//traverseUpdateFunctions(_update_functions);
	//}

	_update_strategy->doUpdate(*this);

	FrameMarkEnd("update")
}

//void Engine::fixedUpdate(void) {
	//FrameMarkStart("fixedUpdate")
	////if (_fixed_update_functions_modified) {
		////ZoneScopedN("MM::Engine::fixedUpdate::sort_update_functions")

		////std::sort(_fixed_update_functions.begin(), _fixed_update_functions.end(), [](const auto& a, const auto& b) { return a->priority > b->priority; });
		////_fixed_update_functions_modified = false;
	////}

	////{
		////ZoneScopedN("MM::Engine::fixedUpdate::traverseUpdateFunctions")
		////traverseUpdateFunctions(_fixed_update_functions);
	////}

	////if (!_fixed_defered.empty()) {
		////ZoneScopedN("MM::Engine::fixedUpdate::defered")
		////for (auto& fn : _fixed_defered) {
			////fn(*this);
		////}

		////_fixed_defered.clear();
	////}

	//FrameMarkEnd("fixedUpdate")
//}

#ifdef __EMSCRIPTEN__
#include <emscripten.h>

static void emscripten_update(void* arg) {
	auto* e = (MM::Engine*)arg;

	e->update();
}
#endif

void Engine::run(void) {
#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop_arg(emscripten_update, this, 0, 1);
#else

	_is_running = true;
	while (_is_running) {
		update();
	}
#endif
}

void Engine::stop(void) {
	_is_running = false;
}

bool Engine::enableService(entt::id_type s_t) {
	if (_services.count(s_t)) {
		auto* ss_entry = _services[s_t].get();
		if (ss_entry->first) {
			return false; // already enabled
		}

		_service_enable_order.emplace_back(s_t); // TODO: make sure

		ss_entry->first = ss_entry->second.get()->enable(*this);

		_update_strategy->enableService(s_t); // after service::enable()

		return ss_entry->first;
	}

	// not found
	assert(false && "first add Service");
	return false;
}

void Engine::disableService(entt::id_type s_t) {
	if (_services.count(s_t)) {
		auto* s_entry = _services[s_t].get();
		if (s_entry->first) {

			_update_strategy->disableService(s_t);

			s_entry->first = false;

			s_entry->second.get()->disable(*this);
			//_service_enable_order.emplace_back(service_family::type<T>);
			auto it = std::find(_service_enable_order.begin(), _service_enable_order.end(), s_t);
			if (it != _service_enable_order.end()) {
				_service_enable_order.erase(it);
			}
		}
	}
}

bool Engine::provide(entt::id_type I, entt::id_type T) {
	if (!_services.count(T)) {
		// TODO: log error
		return false;
	}

	_services[I] = _services[T];

	return true;
}

} // MM

