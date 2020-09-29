#pragma once

#include <nlohmann/json.hpp>

#include <mm/components/velocity2d.hpp>

#include "./json_glm.hpp"

namespace MM::Components {
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Velocity2D, velocity, rotation)
} // MM::Components

