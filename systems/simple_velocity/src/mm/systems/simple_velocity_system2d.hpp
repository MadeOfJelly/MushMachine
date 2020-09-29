#pragma once

#include <functional>

#include <mm/components/velocity2d.hpp>
#include <mm/components/transform2d.hpp>

#include <mm/services/scene_service_interface.hpp>

namespace MM::Systems {
	inline void SimpleVelocity(Scene& scene, float delta) {
		scene.view<MM::Components::Transform2D, MM::Components::Velocity2D>().each([delta](auto, auto& t, auto& v) {
				t.position += v.velocity * delta;
				t.rotation += v.rotation * delta;
			}
		);
	}
}

