#include "./engine.hpp"

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

Engine::FunctionDataHandle Engine::addUpdate(std::function<void(Engine&)> function) {
	if (!function) {
		LOG_ERROR("could not add Update, empty function!");
		return {};
	}

	FunctionDataHandle r = _update_functions.emplace_back(std::make_shared<FunctionDataType>(function));
	_update_functions_modified = true;

	return r;
}

Engine::FunctionDataHandle Engine::addFixedUpdate(std::function<void(Engine&)> function) {
	if (!function) {
		LOG_ERROR("could not add fixedUpdate, empty function!");
		return {};
	}

	FunctionDataHandle r = _fixed_update_functions.emplace_back(std::make_shared<FunctionDataType>(function));
	_fixed_update_functions_modified = true;

	return r;
}

void Engine::removeUpdate(FunctionDataHandle handle) {
	if (handle.expired()) {
		LOG_ERROR("could not remove Update, invalid handle!");
		return;
	}

	auto lock = handle.lock();
	auto it = std::find(_update_functions.begin(), _update_functions.end(), lock);
	if (it != _update_functions.end()) {
		_update_functions.erase(it);
	} else {
		LOG_ERROR("could not remove Update, unknown handle!");
	}
}

void Engine::removeFixedUpdate(FunctionDataHandle handle) {
	if (handle.expired()) {
		LOG_ERROR("could not remove fixedUpdate, invalid handle!");
		return;
	}

	auto lock = handle.lock();
	auto it = std::find(_fixed_update_functions.begin(), _fixed_update_functions.end(), lock);
	if (it != _fixed_update_functions.end()) {
		_fixed_update_functions.erase(it);
	} else {
		LOG_ERROR("could not remove fixedUpdate, unknown handle!");
	}
}

void Engine::addFixedDefer(std::function<void(Engine&)> function) {
	_fixed_defered.emplace_back(function);
}

void Engine::traverseUpdateFunctions(std::vector<std::shared_ptr<FunctionDataType>>& list) {
	for (auto& entry : list) {
		entry->f(*this);
	}
}

Engine::Engine(float f_delta_time) : _fixed_delta_time(f_delta_time) {
	if (!MM::Logger::initialized) {
		MM::Logger::init();
	}

	MM::Logger::initSectionLogger("Engine");
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

	_update_functions.clear();
	_fixed_update_functions.clear();

	spdlog::get("Engine")->flush();
}

void Engine::update(void) {
	FrameMarkStart("update")
	if (_update_functions_modified) {
		ZoneScopedN("MM::Engine::update::sort_update_functions")
		std::sort(_update_functions.begin(), _update_functions.end(), [](const auto& a, const auto& b) { return a->priority > b->priority; });
		_update_functions_modified = false;
	}

	{
		ZoneScopedN("MM::Engine::update::traverseUpdateFunctions")
		traverseUpdateFunctions(_update_functions);
	}

	FrameMarkEnd("update")
}

void Engine::fixedUpdate(void) {
	FrameMarkStart("fixedUpdate")
	if (_fixed_update_functions_modified) {
		ZoneScopedN("MM::Engine::fixedUpdate::sort_update_functions")

		std::sort(_fixed_update_functions.begin(), _fixed_update_functions.end(), [](const auto& a, const auto& b) { return a->priority > b->priority; });
		_fixed_update_functions_modified = false;
	}

	{
		ZoneScopedN("MM::Engine::fixedUpdate::traverseUpdateFunctions")
		traverseUpdateFunctions(_fixed_update_functions);
	}

	if (!_fixed_defered.empty()) {
		ZoneScopedN("MM::Engine::fixedUpdate::defered")
		for (auto& fn : _fixed_defered) {
			fn(*this);
		}

		_fixed_defered.clear();
	}

	FrameMarkEnd("fixedUpdate")
}

#ifdef __EMSCRIPTEN__
#include <emscripten.h>

static void emscripten_update(void* arg) {
	using clock = std::chrono::high_resolution_clock;
	static long long int accumulator = 0;
	static auto now = clock::now();

	auto* e = (MM::Engine*)arg;

	auto newNow = clock::now();
	auto deltaTime = std::chrono::duration_cast<std::chrono::nanoseconds>(newNow - now);
	now = newNow;
	accumulator += deltaTime.count();
	auto dt = e->getFixedDeltaTime() * 1'000'000'000.0f;
	while (accumulator >= dt) {
		accumulator -= dt;
		e->fixedUpdate();
	}

	e->update();
}
#endif

void Engine::run(void) {
#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop_arg(emscripten_update, this, 0, 1);
#else
	using clock = std::chrono::high_resolution_clock;

	_is_running = true;
	long long int accumulator = 0;
	auto now = clock::now();
	while (_is_running) {
		auto newNow = clock::now();
		auto deltaTime = std::chrono::duration_cast<std::chrono::nanoseconds>(newNow - now);
		now = newNow;
		accumulator += deltaTime.count();
		auto dt = _fixed_delta_time * 1'000'000'000.0f;


		size_t continuous_counter = 0;
		while (accumulator >= dt) {
			continuous_counter++;
			accumulator -= static_cast<long long int>(dt);
			fixedUpdate();
		}

		if (continuous_counter > 2) {
			LOG_WARN("had {} contiguous fixedUpdates!", std::to_string(continuous_counter));
		}

		update();
	}
#endif
}

void Engine::stop(void) {
	_is_running = false;
}

bool Engine::enableService(service_family::family_type s_t) {
	if (_services.count(s_t)) {
		auto* ss_entry = _services[s_t].get();
		if (ss_entry->first) {
			return false; // already enabled
		}

		_service_enable_order.emplace_back(s_t); // TODO: make sure
		return ss_entry->first = ss_entry->second.get()->enable(*this);
	}

	// not found
	assert(false && "first add Service");
	return false;
}

void Engine::disableService(service_family::family_type s_t) {
	if (_services.count(s_t)) {
		auto* s_entry = _services[s_t].get();
		if (s_entry->first) {
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

bool Engine::provide(service_family::family_type I, service_family::family_type T) {
	if (!_services.count(T)) {
		// TODO: log error
		return false;
	}

	_services[I] = _services[T];

	return true;
}


} // MM

