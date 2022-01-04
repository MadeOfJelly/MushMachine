#pragma once

#include <nlohmann/json.hpp>

#include <mm/components/velocity2d_position_intent.hpp>

#include "./json_glm.hpp"

namespace MM::Components {
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Velocity2DPositionIntent, intent)
} // MM::Components

