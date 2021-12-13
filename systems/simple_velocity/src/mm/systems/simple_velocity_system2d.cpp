#include "./simple_velocity_system2d.hpp"

#include <entt/entity/registry.hpp>

namespace MM::Systems {

	void simple_velocity(
		entt::view<entt::get_t<
			Components::Transform2D,
			const Components::Velocity2D
		>> view,
		const Components::TimeDelta& td
	) {
		view.each([delta = td.tickDelta](auto, auto& t, auto& v) {
				t.position += v.velocity * delta;
				t.rotation += v.rotation * delta;
			}
		);
	}

} // MM::Systems

