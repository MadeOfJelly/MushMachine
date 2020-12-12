#pragma once

#include "../engine_fwd.hpp"

#include <mm/services/service.hpp>
#include <functional>
#include <string>
#include <vector>

namespace MM::UpdateStrategies {

using update_key_t = entt::id_type;

enum class update_phase_t {
	PRE, // for on-main-thread
	MAIN,
	POST // for on-main-thread
};

struct UpdateCreationInfo {
	update_key_t key; // key for dependencies

	std::string name; // for debugging

	std::function<void(Engine&)> fn; // the actual payload

	update_phase_t phase = update_phase_t::MAIN;

	bool auto_enable = true; // whether this update is enabled with the service

	// this update also depends on (in the same phase)
	std::vector<update_key_t> dependencies {};
};

// pure virtual interface for managing the update logic of the engine
class UpdateStrategy {
	public:
		virtual ~UpdateStrategy(void) {}

	protected: // the engine facing interface
		friend ::MM::Engine;

		// TODO: return something?
		virtual bool registerService(const entt::id_type s_id, std::vector<UpdateCreationInfo>&& info_array) = 0;

		// returns true on success
		// failure conditions may include:
		// - already en/dis-abled
		// - is auto_enable
		// - impossible dependencies
		virtual bool enableService(const entt::id_type s_id) = 0;
		virtual bool disableService(const entt::id_type s_id) = 0;

		// runs one update
		virtual void doUpdate(MM::Engine& engine) = 0;

	public: // the user facing interface
		// similar to *ableService, can only be used for non-auto_enable-updates
		virtual bool enable(const update_key_t key) = 0;
		virtual bool disable(const update_key_t key) = 0;

		// add extra dependencies into the tree, the user has the most knowlage about
		// the order the services should execute in.
		// A -> B (make A depend on B)
		virtual bool depend(const update_key_t A, const update_key_t B) = 0;

		// WIP:

		// dont use, if you are not using it to modify the engine.
		// you usualy dont need to use this, if you think you need to use this, you probably dont.
		//virtual void addFixedDefered(std::function<void(Engine&)> function) = 0;
		virtual void addDefered(std::function<void(Engine&)> function) = 0; // called after everything

		//virtual std::future addAsync(std::function<void(Engine&)> function) = 0;
};

} // MM::UpdateStrategies

