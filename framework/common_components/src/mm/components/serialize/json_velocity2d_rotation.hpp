#pragma once

#include <nlohmann/json.hpp>

#include <mm/components/velocity2d_rotation.hpp>

#include "./json_glm.hpp"

namespace MM::Components {
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Velocity2DRotation, rot_vel)
} // MM::Components

