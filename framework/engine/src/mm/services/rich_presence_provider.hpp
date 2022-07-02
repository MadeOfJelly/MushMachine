#pragma once

#include <mm/engine.hpp>

namespace MM::Services {

// provides information for a RichPresenceConsumer
class RichPresenceProviderInterface : public Service {
	public:
		struct RichPresenceInformation {
			std::string app {"mm_app"};
			std::string status {"sleeping..."};
			std::string details {"~.~"};
		};

		// TODO: do i want this?
		//virtual bool infoChanged(void) { return true; }

		// ah yes copy
		virtual RichPresenceInformation get(void) = 0;
};

} // MM::Services

