#include "./simple_velocity_system2d.hpp"
#include "mm/components/rotation2d.hpp"
#include "mm/components/velocity2d_position.hpp"
#include "mm/components/velocity2d_rotation.hpp"

#include <entt/entity/registry.hpp>

namespace MM::Systems {

	void simple_positional_velocity(
		entt::view<entt::get_t<
			Components::Position2D,
			const Components::Velocity2DPosition
		>> view,
		const Components::TimeDelta& td
	) {
		view.each([delta = td.tickDelta](auto& t, auto& v) {
			t.pos += v.pos_vel * delta;
		});
	}

	void simple_positional_velocity_patching(
		entt::registry& scene,
		entt::view<entt::get_t<
			Components::Position2D,
			const Components::Velocity2DPosition
		>> view,
		const Components::TimeDelta& td
	) {
		view.each([&scene, delta = td.tickDelta](const auto e, auto& t, auto& v) {
			if (v.pos_vel.x != 0.f || v.pos_vel.y != 0.f) {
				t.pos += v.pos_vel * delta;
				scene.patch<MM::Components::Position2D>(e);
			}
		});
	}

	void simple_rotational_velocity(
		entt::view<entt::get_t<
			Components::Rotation2D,
			const Components::Velocity2DRotation
		>> view,
		const Components::TimeDelta& td
	) {
		view.each([delta = td.tickDelta](auto& r, auto& v) {
			r.rot += v.rot_vel * delta;
		});
	}

	void simple_rotational_velocity_patching(
		entt::registry& scene,
		entt::view<entt::get_t<
			Components::Rotation2D,
			const Components::Velocity2DRotation
		>> view,
		const Components::TimeDelta& td
	) {
		view.each([&scene, delta = td.tickDelta](const auto e, auto& r, auto& v) {
			if (v.rot_vel != 0.f) {
				r.rot += v.rot_vel * delta;
				scene.patch<MM::Components::Rotation2D>(e);
			}
		});
	}

} // MM::Systems

