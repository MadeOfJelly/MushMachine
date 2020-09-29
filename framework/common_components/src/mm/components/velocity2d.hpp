//
// Created by FlaXxy on 29.07.2018.
//
#pragma once

#include <glm/vec2.hpp>

namespace MM::Components {

	struct Velocity2D {
		glm::vec2	velocity;
		float		rotation {0.f};
	};

} // MM::Components

