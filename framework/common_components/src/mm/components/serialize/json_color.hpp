#pragma once

#include <nlohmann/json.hpp>

#include <mm/components/color.hpp>

#include "./json_glm.hpp"

namespace MM::Components {
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Color, color)
} // MM::Components

