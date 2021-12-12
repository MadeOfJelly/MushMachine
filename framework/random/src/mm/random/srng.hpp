#pragma once

#include <squirrel_noise/RawNoise.hpp>
#include <mm/scalar_range2.hpp>

namespace MM::Random {

// Seeded (Pseudo-) Random Number Generator
struct SRNG {
	// make shuffle compat 
	// TODO: add more type info
	using result_type = uint32_t;

	uint32_t seed = 1337;
	int32_t pos = 0;

	// basic

	uint32_t getNext(void) {
		return SquirrelNoise4::Get1dNoiseUint32(pos++, seed);
	}

	float zeroToOne(void) {
		return SquirrelNoise4::Get1dNoiseZeroToOne(pos++, seed);
	}

	float negOneToOne(void) {
		return SquirrelNoise4::Get1dNoiseNegOneToOne(pos++, seed);
	}

	// advanced

	uint32_t minMax(uint32_t min, uint32_t max) {
		return (getNext() % ((max - min) + 1)) + min;
	}

	bool roll(float prob) {
		return zeroToOne() <= prob;
	}

	// more advanced

	// inclusive
	template<typename T>
	T range(const ScalarRange2<T>& range) {
		return (getNext() % ((range.max() - range.min()) + 1)) + range.min();
	}

	// for conviniece
	uint32_t operator()(void) {
		return getNext();
	}

	bool operator()(float prob) {
		return roll(prob);
	}

	// std:: distributions need those
	constexpr static uint32_t min(void) {
		return 0;
	}

	constexpr static uint32_t max(void) {
		return 0xffffffff;
	}
};

template<>
double SRNG::range(const ScalarRange2<double>& range);

template<>
float SRNG::range(const ScalarRange2<float>& range);

} // MM::Random

