#include "./player_velocity2d_system.hpp"

#include <entt/entity/registry.hpp>

#include <tracy/Tracy.hpp>

namespace MM::Systems {

void player_velocity2d(entt::view<entt::get_t<MM::Input::PlayerID, MM::Components::Velocity2D>> view, const MM::Engine* engine) {
	ZoneScopedN("MM::Systems::PlayerVelocity2D");

	auto& input_ss = engine->getService<MM::Services::InputService>();

	view.each([&input_ss](const MM::Input::PlayerID p_id, MM::Components::Velocity2D& v) {
		//const float movement_speed = 8.f; // apply via post processing

		auto vec_force = input_ss.getMoveForce(p_id);

		if (vec_force >= 0.01f) {
			v.velocity = input_ss.getMoveVec(p_id);
			//v.velocity *= vec_force;
		} else {
			v.velocity = {0.f, 0.f};
		}
	});
}

} // MM::Systems

