#pragma once

#include <functional>

#include <mm/components/velocity2d.hpp>
#include <mm/components/transform2d.hpp>
#include <mm/components/time_delta.hpp>

#include <mm/services/scene_service_interface.hpp>

namespace MM::Systems {
	inline void simple_velocity(entt::view<entt::exclude_t<>, Components::Transform2D, const Components::Velocity2D> view, const Components::TimeDelta& td) {
		view.each([delta = td.tickDelta](auto, auto& t, auto& v) {
				t.position += v.velocity * delta;
				t.rotation += v.rotation * delta;
			}
		);
	}
}

