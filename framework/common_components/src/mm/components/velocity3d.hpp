#pragma once

#include <glm/vec3.hpp>

namespace MM::Components {

	struct Velocity3D {
		glm::vec3	velocity {0.f, 0.f, 0.f};
		glm::vec3	rotation {0.f, 0.f, 0.f};
	};

} // MM::Components

