//-----------------------------------------------------------------------------------------------
// RawNoise.hpp
//
#pragma once

#include <cstdint>

namespace SquirrelNoise4 {

/////////////////////////////////////////////////////////////////////////////////////////////////
// SquirrelNoise4 - Squirrel's Raw Noise utilities (version 4)
//
// This code is made available under the Creative Commons attribution 3.0 license (CC-BY-3.0 US):
//	Attribution in source code comments (even closed-source/commercial code) is sufficient.
//	License summary and text available at: https://creativecommons.org/licenses/by/3.0/us/
//
// These noise functions were written by Squirrel Eiserloh as a cheap and simple substitute for
//	the [sometimes awful] bit-noise sample code functions commonly found on the web, many of which
//	are hugely biased or terribly patterned, e.g. having bits which are on (or off) 75% or even
//	100% of the time (or are WAY too overkill-and-slow for our needs, such as MD5 or SHA).
//
// Note: This is work in progress, and has not yet been tested thoroughly.  Use at your own risk.
//	Please report any bugs, issues, or bothersome cases to SquirrelEiserloh at gmail.com.
//
// The following functions are all based on a simple bit-noise hash function which returns an
//	uint32_t containing 32 reasonably-well-scrambled bits, based on a given (signed)
//	integer input parameter (position/index) and [optional] seed.  Kind of like looking up a
//	value in an infinitely large [non-existent] table of previously rolled random numbers.
//
// These functions are deterministic and random-access / order-independent (i.e. state-free),
//	so they are particularly well-suited for use in smoothed/fractal/simplex/Perlin noise
//	functions and out-of-order (or on-demand) procedural content generation (i.e. that mountain
//	village is the same whether you generated it first or last, ahead of time or just now).
//
// The N-dimensional variations simply hash their multidimensional coordinates down to a single
//	32-bit index and then proceed as usual, so while results are not unique they should
//	(hopefully) not seem locally predictable or repetitive.
//
// Modified by Erik Scholz 2021, no change to the license.
//
/////////////////////////////////////////////////////////////////////////////////////////////////


//-----------------------------------------------------------------------------------------------
// Raw pseudorandom noise functions (random-access / deterministic).  Basis of all other noise.
//
uint32_t Get1dNoiseUint32( int32_t index, uint32_t seed=0 );
uint32_t Get2dNoiseUint32( int32_t indexX, int32_t indexY, uint32_t seed=0 );
uint32_t Get3dNoiseUint32( int32_t indexX, int32_t indexY, int32_t indexZ, uint32_t seed=0 );
uint32_t Get4dNoiseUint32( int32_t indexX, int32_t indexY, int32_t indexZ, int32_t indexT, uint32_t seed=0 );

//-----------------------------------------------------------------------------------------------
// Same functions, mapped to floats in [0,1] for convenience.
//
float Get1dNoiseZeroToOne( int32_t index, uint32_t seed=0 );
float Get2dNoiseZeroToOne( int32_t indexX, int32_t indexY, uint32_t seed=0 );
float Get3dNoiseZeroToOne( int32_t indexX, int32_t indexY, int32_t indexZ, uint32_t seed=0 );
float Get4dNoiseZeroToOne( int32_t indexX, int32_t indexY, int32_t indexZ, int32_t indexT, uint32_t seed=0 );

//-----------------------------------------------------------------------------------------------
// Same functions, mapped to floats in [-1,1] for convenience.
//
float Get1dNoiseNegOneToOne( int32_t index, uint32_t seed=0 );
float Get2dNoiseNegOneToOne( int32_t indexX, int32_t indexY, uint32_t seed=0 );
float Get3dNoiseNegOneToOne( int32_t indexX, int32_t indexY, int32_t indexZ, uint32_t seed=0 );
float Get4dNoiseNegOneToOne( int32_t indexX, int32_t indexY, int32_t indexZ, int32_t indexT, uint32_t seed=0 );


/////////////////////////////////////////////////////////////////////////////////////////////////
// Simple functions inlined below
/////////////////////////////////////////////////////////////////////////////////////////////////


//-----------------------------------------------------------------------------------------------
inline uint32_t Get2dNoiseUint32( int32_t indexX, int32_t indexY, uint32_t seed )
{
	const int32_t PRIME_NUMBER = 198491317; // Large prime number with non-boring bits
	return Get1dNoiseUint32( indexX + (PRIME_NUMBER * indexY), seed );
}


//-----------------------------------------------------------------------------------------------
inline uint32_t Get3dNoiseUint32( int32_t indexX, int32_t indexY, int32_t indexZ, uint32_t seed )
{
	const int32_t PRIME1 = 198491317; // Large prime number with non-boring bits
	const int32_t PRIME2 = 6542989; // Large prime number with distinct and non-boring bits
	return Get1dNoiseUint32( indexX + (PRIME1 * indexY) + (PRIME2 * indexZ), seed );
}


//-----------------------------------------------------------------------------------------------
inline uint32_t Get4dNoiseUint32( int32_t indexX, int32_t indexY, int32_t indexZ, int32_t indexT, uint32_t seed )
{
	const int32_t PRIME1 = 198491317; // Large prime number with non-boring bits
	const int32_t PRIME2 = 6542989; // Large prime number with distinct and non-boring bits
	const int32_t PRIME3 = 357239; // Large prime number with distinct and non-boring bits
	return Get1dNoiseUint32( indexX + (PRIME1 * indexY) + (PRIME2 * indexZ) + (PRIME3 * indexT), seed );
}


//-----------------------------------------------------------------------------------------------
inline float Get1dNoiseZeroToOne( int32_t index, uint32_t seed )
{
	const double ONE_OVER_MAX_UINT = (1.0 / (double) 0xFFFFFFFF);
	return (float)( ONE_OVER_MAX_UINT * (double) Get1dNoiseUint32( index, seed ) );
}


//-----------------------------------------------------------------------------------------------
inline float Get2dNoiseZeroToOne( int32_t indexX, int32_t indexY, uint32_t seed )
{
	const double ONE_OVER_MAX_UINT = (1.0 / (double) 0xFFFFFFFF);
	return (float)( ONE_OVER_MAX_UINT * (double) Get2dNoiseUint32( indexX, indexY, seed ) );
}


//-----------------------------------------------------------------------------------------------
inline float Get3dNoiseZeroToOne( int32_t indexX, int32_t indexY, int32_t indexZ, uint32_t seed )
{
	const double ONE_OVER_MAX_UINT = (1.0 / (double) 0xFFFFFFFF);
	return (float)( ONE_OVER_MAX_UINT * (double) Get3dNoiseUint32( indexX, indexY, indexZ, seed ) );
}


//-----------------------------------------------------------------------------------------------
inline float Get4dNoiseZeroToOne( int32_t indexX, int32_t indexY, int32_t indexZ, int32_t indexT, uint32_t seed )
{
	const double ONE_OVER_MAX_UINT = (1.0 / (double) 0xFFFFFFFF);
	return (float)( ONE_OVER_MAX_UINT * (double) Get4dNoiseUint32( indexX, indexY, indexZ, indexT, seed ) );
}


//-----------------------------------------------------------------------------------------------
inline float Get1dNoiseNegOneToOne( int32_t index, uint32_t seed )
{
	const double ONE_OVER_MAX_INT = (1.0 / (double) 0x7FFFFFFF);
	return (float)( ONE_OVER_MAX_INT * (double) (int32_t) Get1dNoiseUint32( index, seed ) );
}


//-----------------------------------------------------------------------------------------------
inline float Get2dNoiseNegOneToOne( int32_t indexX, int32_t indexY, uint32_t seed )
{
	const double ONE_OVER_MAX_INT = (1.0 / (double) 0x7FFFFFFF);
	return (float)( ONE_OVER_MAX_INT * (double) (int32_t) Get2dNoiseUint32( indexX, indexY, seed ) );
}


//-----------------------------------------------------------------------------------------------
inline float Get3dNoiseNegOneToOne( int32_t indexX, int32_t indexY, int32_t indexZ, uint32_t seed )
{
	const double ONE_OVER_MAX_INT = (1.0 / (double) 0x7FFFFFFF);
	return (float)( ONE_OVER_MAX_INT * (double) (int32_t) Get3dNoiseUint32( indexX, indexY, indexZ, seed ) );
}


//-----------------------------------------------------------------------------------------------
inline float Get4dNoiseNegOneToOne( int32_t indexX, int32_t indexY, int32_t indexZ, int32_t indexT, uint32_t seed )
{
	const double ONE_OVER_MAX_INT = (1.0 / (double) 0x7FFFFFFF);
	return (float)( ONE_OVER_MAX_INT * (double) (int32_t) Get4dNoiseUint32( indexX, indexY, indexZ, indexT, seed ) );
}

} // SquirrelNoise4

