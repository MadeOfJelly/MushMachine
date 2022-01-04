#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <nlohmann/json.hpp>

namespace glm {

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(vec2, x, y)
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(vec3, x, y, z)
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(vec4, x, y, z, w)

	//NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(mat4x4, [0], y, z, w)
    inline void to_json(nlohmann::json& nlohmann_json_j, const mat4x4& nlohmann_json_t) {
		// TODO: make 2d array?
		nlohmann_json_j = nlohmann::json::array_t{};
		nlohmann_json_j[0] = nlohmann_json_t[0];
		nlohmann_json_j[1] = nlohmann_json_t[1];
		nlohmann_json_j[2] = nlohmann_json_t[2];
		nlohmann_json_j[3] = nlohmann_json_t[3];
	}
    inline void from_json(const nlohmann::json& nlohmann_json_j, mat4x4& nlohmann_json_t) {
		if (!nlohmann_json_j.is_array()) {
			//throw nlohmann::json::type_error::create(0, "", nlohmann_json_j);
			assert(false && "expected array");
			return; // TODO: dont fail silently
		}

		nlohmann_json_j.at(0).get_to(nlohmann_json_t[0]);
		nlohmann_json_j.at(1).get_to(nlohmann_json_t[1]);
		nlohmann_json_j.at(2).get_to(nlohmann_json_t[2]);
		nlohmann_json_j.at(3).get_to(nlohmann_json_t[3]);
	}
}

