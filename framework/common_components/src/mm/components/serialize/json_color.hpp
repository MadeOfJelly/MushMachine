#pragma once

#include <nlohmann/json.hpp>

#include <mm/components/color.hpp>

#include "./json_glm.hpp"

namespace MM::Components {
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Color, color)
} // MM::Components

