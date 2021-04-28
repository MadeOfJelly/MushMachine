#pragma once

#include <mm/update_strategies/update_strategy.hpp>

namespace MM::UpdateStrategies {

// all tasks and it dependencies
using Graph = std::unordered_map<update_key_t, std::set<update_key_t>>;

template<typename FN>
void walk_graph_sequential(const Graph& graph, FN&& fn) {
	// set of all tasks, each completed task will be removed from this until none are left
	std::set<update_key_t> unworked_node_set{};
	for (auto it = graph.begin(); it != graph.end(); it++) {
		unworked_node_set.emplace(it->first);
	}

	// TODO: check for inf-loops
	while (!unworked_node_set.empty()) {
		for (auto node_it = unworked_node_set.begin(); node_it != unworked_node_set.end();) {
			// check if deps are resolved
			bool resolved = true;
			for (const auto dep : graph.at(*node_it)) {
				if (unworked_node_set.count(dep)) {
					resolved = false;
					break;
				}
			}

			if (resolved) {
				// to task
				fn(*node_it);

				node_it = unworked_node_set.erase(node_it);
			} else {
				node_it++;
			}
		}
	}
}

template<typename TaskGet>
Graph build_task_graph(const std::set<update_key_t>& tasks, TaskGet&& task_get) {
	Graph graph;

	// phase 1: create all tasks
	for (const auto task_id : tasks) {
		graph[task_id];
	}

	// phase 2: do all dependencies (and dependents)
	for (const auto task_id : tasks) {
		auto& graph_node = graph[task_id];

		{ // dependencies
			//graph_node.merge(task_get(task_id)._dependencies);
			const auto& dependencies = task_get(task_id)._dependencies;
			graph_node.insert(dependencies.cbegin(), dependencies.cend());
		}

		// dependents
		for (const auto dependent : task_get(task_id)._dependents) {
			if (graph.count(dependent)) {
				graph[dependent].emplace(task_id);
			}
		}
	}

	return graph;
}

} // MM::UpdateStrategies

