#pragma once

#include <squirrel_noise/RawNoise.hpp>
#include <mm/scalar_range2.hpp>

namespace MM::Random {

// Seeded (Pseudo-) Random Number Generator
struct SRNG {
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
	// TODO: test for floats
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
};

} // MM::Random

