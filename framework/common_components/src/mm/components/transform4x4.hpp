#pragma once

#include <glm/mat4x4.hpp>

namespace MM::Components {

	// tag/flag to track dirty positional data, to reduce computation.
	struct DirtyTransformTag {};

	struct Transform4x4 {
		glm::mat4x4 trans {1.f};
	};

} // MM::Components

