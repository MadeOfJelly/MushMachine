#include "./player_velocity2d_system.hpp"

#include <entt/entity/registry.hpp>

#include <tracy/Tracy.hpp>

namespace MM::Systems {

void player_velocity2d(entt::view<entt::get_t<const MM::Input::PlayerID, MM::Components::Velocity2DPositionIntent>> view, MM::Engine& engine) {
	ZoneScopedN("MM::Systems::PlayerVelocity2D");

	auto& input_ss = engine.getService<MM::Services::InputService>();

	view.each([&input_ss](const MM::Input::PlayerID p_id, MM::Components::Velocity2DPositionIntent& v) {
		v.intent = input_ss.getMoveVec(p_id);
	});
}

} // MM::Systems

