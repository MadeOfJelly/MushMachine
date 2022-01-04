#pragma once

#include <nlohmann/json.hpp>

#include <mm/components/rotation2d.hpp>

#include "./json_glm.hpp"

namespace MM::Components {
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Rotation2D, rot)
} // MM::Components

