#pragma once

// TODO: test

#include <mm/string_view_split.hpp>

// TODO: make proper namespace
namespace MM {

// removes './'s and resolves '../'
// returns false if too many ..
inline bool path_shorten(std::string& path) {
	auto splits = std_utils::split(path, "/");
	std::vector<std::string_view> new_splits;

	for (size_t i = 0; i < splits.size(); i++) {
		if (splits[i] == "..") {
			if (new_splits.empty())
				return false;

			new_splits.pop_back();
		} else if (splits[i] == ".") {
			// skip
		} else {
			new_splits.emplace_back(splits[i]);
		}
	}

	std::string new_path;
	for (auto& s : new_splits) {
		new_path += "/";
		new_path += s;
	}
	path = new_path;

	return true;
}

// remove the file name from the path
inline std::string base_path(std::string_view path) {
	auto pos = path.find_last_of("/");
	if (pos == path.npos)
		return std::string(path); // TODO: make sure there is a '/'

	return std::string(path.substr(0, pos+1));
}

} // MM

