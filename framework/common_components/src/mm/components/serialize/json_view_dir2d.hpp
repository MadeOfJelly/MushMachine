#pragma once

#include <nlohmann/json.hpp>

#include <mm/components/view_dir2d.hpp>

namespace MM::Components {
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ViewDir2D, dir)
} // MM::Components

