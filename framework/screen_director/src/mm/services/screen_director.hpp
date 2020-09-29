#pragma once

#include <mm/engine.hpp>

//#include <vector> // in engine.hpp
#include <map>

namespace MM::Services {

class ScreenDirector : public Service {
	public:
		const char* name(void) override { return "ScreenDirector"; }

		// enable switches to queued_screen_index
		bool enable(MM::Engine&) override;
		void disable(MM::Engine&) override;

	public:
		struct Screen {
			// lists of services relevant for this screen, disable and enable are called when its changed to
			std::vector<Engine::service_family_type> start_disable;
			std::vector<Engine::service_family_type> start_enable;

			// register provider, when its changed to
			std::vector<
				std::pair<
					Engine::service_family_type,	// I
					Engine::service_family_type		// T
				>
			> start_provide;

			// lists of services relevant for this screen, disable and enable are called when its changed from
			std::vector<Engine::service_family_type> end_disable;
			std::vector<Engine::service_family_type> end_enable;

			// called when its changed to, after services disable and enable
			std::function<void(MM::Engine&)> start_fn = [](MM::Engine&){};

			// called when the screen changed from
			std::function<void(MM::Engine&)> end_fn = [](MM::Engine&){};
		};

		std::map<std::string, Screen> screens;

		// TODO: private?
		std::string curr_screen_id = "";
		std::string queued_screen_id = "";

	private:
		MM::Engine::FunctionDataHandle _update_handle;
		void update(MM::Engine& engine);

	private:
		// INTERNAL helper, to reduce redundant code
		void startScreen(MM::Engine& engine, Screen& screen);
		void endScreen(MM::Engine& engine, Screen& screen);

	public:
		void changeScreenTo(MM::Engine& engine, const std::string id);
		void queueChangeScreenTo(const std::string& id);
};

} // MM::Services

