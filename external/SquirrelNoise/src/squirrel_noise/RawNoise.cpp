//-----------------------------------------------------------------------------------------------
// RawNoise.cpp
//
#include "./RawNoise.hpp"

namespace SquirrelNoise4 {

//-----------------------------------------------------------------------------------------------
// Fast hash of an int32 into a different (unrecognizable) uint32.
//
// Returns an uint32_t containing 32 reasonably-well-scrambled bits, based on the hash
//	of a given (signed) integer input parameter (position/index) and [optional] seed.  Kind of
//	like looking up a value in an infinitely large table of previously generated random numbers.
//
// The bit-noise constants and bit-shifts were evolved by a genetic algorithm using the
//	"BigCrush" test for fitness, and have so far produced excellent test results.
//
// I call this particular approach SquirrelNoise (version 4).
//
uint32_t Get1dNoiseUint32( int32_t positionX, uint32_t seed ) {
	const uint32_t BIT_NOISE1 = 0xD2A80A23; // 0b1101'0010'1010'1000'0000'1010'0010'0011;
	const uint32_t BIT_NOISE2 = 0xA884F197; // 0b1010'1000'1000'0100'1111'0001'1001'0111;
	const uint32_t BIT_NOISE3 = 0x1B56C4E9; // 0b0001'1011'0101'0110'1100'0100'1110'1001;

	uint32_t mangledBits = (uint32_t) positionX;
	mangledBits *= BIT_NOISE1;
	mangledBits += seed;
	mangledBits ^= (mangledBits >> 7);
	mangledBits += BIT_NOISE2;
	mangledBits ^= (mangledBits >> 8);
	mangledBits *= BIT_NOISE3;
	mangledBits ^= (mangledBits >> 11);
	return mangledBits;
}

} // SquirrelNoise4

