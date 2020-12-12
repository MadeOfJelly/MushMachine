#pragma once

#include <vector>

namespace MM {

	class Engine;
	namespace UpdateStrategies {
		struct UpdateCreationInfo;
	}

	namespace Services {

		class Service {
			public:
				virtual ~Service(void) {}

				virtual const char* name(void) = 0;

				virtual bool enable(Engine& engine) = 0;
				virtual void disable(Engine& engine) = 0;

				// optional, only if service actually needs to be part of the update loop
				virtual std::vector<UpdateStrategies::UpdateCreationInfo> registerUpdates(void) { return {}; }
		};

	} // Services

} //MM

