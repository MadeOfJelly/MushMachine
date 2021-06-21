#include "./srng.hpp"

namespace MM::Random {

template<>
double SRNG::range(const ScalarRange2<double>& range) {
	return zeroToOne() * (range.max() - range.min()) + range.min();
}

template<>
float SRNG::range(const ScalarRange2<float>& range) {
	return zeroToOne() * (range.max() - range.min()) + range.min();
}

} // MM::Random

