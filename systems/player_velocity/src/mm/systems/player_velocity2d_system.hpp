#pragma once

#include <mm/engine_fwd.hpp>
#include <mm/services/input_service.hpp>
#include <mm/components/velocity2d_position_intent.hpp>

#include <entt/entity/storage.hpp>

namespace MM::Systems {

	// this system transforms the input from the input_service into velocity intent
	void player_velocity2d(entt::view<entt::get_t<const MM::Input::PlayerID, MM::Components::Velocity2DPositionIntent>> view, MM::Engine& engine);

} // MM::Systems

