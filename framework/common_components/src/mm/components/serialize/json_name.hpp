#pragma once

#include <nlohmann/json.hpp>

#include <mm/components/name.hpp>

namespace MM::Components {
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Name, str)
} // MM::Components

