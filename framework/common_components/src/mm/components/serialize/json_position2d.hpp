#pragma once

#include <nlohmann/json.hpp>

#include <mm/components/position2d.hpp>

#include "./json_glm.hpp"

namespace MM::Components {
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Position2D, pos)
} // MM::Components

