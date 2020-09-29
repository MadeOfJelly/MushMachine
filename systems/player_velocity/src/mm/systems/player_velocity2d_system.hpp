#pragma once

//#include <mm/services/scene_service_interface.hpp>
#include <mm/engine_fwd.hpp>

namespace MM::Systems {

	// this system transforms the input from the input_ss into velocity
	// uses Components::Velocity2D, PlayerID
	void PlayerVelocity2D(Scene& scene, float delta);

} // MM::Systems

