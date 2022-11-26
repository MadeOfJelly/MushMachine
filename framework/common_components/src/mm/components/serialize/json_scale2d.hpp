#pragma once

#include <nlohmann/json.hpp>

#include <mm/components/scale2d.hpp>

#include "./json_glm.hpp"

namespace MM::Components {
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Scale2D, scale)
} // MM::Components

