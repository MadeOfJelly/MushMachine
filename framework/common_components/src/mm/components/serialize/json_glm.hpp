#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <nlohmann/json.hpp>

namespace glm {

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(vec2, x, y)
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(vec3, x, y, z)
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(vec4, x, y, z, w)

}

