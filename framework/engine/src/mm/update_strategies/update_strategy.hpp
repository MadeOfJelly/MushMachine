#pragma once

#include "../engine_fwd.hpp"

#include <entt/core/hashed_string.hpp>

#include <mm/services/service.hpp>

#include <functional>
#include <string>
#include <vector>
#include <set>

// fwd / hack
namespace MM::Services {
	class ImGuiEngineTools;
}

namespace MM::UpdateStrategies {

using update_key_t = entt::id_type;

enum update_phase_t {
	PRE = 0, // for on-main-thread
	MAIN,
	POST // for on-main-thread
};

struct TaskInfo {
	update_key_t _key; // key for dependencies
	std::string _name; // unhashed key

	update_phase_t _phase = update_phase_t::MAIN;

	// updates we make dependents
	std::set<update_key_t> _dependents {};

	// this update also depends on (in the same phase)
	std::set<update_key_t> _dependencies {};

	std::function<void(Engine&)> _fn; // the actual payload

	public: // construction and assignment
		TaskInfo(void) = delete;
		TaskInfo(const TaskInfo&) = default;
		TaskInfo(TaskInfo&&) = default;

		// TODO: is this right??
		TaskInfo& operator=(const TaskInfo&) = default;
		TaskInfo& operator=(TaskInfo&&) = default;

		explicit TaskInfo(const std::string_view key_in) {
			key(key_in);
		}

	public: // builder interface
		// TODO: does this need to be public?
		TaskInfo& key(const std::string_view key) {
			_name = key;
			_key = entt::hashed_string::value(key.data(), key.size());
			return *this;
		}

		// default phase is main
		TaskInfo& phase(const update_phase_t phase) {
			_phase = phase;
			return *this;
		}

		template<typename Fn>
		TaskInfo& fn(Fn&& fn) {
			_fn = fn;
			return *this;
		}

		TaskInfo& precede(const std::string_view key) {
			_dependents.emplace(entt::hashed_string::value(key.data(), key.size()));
			return *this;
		}

		TaskInfo& succeed(const std::string_view key) {
			_dependencies.emplace(entt::hashed_string::value(key.data(), key.size()));
			return *this;
		}
};

// pure virtual interface for managing the update(-task) logic of the engine
class UpdateStrategy {
	public:
		virtual ~UpdateStrategy(void) {}

		virtual const char* name(void) = 0;

	protected: // the engine facing interface
		friend ::MM::Engine;

		// returns true on success
		// failure conditions may include:
		// - already en/dis-abled
		// - impossible dependencies?
		virtual bool enableService(const entt::id_type s_id, std::vector<TaskInfo>&& task_array) = 0;
		virtual bool disableService(const entt::id_type s_id) = 0;

		// runs one update
		virtual void doUpdate(MM::Engine& engine) = 0;

	public: // the user facing interface
		// WIP:

		// dont use, if you are not using it to modify the engine.
		// you usually dont need to use this, if you think you need to use this, you probably dont.
		virtual void addDeferred(std::function<void(Engine&)> function) = 0; // called after everything

		// adds a task, which does not need to be completed on the end of the tick.
		// warning: in a single-threaded environment, they will still be done on the main thread, so blocking is not a option
		// note: the US might decide to limit the amount of executed asyncs per tick (eg. when single-threaded)
		virtual void addAsync(std::function<void(Engine&)> function) = 0;
		//virtual std::future addAsync(std::function<void(Engine&)> function) = 0;
	protected: // engine tools inspector
		friend ::MM::Services::ImGuiEngineTools;

		virtual void forEachTask(std::function<bool(TaskInfo&)> fn) = 0;
};

} // MM::UpdateStrategies

