#pragma once

#include "./update_strategy.hpp"

#include <memory>
#include <unordered_map>
#include <vector>
#include <set>

namespace MM::UpdateStrategies {

class SingleThreadedDefault : public MM::UpdateStrategies::UpdateStrategy {
	private:
		struct Task {
			std::string name;
			std::function<void(Engine&)> fn;
			update_phase_t phase;
			bool auto_enable;

			bool enabled = false;
		};

		std::unordered_map<update_key_t, Task> _tasks;

		// the tasks a service has registered
		std::unordered_map<entt::id_type, std::vector<update_key_t>> _service_tasks;

		// tasks dependencies
		using Graph = std::unordered_map<update_key_t, std::set<update_key_t>>;
		// TODO: do vector for perf?

		Graph _pre_graph;
		Graph _main_graph;
		Graph _post_graph;

		std::set<update_key_t> _pre_active;
		std::set<update_key_t> _main_active;
		std::set<update_key_t> _post_active;

		std::vector<std::function<void(Engine&)>> _defered_queue;

	private:
		Graph& getGraph(update_phase_t phase);
		std::set<update_key_t>& getActiveSet(update_phase_t phase);

		void runType(MM::Engine& engine, update_phase_t phase);

	public:
		SingleThreadedDefault(void) = default;
		virtual ~SingleThreadedDefault(void);

	protected:
		bool registerService(const entt::id_type s_id, std::vector<UpdateCreationInfo>&& info_array) override;

		bool enableService(const entt::id_type s_id) override;
		bool disableService(const entt::id_type s_id) override;

		void doUpdate(MM::Engine& engine) override;

	public:
		bool enable(const update_key_t key) override;
		bool disable(const update_key_t key) override;

		bool depend(const update_key_t A, const update_key_t B) override;

		void addDefered(std::function<void(Engine&)> function) override;
};

} // MM::UpdateStrategies

