#pragma once

#include "./update_strategy.hpp"

#include <memory>
#include <unordered_map>
#include <vector>
#include <set>

// HACK: welp
// fwd
namespace MM::Services {
	class ImGuiEngineTools;
}

namespace MM::UpdateStrategies {

class Sequential : public MM::UpdateStrategies::UpdateStrategy {
	friend MM::Services::ImGuiEngineTools;

	private:
		std::unordered_map<update_key_t, TaskInfo> _tasks;

		// the tasks a service has registered
		std::unordered_map<entt::id_type, std::set<update_key_t>> _service_tasks;

		std::vector<std::function<void(Engine&)>> _deferred_queue;
		std::vector<std::function<void(Engine&)>> _async_queue;

	private: // utils
		void doGraphSequential(MM::Engine& engine, const std::set<update_key_t>& tasks);

	public:
		Sequential(void) = default;
		virtual ~Sequential(void);

		const char* name(void) override { return "Sequential"; }

	protected: // engine facing interface
		bool enableService(const entt::id_type s_id, std::vector<TaskInfo>&& task_array) override;
		bool disableService(const entt::id_type s_id) override;

		void doUpdate(MM::Engine& engine) override;

	public: // user facing interface
		void addDeferred(std::function<void(Engine&)> function) override;

		void addAsync(std::function<void(Engine&)> function) override;

		size_t _max_async_per_tick = 5; // prevent blocking, this should be finetuned

	protected: // engine tools interface
		void forEachTask(std::function<bool(TaskInfo&)> fn) override {
			for (auto&& t : _tasks) {
				if (!fn(t.second)) {
					break;
				}
			}
		}
};

} // MM::UpdateStrategies

