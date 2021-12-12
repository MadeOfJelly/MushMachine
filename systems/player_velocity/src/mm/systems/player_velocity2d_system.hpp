#pragma once

#include <mm/engine_fwd.hpp>
#include <entt/entity/utility.hpp>
#include <mm/services/input_service.hpp>
#include <mm/components/velocity2d.hpp>

namespace MM::Systems {

	// this system transforms the input from the input_service into velocity
	void player_velocity2d(entt::view<entt::exclude_t<>, MM::Input::PlayerID, MM::Components::Velocity2D> view, const MM::Engine* engine);

} // MM::Systems

