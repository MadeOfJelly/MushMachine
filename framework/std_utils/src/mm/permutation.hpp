#pragma once

#include <vector>
#include <numeric>
#include <algorithm>

namespace MM::std_utils {

// TODO: move to some lib in engine
// TODO: determain the ideal sort
// uses std::sort rn
template<typename F>
std::vector<size_t> generate_sort_permutation(size_t size, F&& sort_fn) {
	std::vector<size_t> index_mapping;

	index_mapping.resize(size);
	std::iota(index_mapping.begin(), index_mapping.end(), 0);

	std::sort(index_mapping.begin(), index_mapping.end(), sort_fn);

	return index_mapping;
}

// TODO: reimplement, this naive solution is kind of costly
template<typename ContainerType>
void apply_permutation(ContainerType& vec, const std::vector<size_t>& perm) {
	ContainerType new_vec(vec.size());

	for (size_t i = 0; i < vec.size(); i++) {
		new_vec[i] = vec[perm[i]]; // thx ec and random guy
	}

	vec = std::move(new_vec);
}

} // MM::std_utils

