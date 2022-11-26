#pragma once

#include <nlohmann/json.hpp>

#include <mm/components/name.hpp>

namespace MM::Components {
	// TODO: manual with str len limit
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Name, str)
} // MM::Components

