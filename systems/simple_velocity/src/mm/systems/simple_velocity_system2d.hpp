#pragma once

#include <mm/components/velocity2d.hpp>
#include <mm/components/transform2d.hpp>
#include <mm/components/time_delta.hpp>

#include <mm/services/scene_service_interface.hpp>

namespace MM::Systems {

	void simple_velocity(
		entt::view<entt::get_t<
			Components::Transform2D,
			const Components::Velocity2D
		>> view,
		const Components::TimeDelta& td
	);

} // MM::Systems

