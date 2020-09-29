#pragma once

namespace MM {

	class Engine;

	namespace Services {

		class Service {
			public:
				virtual ~Service(void) {}

				virtual const char* name(void) { return "UnNamedService"; }
				//virtual const char* name(void) = 0; // use this to find unnamed services

				// required
				virtual bool enable(Engine& engine) = 0;
				virtual void disable(Engine& engine) = 0;
		};

	} // Services

} //MM

