#pragma once

#include <nlohmann/json.hpp>

#include <mm/components/velocity3d.hpp>

#include "./json_glm.hpp"

namespace MM::Components {
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Velocity3D, velocity, rotation)
} // MM::Components

