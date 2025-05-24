#pragma once

#include <mm/components/velocity2d_position.hpp>
#include <mm/components/velocity2d_rotation.hpp>
#include <mm/components/position2d.hpp>
#include <mm/components/rotation2d.hpp>
#include <mm/components/time_delta.hpp>

#include <entt/fwd.hpp>
#include <entt/entity/storage.hpp>

namespace MM::Systems {

	// non patching (on_update())
	void simple_positional_velocity(
		entt::view<entt::get_t<
			Components::Position2D,
			const Components::Velocity2DPosition
		>> view,
		const Components::TimeDelta& td
	);

	// patching (on_update())
	void simple_positional_velocity_patching(
		entt::registry& scene,
		entt::view<entt::get_t<
			Components::Position2D,
			const Components::Velocity2DPosition
		>> view,
		const Components::TimeDelta& td
	);

	// non patching (on_update())
	void simple_rotational_velocity(
		entt::view<entt::get_t<
			Components::Rotation2D,
			const Components::Velocity2DRotation
		>> view,
		const Components::TimeDelta& td
	);

	// patching (on_update())
	void simple_rotational_velocity_patching(
		entt::registry& scene,
		entt::view<entt::get_t<
			Components::Rotation2D,
			const Components::Velocity2DRotation
		>> view,
		const Components::TimeDelta& td
	);

} // MM::Systems

