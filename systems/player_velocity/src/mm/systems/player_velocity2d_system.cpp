#include "./player_velocity2d_system.hpp"

#include <entt/entity/registry.hpp>

#include <mm/components/velocity2d.hpp>

#include <mm/services/input_service.hpp>

#include <tracy/Tracy.hpp>

namespace MM::Systems {
	void PlayerVelocity2D(Scene& scene, float) {
		ZoneScopedN("MM::Systems::PlayerVelocity2D");

		MM::Engine* engine_ptr = scene.ctx<MM::Engine*>();
		auto& input_ss = engine_ptr->getService<MM::Services::InputService>();

		scene.view<MM::Input::PlayerID, MM::Components::Velocity2D>().each(
			[&input_ss](const MM::Input::PlayerID p_id, MM::Components::Velocity2D& v) {
				//const float movement_speed = 8.f; // apply via post processing

				auto vec_force = input_ss.getMoveForce(p_id);

				if (vec_force >= 0.01f) {
					v.velocity = input_ss.getMoveVec(p_id);
					v.velocity *= vec_force;
					//v.velocity *= movement_speed;
				} else {
					v.velocity = {0.f, 0.f};
				}
			}
		);
	}
}

