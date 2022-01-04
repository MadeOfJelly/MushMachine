#pragma once

#include <nlohmann/json.hpp>

#include <mm/components/velocity2d_position.hpp>

#include "./json_glm.hpp"

namespace MM::Components {
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Velocity2DPosition, pos_vel)
} // MM::Components

