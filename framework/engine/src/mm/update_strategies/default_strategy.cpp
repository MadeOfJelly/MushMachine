#include "./default_strategy.hpp"

#include <cassert>
#include <functional>

namespace MM::UpdateStrategies {

#define __L_ASSERT(cond) if (!(cond)) { return false; }

SingleThreadedDefault::Graph& SingleThreadedDefault::getGraph(update_phase_t type) {
	using type_t = update_phase_t;

	switch (type) {
		case type_t::PRE:
			return _pre_graph;
		case type_t::MAIN:
			return _main_graph;
		case type_t::POST:
			return _post_graph;
	}

	return _main_graph; // unreachable
}

std::set<update_key_t>& SingleThreadedDefault::getActiveSet(update_phase_t type) {
	using type_t = update_phase_t;

	switch (type) {
		case type_t::PRE:
			return _pre_active;
		case type_t::MAIN:
			return _main_active;
		case type_t::POST:
			return _post_active;
	}

	return _main_active; // unreachable
}

void SingleThreadedDefault::runType(MM::Engine& engine, update_phase_t type) {
	auto aset = getActiveSet(type); // copy
	const auto& graph = getGraph(type);

	while (!aset.empty()) {
		for (const auto key : aset) {
			// check if dependencies are resolved (or dont exist)
			bool resolved = true;
			for (const auto deps : graph.at(key)) {
				if (aset.count(deps)) {
					resolved = false;
					break;
				}
			}

			if (resolved) {
				// do task
				_tasks[key].fn(engine);

				// delete task from set
				aset.erase(key);

				break; // this might be optional but makes the delete easy
			} else {
				// continue the for / dont do anything with this task yet
			}
		}
	}
}

SingleThreadedDefault::~SingleThreadedDefault(void) {
}

bool SingleThreadedDefault::registerService(const entt::id_type s_id, std::vector<UpdateCreationInfo>&& info_array) {
	__L_ASSERT(_service_tasks.count(s_id) == 0);

	// early out
	if (info_array.empty()) {
		return true;
	}

	auto& service_tasks = _service_tasks[s_id];

	for (const UpdateCreationInfo& reg_e : info_array) {
		// test if already exists
		__L_ASSERT(_tasks.count(reg_e.key) == 0);
		// also the graphs
		__L_ASSERT(_pre_graph.count(reg_e.key) == 0);
		__L_ASSERT(_main_graph.count(reg_e.key) == 0);
		__L_ASSERT(_post_graph.count(reg_e.key) == 0);

		// also the enabled taks
		__L_ASSERT(_pre_active.count(reg_e.key) == 0);
		__L_ASSERT(_main_active.count(reg_e.key) == 0);
		__L_ASSERT(_post_active.count(reg_e.key) == 0);

		// potentially check for cicles (can be done by provided decorator)
	}

	for (const UpdateCreationInfo& reg_e : info_array) {
		// add to tasks
		{
			auto& new_task = _tasks[reg_e.key];
			new_task.name = reg_e.name;
			new_task.fn = reg_e.fn;
			new_task.phase = reg_e.phase;
			new_task.auto_enable = reg_e.auto_enable;

			new_task.enabled = false;
		}

		// add relation to service
		service_tasks.emplace_back(reg_e.key);

		// fill in dependencies
		auto& graph = getGraph(reg_e.phase);
		graph[reg_e.key].insert(reg_e.dependencies.begin(), reg_e.dependencies.end());
	}

	return true;
}

bool SingleThreadedDefault::enable(const update_key_t key) {
	__L_ASSERT(_tasks.count(key) == 1);
	__L_ASSERT(_tasks[key].enabled == false);

	auto ret = getActiveSet(_tasks[key].phase).emplace(key);
	_tasks[key].enabled = true;

	return ret.second;
}

bool SingleThreadedDefault::disable(const update_key_t key) {
	__L_ASSERT(_tasks.count(key) == 1);
	__L_ASSERT(_tasks[key].enabled == true);

	auto& aset = getActiveSet(_tasks[key].phase);

	__L_ASSERT(aset.count(key) == 1);

	aset.erase(key);

	_tasks[key].enabled = false;

	return true;
}

bool SingleThreadedDefault::enableService(const entt::id_type s_id) {
	bool succ = true;
	for (const auto id : _service_tasks[s_id]) {
		auto& task = _tasks[id];

		// there should be no task running, if the service is no enabled!!
		assert(!task.enabled);

		if (task.auto_enable) {
			succ &= enable(id);
		}
	}

	return succ;
}

bool SingleThreadedDefault::disableService(const entt::id_type s_id) {
	bool succ = true;
	for (const auto id : _service_tasks[s_id]) {
		auto& task = _tasks[id];

		if (task.auto_enable) {
			assert(task.enabled); // this should never happen
		}

		if (task.enabled) {
			succ &= disable(id);
		}
	}

	return succ;
}

bool SingleThreadedDefault::depend(const update_key_t A, const update_key_t B) {
	// TODO: error checking lol

	if (_tasks.count(A) == 0) {
		return false; // can not add a dependecy of a non existing task
		// TODO: or do we?
	}

	if (_tasks.count(B) == 1) {
		if (_tasks[A].phase != _tasks[B].phase) {
			// cross graph tasks are not allowed
			return false;
		}
	}

	auto& graph = getGraph(_tasks[A].phase);
	auto ret = graph[A].emplace(B);
	return ret.second; // returns whether it was inserted (or already existed)
}

void SingleThreadedDefault::doUpdate(MM::Engine& engine) {
	// pre
	runType(engine, update_phase_t::PRE);

	// main
	runType(engine, update_phase_t::MAIN);

	// post
	runType(engine, update_phase_t::POST);

	// simulate async
	for (size_t i = 0; !_async_queue.empty() && i < _max_async_per_tick; i++) {
		_async_queue.back()(engine);
		_async_queue.pop_back();
	}

	if (!_deferred_queue.empty()) {
		for (auto&& fn : _deferred_queue) {
			fn(engine);
		}

		_deferred_queue.clear();
	}
}

void SingleThreadedDefault::addDeferred(std::function<void(Engine&)> function) {
	_deferred_queue.emplace_back(std::move(function));
}

void SingleThreadedDefault::addAsync(std::function<void(Engine&)> function) {
	_async_queue.emplace_back(std::move(function));
}

#undef __L_ASSERT

} // MM::UpdateStrategies

