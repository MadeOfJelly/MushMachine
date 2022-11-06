#pragma once

#include <mm/engine.hpp>

namespace MM::Services {

// counts X times and stops engine
// useful for automatic testing
class CountDown : public Service {
	public:
		CountDown(int64_t start) : _counter(start) {}

		const char* name(void) override { return "CountDown"; }

		bool enable(Engine&, std::vector<UpdateStrategies::TaskInfo>& task_array) override {
			// add task
			task_array.push_back(
				UpdateStrategies::TaskInfo{"CountDown::tick"}
				.phase(UpdateStrategies::update_phase_t::POST)
				.fn([this](Engine& e) { this->tick(e); })
			);

			return true;
		}

		void disable(Engine&) override {}

	private:

		void tick(Engine& engine) {
			_counter--;
			if (_counter == 0) {
				engine.stop();
			}
		}

	private:
		int64_t _counter = 0;

};

} // MM::Services

