#pragma once

#include <mm/engine_fwd.hpp>

namespace MM::Systems {

	// this system updates time and sun depending on time with the time delta
	void FastSkySun(Scene& scene, float delta);

} // MM::Systems

