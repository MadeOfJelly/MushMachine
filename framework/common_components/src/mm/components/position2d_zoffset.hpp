#pragma once

namespace MM::Components {

	// used to lift 2D into 3D space. like a z-index in css/svg
	struct Position2D_ZOffset {
		float z_offset {500.f}; // default camera allows values to be between 0 and 1000
	};

} // MM::Components

