#include "./sequential_strategy.hpp"

#include "./tasking_utils.hpp"

#include <cassert>
#include <functional>

#include <mm/logger.hpp>
#include <tracy/Tracy.hpp>

namespace MM::UpdateStrategies {

//#define __L_ASSERT(cond) if (!(cond)) { return false; }
void Sequential::doGraphSequential(MM::Engine& engine, const std::set<update_key_t>& tasks) {
	Graph graph = build_task_graph(tasks,
		[this](update_key_t key) -> const TaskInfo& {
			return _tasks.at(key);
		}
	);

	walk_graph_sequential(graph, [this, &engine](update_key_t task_id  ) {
		_tasks.at(task_id)._fn(engine);
	});
}

Sequential::~Sequential(void) {
}

bool Sequential::enableService(const entt::id_type service_id, std::vector<TaskInfo>&& task_array) {
	if (_service_tasks.count(service_id)) {
		// error
		return false;
	}

	auto& service = _service_tasks[service_id];

	for (const auto& task : task_array) {
		if (_tasks.count(task._key)) {
			entt::id_type other_service = 0;
			for (const auto& it : _service_tasks) {
				if (it.second.count(task._key)) {
					other_service = it.first;
				}
			}

			// TODO: this is cryptic
			SPDLOG_ERROR("service '{}' adding task with taken id '{}'/'{}'. taken by '{}'", service_id, task._key, task._name, other_service);

			continue;
		}

		_tasks.emplace(task._key, task);
		service.emplace(task._key);
	}

	return true;
}

bool Sequential::disableService(const entt::id_type service_id) {
	if (!_service_tasks.count(service_id)) {
		// error
		return false;
	}

	for (const auto task_id : _service_tasks[service_id]) {
		_tasks.erase(task_id);
	}

	_service_tasks.erase(service_id);

	return true;
}

void Sequential::doUpdate(MM::Engine& engine) {
	ZoneScopedN("MM::UpdateStrategies::Sequential::doUpdate");
	// TODO: caching
	std::set<update_key_t> pre_tasks;
	std::set<update_key_t> main_tasks;
	std::set<update_key_t> post_tasks;

	for (const auto& [task_id, task] : _tasks) {
		switch (task._phase) {
			case update_phase_t::PRE:
				pre_tasks.emplace(task_id);
				break;
			case update_phase_t::MAIN:
				main_tasks.emplace(task_id);
				break;
			case update_phase_t::POST:
				post_tasks.emplace(task_id);
				break;
		}
	}

	{ // pre
		ZoneScopedN("MM::UpdateStrategies::Sequential::doUpdate::pre");
		doGraphSequential(engine, pre_tasks);
	}

	{ // main
		ZoneScopedN("MM::UpdateStrategies::Sequential::doUpdate::main");
		doGraphSequential(engine, main_tasks);
	}

	{ // post
		ZoneScopedN("MM::UpdateStrategies::Sequential::doUpdate::post");
		doGraphSequential(engine, post_tasks);
	}

	{ // simulate async
		ZoneScopedN("MM::UpdateStrategies::Sequential::doUpdate::async");
		for (size_t i = 0; !_async_queue.empty() && i < _max_async_per_tick; i++) {
			_async_queue.back()(engine);
			_async_queue.pop_back();
		}
	}

	{
		ZoneScopedN("MM::UpdateStrategies::Sequential::doUpdate::deferred");
		if (!_deferred_queue.empty()) {
			for (auto&& fn : _deferred_queue) {
				fn(engine);
			}

			_deferred_queue.clear();
		}
	}
}

void Sequential::addDeferred(std::function<void(Engine&)> function) {
	_deferred_queue.emplace_back(std::move(function));
}

void Sequential::addAsync(std::function<void(Engine&)> function) {
	_async_queue.emplace_back(std::move(function));
}

//#undef __L_ASSERT

} // MM::UpdateStrategies

