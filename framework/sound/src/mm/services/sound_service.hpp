#pragma once

#include <soloud.h>

#include <mm/engine.hpp>

namespace MM::Services {

	class SoundService : public Service {
		public:
			SoLoud::Soloud engine;

		public:
			bool enable(Engine&) override;
			void disable(Engine&) override;

			const char* name(void) override { return "SoundService"; }

		public:
			SoundService(void);
			~SoundService(void);

			void printErrorString(SoLoud::result errorCode);
	};

} // MM::Services

