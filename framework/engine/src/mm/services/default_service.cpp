#include "./default_service.hpp"

namespace MM::Services {

// TODO: error handling
bool DefaultService::enable(Engine& engine) {
	{
		_func_handles[0] = engine.addUpdate([this](Engine& e) { this->preSceneUpdate(e); });
		auto tmp_lock = _func_handles[0].lock();
		tmp_lock->priority = 1;
		tmp_lock->name = "DefaultService::preSceneUpdate";
	}

	{
		_func_handles[1] = engine.addUpdate([this](Engine& e) { this->postSceneUpdate(e); });
		auto tmp_lock = _func_handles[1].lock();
		tmp_lock->priority = -1;
		tmp_lock->name = "DefaultService::postSceneUpdate";
	}

	{
		_func_handles[2] = engine.addFixedUpdate([this](Engine& e) { this->preSceneFixedUpdate(e); });
		auto tmp_lock = _func_handles[2].lock();
		tmp_lock->priority = 1;
		tmp_lock->name = "DefaultService::preSceneFixedUpdate";
	}

	{
		_func_handles[3] = engine.addFixedUpdate([this](Engine& e) { this->postSceneFixedUpdate(e); });
		auto tmp_lock = _func_handles[3].lock();
		tmp_lock->priority = -1;
		tmp_lock->name = "DefaultService::postSceneFixedUpdate";
	}

	return true;
}

void DefaultService::disable(Engine& engine) {
	engine.removeUpdate(_func_handles[0]);
	engine.removeUpdate(_func_handles[1]);

	engine.removeFixedUpdate(_func_handles[2]);
	engine.removeFixedUpdate(_func_handles[3]);

	_func_handles[0].reset();
	_func_handles[1].reset();
	_func_handles[2].reset();
	_func_handles[3].reset();
}

} // MM::Services

