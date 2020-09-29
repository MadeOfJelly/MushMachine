#pragma once

#include <nlohmann/json.hpp>

#include <mm/components/view_dir3d.hpp>

namespace MM::Components {
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ViewDir3D, yaw, pitch, roll)
} // MM::Components

