#pragma once

#include <nlohmann/json.hpp>

#include <mm/components/transform4x4.hpp>

#include "./json_glm.hpp"

namespace MM::Components {
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Transform4x4, trans)
} // MM::Components

