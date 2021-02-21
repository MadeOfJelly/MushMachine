#include "./simple_scene.hpp"

#include <mm/components/time_delta.hpp>

#include <entt/entity/registry.hpp>


#include <tracy/Tracy.hpp>

#include <mm/logger.hpp>
#define LOG_SSS(x) LOG("SimpleSceneService", x)

namespace MM::Services {

bool SimpleSceneService::enable(Engine& engine) {
	// default scene
	if (!_scene) {
		_scene = std::make_unique<Scene>();
		_scene->set<MM::Engine*>(&engine);
	}

	resetTime();

	return true;
}

void SimpleSceneService::disable(Engine&) {
}

std::vector<UpdateStrategies::UpdateCreationInfo> SimpleSceneService::registerUpdates(void) {
	using namespace entt::literals;
	return {
		{
			"SimpleSceneService::scene_tick"_hs,
			"SimpleSceneService::scene_tick",
			[this](Engine& e) { this->sceneFixedUpdate(e); },
			UpdateStrategies::update_phase_t::MAIN,
			true,
			{} // no dependencies"
		},
		{
			"SimpleSceneService::scene_change"_hs,
			"SimpleSceneService::scene_change",
			[this](Engine& e) { this->changeSceneFixedUpdate(e); },
			UpdateStrategies::update_phase_t::MAIN,
			true,
			//{"SimpleSceneService::scene_update"_hs} // first update, than change????
			{"SimpleSceneService::scene_tick"_hs}
		}
	};
}

void SimpleSceneService::sceneFixedUpdate(Engine&) {
	ZoneScoped;

	auto newNow = clock::now();
	auto deltaTime = std::chrono::duration_cast<std::chrono::nanoseconds>(newNow - _last_time);
	_last_time = newNow;
	_accumulator += deltaTime.count();
	const double dt = f_delta * 1'000'000'000.0;

	size_t continuous_counter = 0;

	auto& time_ctx = _scene->ctx_or_set<MM::Components::TimeDelta>(f_delta, initial_delta_factor);
	time_ctx.tickDelta = f_delta * time_ctx.deltaFactor;

	// TODO: this is just cancer
	while (_accumulator >= static_cast<decltype(_accumulator)>(dt)){
		_accumulator -= static_cast<decltype(_accumulator)>(dt);
		continuous_counter++;

		::MM::EachSystemInScene(*_scene, [&](::MM::Scene& s, ::MM::System& fn) {
			fn(s, time_ctx.tickDelta);
		});

		TracyPlot("MM::Services::SimpleSceneService::_scene.alive", (int64_t)_scene->alive());
	}

	if (continuous_counter > 2) {
		LOG_SSS(fmt::format("had {} contiguous scene ticks!", continuous_counter));
	}
}

void SimpleSceneService::changeSceneFixedUpdate(Engine& engine) {
	if (_next_scene) {
		LOG_SSS("changing scene...");
		_scene = std::move(_next_scene);
		_scene->set<MM::Engine*>(&engine); // make engine accessible from scene
	}
}

void SimpleSceneService::changeScene(std::unique_ptr<Scene>&& new_scene) {
	if (_next_scene) {
		LOG_SSS("warn: already next scene enqueued! overwriting...");
	}

	_next_scene = std::move(new_scene);

	//// ensure there are systems
	//if (!_next_scene->try_ctx<MM::simple_scene::SystemsContainer>()) {
		//_next_scene->set<MM::simple_scene::SystemsContainer>();
	//}
}

void SimpleSceneService::changeSceneNow(std::unique_ptr<Scene>&& new_scene) {
	_scene = std::move(new_scene);
	//_scene->set<MM::Engine*>(&engine); // make engine accessible from scene

	//// ensure there are systems
	//if (!_scene->try_ctx<MM::simple_scene::SystemsContainer>()) {
		//_scene->set<MM::simple_scene::SystemsContainer>();
	//}
}


void SimpleSceneService::resetTime(void) {
	_last_time = clock::now();
	_accumulator = 0;
}

} // MM::Services

