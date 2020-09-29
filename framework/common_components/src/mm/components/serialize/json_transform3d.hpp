#pragma once

#include <nlohmann/json.hpp>

#include <mm/components/transform3d.hpp>

#include "./json_glm.hpp"

namespace MM::Components {
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Transform3D, position, scale, rotation)
} // MM::Components

