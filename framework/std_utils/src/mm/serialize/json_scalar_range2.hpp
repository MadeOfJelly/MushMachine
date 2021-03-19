#pragma once

#include <nlohmann/json.hpp>

#include "../scalar_range2.hpp"

namespace MM {

template<typename T>
void to_json(nlohmann::json& j, const ScalarRange2<T>& r) {
	j = nlohmann::json{
		{"min", r.min()},
		{"max", r.max()}
	};
}

template<typename T>
void from_json(const nlohmann::json& j, ScalarRange2<T>& r) {
	j.at("min").get_to(r.min());
	j.at("max").get_to(r.max());

	r.sanitize();
}

} // MM

