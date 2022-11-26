#pragma once

#include <nlohmann/json.hpp>

#include <mm/components/position2d_zoffset.hpp>

#include "./json_glm.hpp"

namespace MM::Components {
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Position2D_ZOffset, z_offset)
} // MM::Components

