#pragma once

#include <string_view>
#include <cctype>

namespace MM::std_utils {

inline std::string_view trim_prefix(std::string_view sv) {
	while (!sv.empty() && std::isspace(sv.front())) {
		sv.remove_prefix(1);
	}

	return sv;
}

inline std::string_view trim_suffix(std::string_view sv) {
	while (!sv.empty() && std::isspace(sv.back())) {
		sv.remove_suffix(1);
	}

	return sv;
}

inline std::string_view trim(std::string_view sv) {
	return trim_suffix(trim_prefix(sv));
}

// src : https://marcoarena.wordpress.com/2017/01/03/string_view-odi-et-amo/
inline std::vector<std::string_view> split(std::string_view str, const char* delims) {
	std::vector<std::string_view> ret;

	std::string_view::size_type start = 0;
	auto pos = str.find_first_of(delims, start);
	while (pos != std::string_view::npos) {
		if (pos != start) {
			ret.push_back(str.substr(start, pos - start));
		}
		start = pos + 1;
		pos = str.find_first_of(delims, start);
	}
	if (start < str.length())
		ret.push_back(str.substr(start, str.length() - start));

	return ret;
}

} // MM::std_utils

