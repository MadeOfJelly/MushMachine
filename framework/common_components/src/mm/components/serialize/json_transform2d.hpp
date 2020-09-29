#pragma once

#include <nlohmann/json.hpp>

#include <mm/components/transform2d.hpp>

#include "./json_glm.hpp"

namespace MM::Components {
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Transform2D, position, scale, rotation)
} // MM::Components

