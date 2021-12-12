#pragma once

#include <mm/engine_fwd.hpp>

// fwd
namespace MM::OpenGL::RenderTasks {
	struct FastSkyContext;
}
namespace MM::Components {
	struct TimeDelta;
}

namespace MM::Systems {

	// this system updates time and sun depending on time with the time delta
	void fast_sky_sun(MM::OpenGL::RenderTasks::FastSkyContext& c, const MM::Components::TimeDelta& delta);

} // MM::Systems

