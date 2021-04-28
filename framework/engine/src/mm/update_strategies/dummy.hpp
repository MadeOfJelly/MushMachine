#pragma once

#include "./update_strategy.hpp"

// Rezz x Deadmau5 - hypnocurrency

namespace MM::UpdateStrategies {

// does nothing, even less then a mock class
// for testing only!
class Dummy : public UpdateStrategy {
	public:
		~Dummy(void) {}

		const char* name(void) override { return "Dummy"; }

	protected: // the engine facing interface
		bool enableService(const entt::id_type, std::vector<TaskInfo>&&) override { return true; }
		bool disableService(const entt::id_type) override { return true; }
		void doUpdate(MM::Engine&) override {}

	public: // the user facing interface
		void addDeferred(std::function<void(Engine&)>) override {}
		void addAsync(std::function<void(Engine&)>) override {}
};

} // MM::UpdateStrategies

