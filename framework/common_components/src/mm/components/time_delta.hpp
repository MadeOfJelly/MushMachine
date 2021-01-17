#pragma once

namespace MM::Components {

	// this is a context-variable / "component-singleton".
	// it is used to retirve timing information about the tick.
	// it is "read-only" for the systems and filled in by the service
	// holding the scene.
	struct TimeDelta {
		float tickDelta = 1.f/60.f;

		// this is optionally filled in
		// it turned out, that sound playback needs the factor
		float deltaFactor = 1.f;
	};
}

