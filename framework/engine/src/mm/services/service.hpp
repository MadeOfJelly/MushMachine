#pragma once

#include <vector>

namespace MM {

	class Engine;
	namespace UpdateStrategies {
		struct TaskInfo;
	}

	namespace Services {

		class Service {
			public:
				virtual ~Service(void) {}

				virtual const char* name(void) = 0;

				// tasks are to be filled in by the service impl
				virtual bool enable(Engine& engine, std::vector<UpdateStrategies::TaskInfo>& task_array) = 0;
				virtual void disable(Engine& engine) = 0;
		};

	} // Services

} // MM

