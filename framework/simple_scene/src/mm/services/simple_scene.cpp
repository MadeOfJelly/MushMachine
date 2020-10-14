#include "./simple_scene.hpp"

//#include "../systems_container.hpp"

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

	{
		_f_update_handle = engine.addFixedUpdate([this](Engine& e) { this->sceneFixedUpdate(e); });
		if (_f_update_handle.expired()) {
			return false;
		}

		auto tmp_lock = _f_update_handle.lock();
		tmp_lock->priority = 0;
	}

	{
		_f_update_changer_handle = engine.addFixedUpdate([this](Engine& e) { this->changeSceneFixedUpdate(e); });
		if (_f_update_changer_handle.expired()) {
			return false;
		}

		auto tmp_lock = _f_update_changer_handle.lock();
		tmp_lock->priority = -100;
	}

	return true;
}

void SimpleSceneService::disable(Engine& engine) {
	if (!_f_update_handle.expired()) {
		engine.removeFixedUpdate(_f_update_handle);
		_f_update_handle.reset();
	}

	if (!_f_update_changer_handle.expired()) {
		engine.removeFixedUpdate(_f_update_changer_handle);
		_f_update_changer_handle.reset();
	}
}

void SimpleSceneService::sceneFixedUpdate(Engine& engine) {
	ZoneScoped;
	const auto f_delta = engine.getFixedDeltaTime();

	::MM::EachSystemInScene(*_scene, [&](::MM::Scene& s, ::MM::System& fn) {
		fn(s, f_delta);
	});

	TracyPlot("MM::Services::SimpleSceneService::_scene.alive", (int64_t)_scene->alive());
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

} // MM::Services

