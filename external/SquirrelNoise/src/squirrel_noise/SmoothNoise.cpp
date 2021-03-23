//-----------------------------------------------------------------------------------------------
// SmoothNoise.cpp
//
#include "./RawNoise.hpp"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <glm/common.hpp>
#include <glm/geometric.hpp>


/////////////////////////////////////////////////////////////////////////////////////////////////
// For all fractal (and Perlin) noise functions, the following internal naming conventions
//	are used, primarily to help me visualize 3D and 4D constructs clearly.  They need not
//	have any actual bearing on / relationship to actual external coordinate systems.
//
// 1D noise: only X (+east / -west)
// 2D noise: also Y (+north / -south)
// 3D noise: also Z (+above / -below)
// 4D noise: also T (+after / -before)
/////////////////////////////////////////////////////////////////////////////////////////////////

namespace SquirrelNoise4 {

namespace Easing {
	static float SmoothStep( float t ) {
		//return 2*(t*t*t) - 3*(t*t);
		return t*t * (2*t-3); // optimised
	}
} // Easing


//-----------------------------------------------------------------------------------------------
float Compute1dFractal( float position, float scale, uint32_t numOctaves, float octavePersistence, float octaveScale, bool renormalize, uint32_t seed )
{
	const float OCTAVE_OFFSET = 0.636764989593174f; // Translation/bias to add to each octave

	float totalNoise = 0.f;
	float totalAmplitude = 0.f;
	float currentAmplitude = 1.f;
	float currentPosition = position * (1.f / scale);

	for( uint32_t octaveNum = 0; octaveNum < numOctaves; ++ octaveNum )
	{
		// Determine noise values at nearby integer "grid point" positions
		float positionFloor = ::glm::floor( currentPosition );
		int32_t indexWest = (int32_t) positionFloor;
		int32_t indexEast = indexWest + 1;
		float valueWest = Get1dNoiseZeroToOne( indexWest, seed );
		float valueEast = Get1dNoiseZeroToOne( indexEast, seed );

		// Do a smoothed (nonlinear) weighted average of nearby grid point values
		float distanceFromWest = currentPosition - positionFloor;
		float weightEast = Easing::SmoothStep( distanceFromWest ); // Gives rounder (nonlinear) results
		float weightWest = 1.f - weightEast;
		float noiseZeroToOne = (valueWest * weightWest) + (valueEast * weightEast);
		float noiseThisOctave = 2.f * (noiseZeroToOne - 0.5f); // Map from [0,1] to [-1,1]

		// Accumulate results and prepare for next octave (if any)
		totalNoise += noiseThisOctave * currentAmplitude;
		totalAmplitude += currentAmplitude;
		currentAmplitude *= octavePersistence;
		currentPosition *= octaveScale;
		currentPosition += OCTAVE_OFFSET; // Add "irrational" offset to de-align octave grids
		++ seed; // Eliminates octaves "echoing" each other (since each octave is uniquely seeded)
	}

	// Re-normalize total noise to within [-1,1] and fix octaves pulling us far away from limits
	if( renormalize && totalAmplitude > 0.f )
	{
		totalNoise /= totalAmplitude;				// Amplitude exceeds 1.0 if octaves are used!
		totalNoise = (totalNoise * 0.5f) + 0.5f;	// Map to [0,1]
		totalNoise = Easing::SmoothStep( totalNoise );		// Push towards extents (octaves pull us away)
		totalNoise = (totalNoise * 2.0f) - 1.f;		// Map back to [-1,1]
	}

	return totalNoise;
}


//-----------------------------------------------------------------------------------------------
float Compute2dFractal( float posX, float posY, float scale, uint32_t numOctaves, float octavePersistence, float octaveScale, bool renormalize, uint32_t seed )
{
	const float OCTAVE_OFFSET = 0.636764989593174f; // Translation/bias to add to each octave

	float totalNoise = 0.f;
	float totalAmplitude = 0.f;
	float currentAmplitude = 1.f;
	float invScale = (1.f / scale);
	::glm::vec2 currentPos( posX * invScale, posY * invScale );

	for( uint32_t octaveNum = 0; octaveNum < numOctaves; ++ octaveNum )
	{
		// Determine noise values at nearby integer "grid point" positions
		::glm::vec2 cellMins( ::glm::floor( currentPos.x ), ::glm::floor( currentPos.y ) );
		int32_t indexWestX = (int32_t) cellMins.x;
		int32_t indexSouthY = (int32_t) cellMins.y;
		int32_t indexEastX = indexWestX + 1;
		int32_t indexNorthY = indexSouthY + 1;
		float valueSouthWest = Get2dNoiseZeroToOne( indexWestX, indexSouthY, seed );
		float valueSouthEast = Get2dNoiseZeroToOne( indexEastX, indexSouthY, seed );
		float valueNorthWest = Get2dNoiseZeroToOne( indexWestX, indexNorthY, seed );
		float valueNorthEast = Get2dNoiseZeroToOne( indexEastX, indexNorthY, seed );

		// Do a smoothed (nonlinear) weighted average of nearby grid point values
		::glm::vec2 displacementFromMins = currentPos - cellMins;
		float weightEast  = Easing::SmoothStep( displacementFromMins.x );
		float weightNorth = Easing::SmoothStep( displacementFromMins.y );
		float weightWest  = 1.f - weightEast;
		float weightSouth = 1.f - weightNorth;

		float blendSouth = (weightEast * valueSouthEast) + (weightWest * valueSouthWest);
		float blendNorth = (weightEast * valueNorthEast) + (weightWest * valueNorthWest);
		float blendTotal = (weightSouth * blendSouth) + (weightNorth * blendNorth);
		float noiseThisOctave = 2.f * (blendTotal - 0.5f); // Map from [0,1] to [-1,1]

		// Accumulate results and prepare for next octave (if any)
		totalNoise += noiseThisOctave * currentAmplitude;
		totalAmplitude += currentAmplitude;
		currentAmplitude *= octavePersistence;
		currentPos *= octaveScale;
		currentPos.x += OCTAVE_OFFSET; // Add "irrational" offsets to noise position components
		currentPos.y += OCTAVE_OFFSET; //	at each octave to break up their grid alignment
		++ seed; // Eliminates octaves "echoing" each other (since each octave is uniquely seeded)
	}

	// Re-normalize total noise to within [-1,1] and fix octaves pulling us far away from limits
	if( renormalize && totalAmplitude > 0.f )
	{
		totalNoise /= totalAmplitude;				// Amplitude exceeds 1.0 if octaves are used
		totalNoise = (totalNoise * 0.5f) + 0.5f;	// Map to [0,1]
		totalNoise = Easing::SmoothStep( totalNoise );		// Push towards extents (octaves pull us away)
		totalNoise = (totalNoise * 2.0f) - 1.f;		// Map back to [-1,1]
	}

	return totalNoise;
}


//-----------------------------------------------------------------------------------------------
float Compute3dFractal( float posX, float posY, float posZ, float scale, uint32_t numOctaves, float octavePersistence, float octaveScale, bool renormalize, uint32_t seed )
{
	const float OCTAVE_OFFSET = 0.636764989593174f; // Translation/bias to add to each octave

	float totalNoise = 0.f;
	float totalAmplitude = 0.f;
	float currentAmplitude = 1.f;
	float invScale = (1.f / scale);
	::glm::vec3 currentPos( posX * invScale, posY * invScale, posZ * invScale );

	for( uint32_t octaveNum = 0; octaveNum < numOctaves; ++ octaveNum )
	{
		// Determine noise values at nearby integer "grid point" positions
		::glm::vec3 cellMins( ::glm::floor( currentPos.x ), ::glm::floor( currentPos.y ), ::glm::floor( currentPos.z ) );
		int32_t indexWestX  = (int32_t) cellMins.x;
		int32_t indexSouthY = (int32_t) cellMins.y;
		int32_t indexBelowZ = (int32_t) cellMins.z;
		int32_t indexEastX  = indexWestX + 1;
		int32_t indexNorthY = indexSouthY + 1;
		int32_t indexAboveZ = indexBelowZ + 1;

		// Noise grid cell has 8 corners in 3D
		float aboveSouthWest = Get3dNoiseZeroToOne( indexWestX, indexSouthY, indexAboveZ, seed );
		float aboveSouthEast = Get3dNoiseZeroToOne( indexEastX, indexSouthY, indexAboveZ, seed );
		float aboveNorthWest = Get3dNoiseZeroToOne( indexWestX, indexNorthY, indexAboveZ, seed );
		float aboveNorthEast = Get3dNoiseZeroToOne( indexEastX, indexNorthY, indexAboveZ, seed );
		float belowSouthWest = Get3dNoiseZeroToOne( indexWestX, indexSouthY, indexBelowZ, seed );
		float belowSouthEast = Get3dNoiseZeroToOne( indexEastX, indexSouthY, indexBelowZ, seed );
		float belowNorthWest = Get3dNoiseZeroToOne( indexWestX, indexNorthY, indexBelowZ, seed );
		float belowNorthEast = Get3dNoiseZeroToOne( indexEastX, indexNorthY, indexBelowZ, seed );

		// Do a smoothed (nonlinear) weighted average of nearby grid point values
		::glm::vec3 displacementFromMins = currentPos - cellMins;

		float weightEast  = Easing::SmoothStep( displacementFromMins.x );
		float weightNorth = Easing::SmoothStep( displacementFromMins.y );
		float weightAbove = Easing::SmoothStep( displacementFromMins.z );
		float weightWest  = 1.f - weightEast;
		float weightSouth = 1.f - weightNorth;
		float weightBelow = 1.f - weightAbove;

		// 8-way blend (8 -> 4 -> 2 -> 1)
		float blendBelowSouth = (weightEast * belowSouthEast) + (weightWest * belowSouthWest);
		float blendBelowNorth = (weightEast * belowNorthEast) + (weightWest * belowNorthWest);
		float blendAboveSouth = (weightEast * aboveSouthEast) + (weightWest * aboveSouthWest);
		float blendAboveNorth = (weightEast * aboveNorthEast) + (weightWest * aboveNorthWest);
		float blendBelow = (weightSouth * blendBelowSouth) + (weightNorth * blendBelowNorth);
		float blendAbove = (weightSouth * blendAboveSouth) + (weightNorth * blendAboveNorth);
		float blendTotal = (weightBelow * blendBelow) + (weightAbove * blendAbove);
		float noiseThisOctave = 2.f * (blendTotal - 0.5f); // Map from [0,1] to [-1,1]

		// Accumulate results and prepare for next octave (if any)
		totalNoise += noiseThisOctave * currentAmplitude;
		totalAmplitude += currentAmplitude;
		currentAmplitude *= octavePersistence;
		currentPos *= octaveScale;
		currentPos.x += OCTAVE_OFFSET; // Add "irrational" offsets to noise position components
		currentPos.y += OCTAVE_OFFSET; //	at each octave to break up their grid alignment
		currentPos.z += OCTAVE_OFFSET;
		++ seed; // Eliminates octaves "echoing" each other (since each octave is uniquely seeded)
	}

	// Re-normalize total noise to within [-1,1] and fix octaves pulling us far away from limits
	if( renormalize && totalAmplitude > 0.f )
	{
		totalNoise /= totalAmplitude;				// Amplitude exceeds 1.0 if octaves are used
		totalNoise = (totalNoise * 0.5f) + 0.5f;	// Map to [0,1]
		totalNoise = Easing::SmoothStep( totalNoise );		// Push towards extents (octaves pull us away)
		totalNoise = (totalNoise * 2.0f) - 1.f;		// Map back to [-1,1]
	}

	return totalNoise;
}


//-----------------------------------------------------------------------------------------------
float Compute4dFractal( float posX, float posY, float posZ, float posT, float scale, uint32_t numOctaves, float octavePersistence, float octaveScale, bool renormalize, uint32_t seed )
{
	const float OCTAVE_OFFSET = 0.636764989593174f; // Translation/bias to add to each octave

	float totalNoise = 0.f;
	float totalAmplitude = 0.f;
	float currentAmplitude = 1.f;
	float invScale = (1.f / scale);
	::glm::vec4 currentPos( posX * invScale, posY * invScale, posZ * invScale, posT * invScale );

	for( uint32_t octaveNum = 0; octaveNum < numOctaves; ++ octaveNum )
	{
		// Determine noise values at nearby integer "grid point" positions
		::glm::vec4 cellMins( ::glm::floor( currentPos.x ), ::glm::floor( currentPos.y ), ::glm::floor( currentPos.z ), ::glm::floor( currentPos.w ) );
		int32_t indexWestX   = (int32_t) cellMins.x;
		int32_t indexSouthY  = (int32_t) cellMins.y;
		int32_t indexBelowZ  = (int32_t) cellMins.z;
		int32_t indexBeforeT = (int32_t) cellMins.w;
		int32_t indexEastX  = indexWestX + 1;
		int32_t indexNorthY = indexSouthY + 1;
		int32_t indexAboveZ = indexBelowZ + 1;
		int32_t indexAfterT = indexBeforeT + 1;

		// Noise grid cell has 16 "corners" in 4D
		float beforeBelowSW = Get4dNoiseZeroToOne( indexWestX, indexSouthY, indexBelowZ, indexBeforeT, seed );
		float beforeBelowSE = Get4dNoiseZeroToOne( indexEastX, indexSouthY, indexBelowZ, indexBeforeT, seed );
		float beforeBelowNW = Get4dNoiseZeroToOne( indexWestX, indexNorthY, indexBelowZ, indexBeforeT, seed );
		float beforeBelowNE = Get4dNoiseZeroToOne( indexEastX, indexNorthY, indexBelowZ, indexBeforeT, seed );
		float beforeAboveSW = Get4dNoiseZeroToOne( indexWestX, indexSouthY, indexAboveZ, indexBeforeT, seed );
		float beforeAboveSE = Get4dNoiseZeroToOne( indexEastX, indexSouthY, indexAboveZ, indexBeforeT, seed );
		float beforeAboveNW = Get4dNoiseZeroToOne( indexWestX, indexNorthY, indexAboveZ, indexBeforeT, seed );
		float beforeAboveNE = Get4dNoiseZeroToOne( indexEastX, indexNorthY, indexAboveZ, indexBeforeT, seed );

		float afterBelowSW = Get4dNoiseZeroToOne( indexWestX, indexSouthY, indexBelowZ, indexAfterT, seed );
		float afterBelowSE = Get4dNoiseZeroToOne( indexEastX, indexSouthY, indexBelowZ, indexAfterT, seed );
		float afterBelowNW = Get4dNoiseZeroToOne( indexWestX, indexNorthY, indexBelowZ, indexAfterT, seed );
		float afterBelowNE = Get4dNoiseZeroToOne( indexEastX, indexNorthY, indexBelowZ, indexAfterT, seed );
		float afterAboveSW = Get4dNoiseZeroToOne( indexWestX, indexSouthY, indexAboveZ, indexAfterT, seed );
		float afterAboveSE = Get4dNoiseZeroToOne( indexEastX, indexSouthY, indexAboveZ, indexAfterT, seed );
		float afterAboveNW = Get4dNoiseZeroToOne( indexWestX, indexNorthY, indexAboveZ, indexAfterT, seed );
		float afterAboveNE = Get4dNoiseZeroToOne( indexEastX, indexNorthY, indexAboveZ, indexAfterT, seed );

		// Do a smoothed (nonlinear) weighted average of nearby grid point values
		::glm::vec4 displacementFromMins = currentPos - cellMins;

		float weightEast	= Easing::SmoothStep( displacementFromMins.x );
		float weightNorth	= Easing::SmoothStep( displacementFromMins.y );
		float weightAbove	= Easing::SmoothStep( displacementFromMins.z );
		float weightAfter	= Easing::SmoothStep( displacementFromMins.w );
		float weightWest	= 1.f - weightEast;
		float weightSouth	= 1.f - weightNorth;
		float weightBelow	= 1.f - weightAbove;
		float weightBefore	= 1.f - weightAfter;

		// 16-way blend (16 -> 8 -> 4 -> 2 -> 1)
		float blendBeforeBelowSouth	= (weightEast * beforeBelowSE) + (weightWest * beforeBelowSW);
		float blendBeforeBelowNorth	= (weightEast * beforeBelowNE) + (weightWest * beforeBelowNW);
		float blendBeforeAboveSouth	= (weightEast * beforeAboveSE) + (weightWest * beforeAboveSW);
		float blendBeforeAboveNorth	= (weightEast * beforeAboveNE) + (weightWest * beforeAboveNW);
		float blendAfterBelowSouth  = (weightEast * afterBelowSE)  + (weightWest * afterBelowSW);
		float blendAfterBelowNorth  = (weightEast * afterBelowNE)  + (weightWest * afterBelowNW);
		float blendAfterAboveSouth  = (weightEast * afterAboveSE)  + (weightWest * afterAboveSW);
		float blendAfterAboveNorth  = (weightEast * afterAboveNE)  + (weightWest * afterAboveNW);
		float blendBeforeBelow = (weightSouth * blendBeforeBelowSouth) + (weightNorth * blendBeforeBelowNorth);
		float blendBeforeAbove = (weightSouth * blendBeforeAboveSouth) + (weightNorth * blendBeforeAboveNorth);
		float blendAfterBelow  = (weightSouth * blendAfterBelowSouth)  + (weightNorth * blendAfterBelowNorth);
		float blendAfterAbove  = (weightSouth * blendAfterAboveSouth)  + (weightNorth * blendAfterAboveNorth);
		float blendBefore = (weightBelow * blendBeforeBelow) + (weightAbove * blendBeforeAbove);
		float blendAfter  = (weightBelow * blendAfterBelow) + (weightAbove * blendAfterAbove);
		float blendTotal = (weightBefore * blendBefore) + (weightAfter * blendAfter);
		float noiseThisOctave = 2.f * (blendTotal - 0.5f); // Map from [0,1] to [-1,1]

		// Accumulate results and prepare for next octave (if any)
		totalNoise += noiseThisOctave * currentAmplitude;
		totalAmplitude += currentAmplitude;
		currentAmplitude *= octavePersistence;
		currentPos *= octaveScale;
		currentPos.x += OCTAVE_OFFSET; // Add "irrational" offsets to noise position components
		currentPos.y += OCTAVE_OFFSET; //	at each octave to break up their grid alignment
		currentPos.z += OCTAVE_OFFSET;
		currentPos.w += OCTAVE_OFFSET;
		++ seed; // Eliminates octaves "echoing" each other (since each octave is uniquely seeded)
	}

	// Re-normalize total noise to within [-1,1] and fix octaves pulling us far away from limits
	if( renormalize && totalAmplitude > 0.f )
	{
		totalNoise /= totalAmplitude;				// Amplitude exceeds 1.0 if octaves are used
		totalNoise = (totalNoise * 0.5f) + 0.5f;	// Map to [0,1]
		totalNoise = Easing::SmoothStep( totalNoise );		// Push towards extents (octaves pull us away)
		totalNoise = (totalNoise * 2.0f) - 1.f;		// Map back to [-1,1]
	}

	return totalNoise;
}


//-----------------------------------------------------------------------------------------------
// Perlin noise is fractal noise with "gradient vector smoothing" applied.
//
// In 1D, the gradients are trivial: -1.0 or 1.0, so resulting noise is boring at one octave.
//
float Compute1dPerlin( float position, float scale, uint32_t numOctaves, float octavePersistence, float octaveScale, bool renormalize, uint32_t seed )
{
	const float OCTAVE_OFFSET = 0.636764989593174f; // Translation/bias to add to each octave
	const float gradients[2] = { -1.f, 1.f }; // 1D unit "gradient" vectors; one back, one forward

	float totalNoise = 0.f;
	float totalAmplitude = 0.f;
	float currentAmplitude = 1.f;
	float currentPosition = position * (1.f / scale);

	for( uint32_t octaveNum = 0; octaveNum < numOctaves; ++ octaveNum )
	{
		// Determine random "gradient vectors" (just +1 or -1 for 1D Perlin) for surrounding corners
		float positionFloor = (float) ::glm::floor( currentPosition );
		int32_t indexWest = (int32_t) positionFloor;
		int32_t indexEast = indexWest + 1;
		float gradientWest = gradients[ Get1dNoiseUint32( indexWest, seed ) & 0x00000001 ];
		float gradientEast = gradients[ Get1dNoiseUint32( indexEast, seed ) & 0x00000001 ];

		// Dot each point's gradient with displacement from point to position
		float displacementFromWest = currentPosition - positionFloor; // always positive
		float displacementFromEast = displacementFromWest - 1.f; // always negative
		float dotWest = gradientWest * displacementFromWest; // 1D "dot product" is... multiply
		float dotEast = gradientEast * displacementFromEast;

		// Do a smoothed (nonlinear) weighted average of dot results
		float weightEast = Easing::SmoothStep( displacementFromWest );
		float weightWest = 1.f - weightEast;
		float blendTotal = (weightWest * dotWest) + (weightEast * dotEast);
		float noiseThisOctave = 2.f * blendTotal; // 1D Perlin is in [-.5,.5]; map to [-1,1]

		// Accumulate results and prepare for next octave (if any)
		totalNoise += noiseThisOctave * currentAmplitude;
		totalAmplitude += currentAmplitude;
		currentAmplitude *= octavePersistence;
		currentPosition *= octaveScale;
		currentPosition += OCTAVE_OFFSET; // Add "irrational" offset to de-align octave grids
		++ seed; // Eliminates octaves "echoing" each other (since each octave is uniquely seeded)
	}

	// Re-normalize total noise to within [-1,1] and fix octaves pulling us far away from limits
	if( renormalize && totalAmplitude > 0.f )
	{
		totalNoise /= totalAmplitude;				// Amplitude exceeds 1.0 if octaves are used
		totalNoise = (totalNoise * 0.5f) + 0.5f;	// Map to [0,1]
		totalNoise = Easing::SmoothStep( totalNoise );		// Push towards extents (octaves pull us away)
		totalNoise = (totalNoise * 2.0f) - 1.f;		// Map back to [-1,1]
	}

	return totalNoise;
}


//-----------------------------------------------------------------------------------------------
// Perlin noise is fractal noise with "gradient vector smoothing" applied.
//
// In 2D, gradients are unit-length vectors in various directions with even angular distribution.
//
float Compute2dPerlin( float posX, float posY, float scale, uint32_t numOctaves, float octavePersistence, float octaveScale, bool renormalize, uint32_t seed )
{
	const float OCTAVE_OFFSET = 0.636764989593174f; // Translation/bias to add to each octave
	const ::glm::vec2 gradients[ 8 ] = // Normalized unit vectors in 8 quarter-cardinal directions
	{
		::glm::vec2( +0.923879533f, +0.382683432f ), //  22.5 degrees (ENE)
		::glm::vec2( +0.382683432f, +0.923879533f ), //  67.5 degrees (NNE)
		::glm::vec2( -0.382683432f, +0.923879533f ), // 112.5 degrees (NNW)
		::glm::vec2( -0.923879533f, +0.382683432f ), // 157.5 degrees (WNW)
		::glm::vec2( -0.923879533f, -0.382683432f ), // 202.5 degrees (WSW)
		::glm::vec2( -0.382683432f, -0.923879533f ), // 247.5 degrees (SSW)
		::glm::vec2( +0.382683432f, -0.923879533f ), // 292.5 degrees (SSE)
		::glm::vec2( +0.923879533f, -0.382683432f )	 // 337.5 degrees (ESE)
	};

	float totalNoise = 0.f;
	float totalAmplitude = 0.f;
	float currentAmplitude = 1.f;
	float invScale = (1.f / scale);
	::glm::vec2 currentPos( posX * invScale, posY * invScale );

	for( uint32_t octaveNum = 0; octaveNum < numOctaves; ++ octaveNum )
	{
		// Determine random unit "gradient vectors" for surrounding corners
		::glm::vec2 cellMins( ::glm::floor( currentPos.x ), ::glm::floor( currentPos.y ) );
		::glm::vec2 cellMaxs( cellMins.x + 1.f, cellMins.y + 1.f );
		int32_t indexWestX  = (int32_t) cellMins.x;
		int32_t indexSouthY = (int32_t) cellMins.y;
		int32_t indexEastX  = indexWestX  + 1;
		int32_t indexNorthY = indexSouthY + 1;

		uint32_t noiseSW = Get2dNoiseUint32( indexWestX, indexSouthY, seed );
		uint32_t noiseSE = Get2dNoiseUint32( indexEastX, indexSouthY, seed );
		uint32_t noiseNW = Get2dNoiseUint32( indexWestX, indexNorthY, seed );
		uint32_t noiseNE = Get2dNoiseUint32( indexEastX, indexNorthY, seed );

		const ::glm::vec2& gradientSW = gradients[ noiseSW & 0x00000007 ];
		const ::glm::vec2& gradientSE = gradients[ noiseSE & 0x00000007 ];
		const ::glm::vec2& gradientNW = gradients[ noiseNW & 0x00000007 ];
		const ::glm::vec2& gradientNE = gradients[ noiseNE & 0x00000007 ];

		// Dot each corner's gradient with displacement from corner to position
		::glm::vec2 displacementFromSW( currentPos.x - cellMins.x, currentPos.y - cellMins.y );
		::glm::vec2 displacementFromSE( currentPos.x - cellMaxs.x, currentPos.y - cellMins.y );
		::glm::vec2 displacementFromNW( currentPos.x - cellMins.x, currentPos.y - cellMaxs.y );
		::glm::vec2 displacementFromNE( currentPos.x - cellMaxs.x, currentPos.y - cellMaxs.y );

		float dotSouthWest = ::glm::dot( gradientSW, displacementFromSW );
		float dotSouthEast = ::glm::dot( gradientSE, displacementFromSE );
		float dotNorthWest = ::glm::dot( gradientNW, displacementFromNW );
		float dotNorthEast = ::glm::dot( gradientNE, displacementFromNE );

		// Do a smoothed (nonlinear) weighted average of dot results
		float weightEast = Easing::SmoothStep( displacementFromSW.x );
		float weightNorth = Easing::SmoothStep( displacementFromSW.y );
		float weightWest = 1.f - weightEast;
		float weightSouth = 1.f - weightNorth;

		float blendSouth = (weightEast * dotSouthEast) + (weightWest * dotSouthWest);
		float blendNorth = (weightEast * dotNorthEast) + (weightWest * dotNorthWest);
		float blendTotal = (weightSouth * blendSouth) + (weightNorth * blendNorth);
		float noiseThisOctave = blendTotal * (1.f / 0.662578106f); // 2D Perlin is in [-.662578106,.662578106]; map to ~[-1,1]

		// Accumulate results and prepare for next octave (if any)
		totalNoise += noiseThisOctave * currentAmplitude;
		totalAmplitude += currentAmplitude;
		currentAmplitude *= octavePersistence;
		currentPos *= octaveScale;
		currentPos.x += OCTAVE_OFFSET; // Add "irrational" offset to de-align octave grids
		currentPos.y += OCTAVE_OFFSET; // Add "irrational" offset to de-align octave grids
		++ seed; // Eliminates octaves "echoing" each other (since each octave is uniquely seeded)
	}

	// Re-normalize total noise to within [-1,1] and fix octaves pulling us far away from limits
	if( renormalize && totalAmplitude > 0.f )
	{
		totalNoise /= totalAmplitude;				// Amplitude exceeds 1.0 if octaves are used
		totalNoise = (totalNoise * 0.5f) + 0.5f;	// Map to [0,1]
		totalNoise = Easing::SmoothStep( totalNoise );		// Push towards extents (octaves pull us away)
		totalNoise = (totalNoise * 2.0f) - 1.f;		// Map back to [-1,1]
	}

	return totalNoise;
}


//-----------------------------------------------------------------------------------------------
// Perlin noise is fractal noise with "gradient vector smoothing" applied.
//
// In 3D, gradients are unit-length vectors in random (3D) directions.
//
float Compute3dPerlin( float posX, float posY, float posZ, float scale, uint32_t numOctaves, float octavePersistence, float octaveScale, bool renormalize, uint32_t seed )
{
	const float OCTAVE_OFFSET = 0.636764989593174f; // Translation/bias to add to each octave
	const float fSQRT_3_OVER_3 = 0.577350269189f;
	const ::glm::vec3 gradients[ 8 ] = // Traditional "12 edges" requires modulus and isn't any better.
	{
		::glm::vec3( +fSQRT_3_OVER_3, +fSQRT_3_OVER_3, +fSQRT_3_OVER_3 ), // Normalized unit 3D vectors
		::glm::vec3( -fSQRT_3_OVER_3, +fSQRT_3_OVER_3, +fSQRT_3_OVER_3 ), //  pointing toward cube
		::glm::vec3( +fSQRT_3_OVER_3, -fSQRT_3_OVER_3, +fSQRT_3_OVER_3 ), //  corners, so components
		::glm::vec3( -fSQRT_3_OVER_3, -fSQRT_3_OVER_3, +fSQRT_3_OVER_3 ), //  are all sqrt(3)/3, i.e.
		::glm::vec3( +fSQRT_3_OVER_3, +fSQRT_3_OVER_3, -fSQRT_3_OVER_3 ), // 0.5773502691896257645091f.
		::glm::vec3( -fSQRT_3_OVER_3, +fSQRT_3_OVER_3, -fSQRT_3_OVER_3 ), // These are slightly better
		::glm::vec3( +fSQRT_3_OVER_3, -fSQRT_3_OVER_3, -fSQRT_3_OVER_3 ), // than axes (1,0,0) and much
		::glm::vec3( -fSQRT_3_OVER_3, -fSQRT_3_OVER_3, -fSQRT_3_OVER_3 )  // faster than edges (1,1,0).
	};

	float totalNoise = 0.f;
	float totalAmplitude = 0.f;
	float currentAmplitude = 1.f;
	float invScale = (1.f / scale);
	::glm::vec3 currentPos( posX * invScale, posY * invScale, posZ * invScale );

	for( uint32_t octaveNum = 0; octaveNum < numOctaves; ++ octaveNum )
	{
		// Determine random unit "gradient vectors" for surrounding corners
		::glm::vec3 cellMins( ::glm::floor( currentPos.x ), ::glm::floor( currentPos.y ), ::glm::floor( currentPos.z ) );
		::glm::vec3 cellMaxs( cellMins.x + 1.f, cellMins.y + 1.f, cellMins.z + 1.f );
		int32_t indexWestX  = (int32_t) cellMins.x;
		int32_t indexSouthY = (int32_t) cellMins.y;
		int32_t indexBelowZ = (int32_t) cellMins.z;
		int32_t indexEastX  = indexWestX  + 1;
		int32_t indexNorthY = indexSouthY + 1;
		int32_t indexAboveZ = indexBelowZ + 1;

		uint32_t noiseBelowSW = Get3dNoiseUint32( indexWestX, indexSouthY, indexBelowZ, seed );
		uint32_t noiseBelowSE = Get3dNoiseUint32( indexEastX, indexSouthY, indexBelowZ, seed );
		uint32_t noiseBelowNW = Get3dNoiseUint32( indexWestX, indexNorthY, indexBelowZ, seed );
		uint32_t noiseBelowNE = Get3dNoiseUint32( indexEastX, indexNorthY, indexBelowZ, seed );
		uint32_t noiseAboveSW = Get3dNoiseUint32( indexWestX, indexSouthY, indexAboveZ, seed );
		uint32_t noiseAboveSE = Get3dNoiseUint32( indexEastX, indexSouthY, indexAboveZ, seed );
		uint32_t noiseAboveNW = Get3dNoiseUint32( indexWestX, indexNorthY, indexAboveZ, seed );
		uint32_t noiseAboveNE = Get3dNoiseUint32( indexEastX, indexNorthY, indexAboveZ, seed );

		::glm::vec3 gradientBelowSW = gradients[ noiseBelowSW & 0x00000007 ];
		::glm::vec3 gradientBelowSE = gradients[ noiseBelowSE & 0x00000007 ];
		::glm::vec3 gradientBelowNW = gradients[ noiseBelowNW & 0x00000007 ];
		::glm::vec3 gradientBelowNE = gradients[ noiseBelowNE & 0x00000007 ];
		::glm::vec3 gradientAboveSW = gradients[ noiseAboveSW & 0x00000007 ];
		::glm::vec3 gradientAboveSE = gradients[ noiseAboveSE & 0x00000007 ];
		::glm::vec3 gradientAboveNW = gradients[ noiseAboveNW & 0x00000007 ];
		::glm::vec3 gradientAboveNE = gradients[ noiseAboveNE & 0x00000007 ];

		// Dot each corner's gradient with displacement from corner to position
		::glm::vec3 displacementFromBelowSW( currentPos.x - cellMins.x, currentPos.y - cellMins.y, currentPos.z - cellMins.z );
		::glm::vec3 displacementFromBelowSE( currentPos.x - cellMaxs.x, currentPos.y - cellMins.y, currentPos.z - cellMins.z );
		::glm::vec3 displacementFromBelowNW( currentPos.x - cellMins.x, currentPos.y - cellMaxs.y, currentPos.z - cellMins.z );
		::glm::vec3 displacementFromBelowNE( currentPos.x - cellMaxs.x, currentPos.y - cellMaxs.y, currentPos.z - cellMins.z );
		::glm::vec3 displacementFromAboveSW( currentPos.x - cellMins.x, currentPos.y - cellMins.y, currentPos.z - cellMaxs.z );
		::glm::vec3 displacementFromAboveSE( currentPos.x - cellMaxs.x, currentPos.y - cellMins.y, currentPos.z - cellMaxs.z );
		::glm::vec3 displacementFromAboveNW( currentPos.x - cellMins.x, currentPos.y - cellMaxs.y, currentPos.z - cellMaxs.z );
		::glm::vec3 displacementFromAboveNE( currentPos.x - cellMaxs.x, currentPos.y - cellMaxs.y, currentPos.z - cellMaxs.z );

		float dotBelowSW = ::glm::dot( gradientBelowSW, displacementFromBelowSW );
		float dotBelowSE = ::glm::dot( gradientBelowSE, displacementFromBelowSE );
		float dotBelowNW = ::glm::dot( gradientBelowNW, displacementFromBelowNW );
		float dotBelowNE = ::glm::dot( gradientBelowNE, displacementFromBelowNE );
		float dotAboveSW = ::glm::dot( gradientAboveSW, displacementFromAboveSW );
		float dotAboveSE = ::glm::dot( gradientAboveSE, displacementFromAboveSE );
		float dotAboveNW = ::glm::dot( gradientAboveNW, displacementFromAboveNW );
		float dotAboveNE = ::glm::dot( gradientAboveNE, displacementFromAboveNE );

		// Do a smoothed (nonlinear) weighted average of dot results
		float weightEast  = Easing::SmoothStep( displacementFromBelowSW.x );
		float weightNorth = Easing::SmoothStep( displacementFromBelowSW.y );
		float weightAbove = Easing::SmoothStep( displacementFromBelowSW.z );
		float weightWest  = 1.f - weightEast;
		float weightSouth = 1.f - weightNorth;
		float weightBelow = 1.f - weightAbove;

		// 8-way blend (8 -> 4 -> 2 -> 1)
		float blendBelowSouth = (weightEast * dotBelowSE) + (weightWest * dotBelowSW);
		float blendBelowNorth = (weightEast * dotBelowNE) + (weightWest * dotBelowNW);
		float blendAboveSouth = (weightEast * dotAboveSE) + (weightWest * dotAboveSW);
		float blendAboveNorth = (weightEast * dotAboveNE) + (weightWest * dotAboveNW);
		float blendBelow = (weightSouth * blendBelowSouth) + (weightNorth * blendBelowNorth);
		float blendAbove = (weightSouth * blendAboveSouth) + (weightNorth * blendAboveNorth);
		float blendTotal = (weightBelow * blendBelow) + (weightAbove * blendAbove);
		float noiseThisOctave = blendTotal * (1.f / 0.793856621f); // 3D Perlin is in [-.793856621,.793856621]; map to ~[-1,1]

		// Accumulate results and prepare for next octave (if any)
		totalNoise += noiseThisOctave * currentAmplitude;
		totalAmplitude += currentAmplitude;
		currentAmplitude *= octavePersistence;
		currentPos *= octaveScale;
		currentPos.x += OCTAVE_OFFSET; // Add "irrational" offset to de-align octave grids
		currentPos.y += OCTAVE_OFFSET; // Add "irrational" offset to de-align octave grids
		currentPos.z += OCTAVE_OFFSET; // Add "irrational" offset to de-align octave grids
		++ seed; // Eliminates octaves "echoing" each other (since each octave is uniquely seeded)
	}

	// Re-normalize total noise to within [-1,1] and fix octaves pulling us far away from limits
	if( renormalize && totalAmplitude > 0.f )
	{
		totalNoise /= totalAmplitude;				// Amplitude exceeds 1.0 if octaves are used
		totalNoise = (totalNoise * 0.5f) + 0.5f;	// Map to [0,1]
		totalNoise = Easing::SmoothStep( totalNoise );		// Push towards extents (octaves pull us away)
		totalNoise = (totalNoise * 2.0f) - 1.f;		// Map back to [-1,1]
	}

	return totalNoise;
}


//-----------------------------------------------------------------------------------------------
// Perlin noise is fractal noise with "gradient vector smoothing" applied.
//
// In 4D, gradients are unit-length hyper-vectors in random (4D) directions.
//
float Compute4dPerlin( float posX, float posY, float posZ, float posT, float scale, uint32_t numOctaves, float octavePersistence, float octaveScale, bool renormalize, uint32_t seed )
{
	const float OCTAVE_OFFSET = 0.636764989593174f; // Translation/bias to add to each octave

	const ::glm::vec4 gradients[ 16 ] = // Hard to tell if this is any better in 4D than just having 8
	{
		::glm::vec4( +0.5f, +0.5f, +0.5f, +0.5f ), // Normalized unit 4D vectors pointing toward each
		::glm::vec4( -0.5f, +0.5f, +0.5f, +0.5f ), //  of the 16 hypercube corners, so components are
		::glm::vec4( +0.5f, -0.5f, +0.5f, +0.5f ), //  all sqrt(4)/4, i.e. one-half.
		::glm::vec4( -0.5f, -0.5f, +0.5f, +0.5f ), //
		::glm::vec4( +0.5f, +0.5f, -0.5f, +0.5f ), // It's hard to tell whether these are any better
		::glm::vec4( -0.5f, +0.5f, -0.5f, +0.5f ), //  or worse than vectors facing axes (1,0,0,0) or
		::glm::vec4( +0.5f, -0.5f, -0.5f, +0.5f ), //  3D edges (.7,.7,0,0) or 4D edges (.57,.57,.57,0)
		::glm::vec4( -0.5f, -0.5f, -0.5f, +0.5f ), //  but less-axial gradients looked a little better
		::glm::vec4( +0.5f, +0.5f, +0.5f, -0.5f ), //  with 2D and 3D noise so I'm assuming this is as
		::glm::vec4( -0.5f, +0.5f, +0.5f, -0.5f ), //  good or better as any other gradient-selection
		::glm::vec4( +0.5f, -0.5f, +0.5f, -0.5f ), //  scheme (and is crazy-fast).  *shrug*
		::glm::vec4( -0.5f, -0.5f, +0.5f, -0.5f ), //
		::glm::vec4( +0.5f, +0.5f, -0.5f, -0.5f ), // Plus, we want a power-of-two number of evenly-
		::glm::vec4( -0.5f, +0.5f, -0.5f, -0.5f ), //  distributed gradients, so we can cheaply select
		::glm::vec4( +0.5f, -0.5f, -0.5f, -0.5f ), //  one from bit-noise (use bit-mask, not modulus).
		::glm::vec4( -0.5f, -0.5f, -0.5f, -0.5f )  //
	};

	float totalNoise = 0.f;
	float totalAmplitude = 0.f;
	float currentAmplitude = 1.f;
	float invScale = (1.f / scale);
	::glm::vec4 currentPos( posX * invScale, posY * invScale, posZ * invScale, posT * invScale );

	for( uint32_t octaveNum = 0; octaveNum < numOctaves; ++ octaveNum )
	{
		// Determine random unit "gradient vectors" for 16 surrounding 4D (hypercube) cell corners
		::glm::vec4 cellMins( ::glm::floor( currentPos.x ), ::glm::floor( currentPos.y ), ::glm::floor( currentPos.z ), ::glm::floor( currentPos.w ) );
		::glm::vec4 cellMaxs( cellMins.x + 1.f, cellMins.y + 1.f, cellMins.z + 1.f, cellMins.w + 1.f );
		int32_t indexWestX   = (int32_t) cellMins.x;
		int32_t indexSouthY  = (int32_t) cellMins.y;
		int32_t indexBelowZ  = (int32_t) cellMins.z;
		int32_t indexBeforeT = (int32_t) cellMins.w;
		int32_t indexEastX  = indexWestX   + 1;
		int32_t indexNorthY = indexSouthY  + 1;
		int32_t indexAboveZ = indexBelowZ  + 1;
		int32_t indexAfterT = indexBeforeT + 1;

		// "BeforeBSW" stands for "BeforeBelowSouthWest" below (i.e. 4D hypercube mins), etc.
		uint32_t noiseBeforeBSW = Get4dNoiseUint32( indexWestX, indexSouthY, indexBelowZ, indexBeforeT, seed );
		uint32_t noiseBeforeBSE = Get4dNoiseUint32( indexEastX, indexSouthY, indexBelowZ, indexBeforeT, seed );
		uint32_t noiseBeforeBNW = Get4dNoiseUint32( indexWestX, indexNorthY, indexBelowZ, indexBeforeT, seed );
		uint32_t noiseBeforeBNE = Get4dNoiseUint32( indexEastX, indexNorthY, indexBelowZ, indexBeforeT, seed );
		uint32_t noiseBeforeASW = Get4dNoiseUint32( indexWestX, indexSouthY, indexAboveZ, indexBeforeT, seed );
		uint32_t noiseBeforeASE = Get4dNoiseUint32( indexEastX, indexSouthY, indexAboveZ, indexBeforeT, seed );
		uint32_t noiseBeforeANW = Get4dNoiseUint32( indexWestX, indexNorthY, indexAboveZ, indexBeforeT, seed );
		uint32_t noiseBeforeANE = Get4dNoiseUint32( indexEastX, indexNorthY, indexAboveZ, indexBeforeT, seed );
		uint32_t noiseAfterBSW = Get4dNoiseUint32( indexWestX, indexSouthY, indexBelowZ, indexAfterT, seed );
		uint32_t noiseAfterBSE = Get4dNoiseUint32( indexEastX, indexSouthY, indexBelowZ, indexAfterT, seed );
		uint32_t noiseAfterBNW = Get4dNoiseUint32( indexWestX, indexNorthY, indexBelowZ, indexAfterT, seed );
		uint32_t noiseAfterBNE = Get4dNoiseUint32( indexEastX, indexNorthY, indexBelowZ, indexAfterT, seed );
		uint32_t noiseAfterASW = Get4dNoiseUint32( indexWestX, indexSouthY, indexAboveZ, indexAfterT, seed );
		uint32_t noiseAfterASE = Get4dNoiseUint32( indexEastX, indexSouthY, indexAboveZ, indexAfterT, seed );
		uint32_t noiseAfterANW = Get4dNoiseUint32( indexWestX, indexNorthY, indexAboveZ, indexAfterT, seed );
		uint32_t noiseAfterANE = Get4dNoiseUint32( indexEastX, indexNorthY, indexAboveZ, indexAfterT, seed );

		// Mask with 15 (mod 16) to look up in gradients table
		::glm::vec4 gradientBeforeBSW = gradients[ noiseBeforeBSW & 0x0000000F ];
		::glm::vec4 gradientBeforeBSE = gradients[ noiseBeforeBSE & 0x0000000F ];
		::glm::vec4 gradientBeforeBNW = gradients[ noiseBeforeBNW & 0x0000000F ];
		::glm::vec4 gradientBeforeBNE = gradients[ noiseBeforeBNE & 0x0000000F ];
		::glm::vec4 gradientBeforeASW = gradients[ noiseBeforeASW & 0x0000000F ];
		::glm::vec4 gradientBeforeASE = gradients[ noiseBeforeASE & 0x0000000F ];
		::glm::vec4 gradientBeforeANW = gradients[ noiseBeforeANW & 0x0000000F ];
		::glm::vec4 gradientBeforeANE = gradients[ noiseBeforeANE & 0x0000000F ];
		::glm::vec4 gradientAfterBSW = gradients[ noiseAfterBSW & 0x0000000F ];
		::glm::vec4 gradientAfterBSE = gradients[ noiseAfterBSE & 0x0000000F ];
		::glm::vec4 gradientAfterBNW = gradients[ noiseAfterBNW & 0x0000000F ];
		::glm::vec4 gradientAfterBNE = gradients[ noiseAfterBNE & 0x0000000F ];
		::glm::vec4 gradientAfterASW = gradients[ noiseAfterASW & 0x0000000F ];
		::glm::vec4 gradientAfterASE = gradients[ noiseAfterASE & 0x0000000F ];
		::glm::vec4 gradientAfterANW = gradients[ noiseAfterANW & 0x0000000F ];
		::glm::vec4 gradientAfterANE = gradients[ noiseAfterANE & 0x0000000F ];

		// Dot each corner's gradient with displacement from corner to position
		::glm::vec4 displacementFromBeforeBSW( currentPos.x - cellMins.x, currentPos.y - cellMins.y, currentPos.z - cellMins.z, currentPos.w - cellMins.w );
		::glm::vec4 displacementFromBeforeBSE( currentPos.x - cellMaxs.x, currentPos.y - cellMins.y, currentPos.z - cellMins.z, currentPos.w - cellMins.w );
		::glm::vec4 displacementFromBeforeBNW( currentPos.x - cellMins.x, currentPos.y - cellMaxs.y, currentPos.z - cellMins.z, currentPos.w - cellMins.w );
		::glm::vec4 displacementFromBeforeBNE( currentPos.x - cellMaxs.x, currentPos.y - cellMaxs.y, currentPos.z - cellMins.z, currentPos.w - cellMins.w );
		::glm::vec4 displacementFromBeforeASW( currentPos.x - cellMins.x, currentPos.y - cellMins.y, currentPos.z - cellMaxs.z, currentPos.w - cellMins.w );
		::glm::vec4 displacementFromBeforeASE( currentPos.x - cellMaxs.x, currentPos.y - cellMins.y, currentPos.z - cellMaxs.z, currentPos.w - cellMins.w );
		::glm::vec4 displacementFromBeforeANW( currentPos.x - cellMins.x, currentPos.y - cellMaxs.y, currentPos.z - cellMaxs.z, currentPos.w - cellMins.w );
		::glm::vec4 displacementFromBeforeANE( currentPos.x - cellMaxs.x, currentPos.y - cellMaxs.y, currentPos.z - cellMaxs.z, currentPos.w - cellMins.w );
		::glm::vec4 displacementFromAfterBSW( currentPos.x - cellMins.x, currentPos.y - cellMins.y, currentPos.z - cellMins.z, currentPos.w - cellMaxs.w );
		::glm::vec4 displacementFromAfterBSE( currentPos.x - cellMaxs.x, currentPos.y - cellMins.y, currentPos.z - cellMins.z, currentPos.w - cellMaxs.w );
		::glm::vec4 displacementFromAfterBNW( currentPos.x - cellMins.x, currentPos.y - cellMaxs.y, currentPos.z - cellMins.z, currentPos.w - cellMaxs.w );
		::glm::vec4 displacementFromAfterBNE( currentPos.x - cellMaxs.x, currentPos.y - cellMaxs.y, currentPos.z - cellMins.z, currentPos.w - cellMaxs.w );
		::glm::vec4 displacementFromAfterASW( currentPos.x - cellMins.x, currentPos.y - cellMins.y, currentPos.z - cellMaxs.z, currentPos.w - cellMaxs.w );
		::glm::vec4 displacementFromAfterASE( currentPos.x - cellMaxs.x, currentPos.y - cellMins.y, currentPos.z - cellMaxs.z, currentPos.w - cellMaxs.w );
		::glm::vec4 displacementFromAfterANW( currentPos.x - cellMins.x, currentPos.y - cellMaxs.y, currentPos.z - cellMaxs.z, currentPos.w - cellMaxs.w );
		::glm::vec4 displacementFromAfterANE( currentPos.x - cellMaxs.x, currentPos.y - cellMaxs.y, currentPos.z - cellMaxs.z, currentPos.w - cellMaxs.w );

		float dotBeforeBSW = ::glm::dot( gradientBeforeBSW, displacementFromBeforeBSW );
		float dotBeforeBSE = ::glm::dot( gradientBeforeBSE, displacementFromBeforeBSE );
		float dotBeforeBNW = ::glm::dot( gradientBeforeBNW, displacementFromBeforeBNW );
		float dotBeforeBNE = ::glm::dot( gradientBeforeBNE, displacementFromBeforeBNE );
		float dotBeforeASW = ::glm::dot( gradientBeforeASW, displacementFromBeforeASW );
		float dotBeforeASE = ::glm::dot( gradientBeforeASE, displacementFromBeforeASE );
		float dotBeforeANW = ::glm::dot( gradientBeforeANW, displacementFromBeforeANW );
		float dotBeforeANE = ::glm::dot( gradientBeforeANE, displacementFromBeforeANE );
		float dotAfterBSW = ::glm::dot( gradientAfterBSW, displacementFromAfterBSW );
		float dotAfterBSE = ::glm::dot( gradientAfterBSE, displacementFromAfterBSE );
		float dotAfterBNW = ::glm::dot( gradientAfterBNW, displacementFromAfterBNW );
		float dotAfterBNE = ::glm::dot( gradientAfterBNE, displacementFromAfterBNE );
		float dotAfterASW = ::glm::dot( gradientAfterASW, displacementFromAfterASW );
		float dotAfterASE = ::glm::dot( gradientAfterASE, displacementFromAfterASE );
		float dotAfterANW = ::glm::dot( gradientAfterANW, displacementFromAfterANW );
		float dotAfterANE = ::glm::dot( gradientAfterANE, displacementFromAfterANE );

		// Do a smoothed (nonlinear) weighted average of dot results
		float weightEast  = Easing::SmoothStep( displacementFromBeforeBSW.x );
		float weightNorth = Easing::SmoothStep( displacementFromBeforeBSW.y );
		float weightAbove = Easing::SmoothStep( displacementFromBeforeBSW.z );
		float weightAfter = Easing::SmoothStep( displacementFromBeforeBSW.w );
		float weightWest   = 1.f - weightEast;
		float weightSouth  = 1.f - weightNorth;
		float weightBelow  = 1.f - weightAbove;
		float weightBefore = 1.f - weightAfter;

		// 16-way blend (16 -> 8 -> 4 -> 2 -> 1)
		float blendBeforeBelowSouth	= (weightEast * dotBeforeBSE) + (weightWest * dotBeforeBSW);
		float blendBeforeBelowNorth	= (weightEast * dotBeforeBNE) + (weightWest * dotBeforeBNW);
		float blendBeforeAboveSouth	= (weightEast * dotBeforeASE) + (weightWest * dotBeforeASW);
		float blendBeforeAboveNorth	= (weightEast * dotBeforeANE) + (weightWest * dotBeforeANW);
		float blendAfterBelowSouth  = (weightEast * dotAfterBSE)  + (weightWest * dotAfterBSW);
		float blendAfterBelowNorth  = (weightEast * dotAfterBNE)  + (weightWest * dotAfterBNW);
		float blendAfterAboveSouth  = (weightEast * dotAfterASE)  + (weightWest * dotAfterASW);
		float blendAfterAboveNorth  = (weightEast * dotAfterANE)  + (weightWest * dotAfterANW);
		float blendBeforeBelow = (weightSouth * blendBeforeBelowSouth) + (weightNorth * blendBeforeBelowNorth);
		float blendBeforeAbove = (weightSouth * blendBeforeAboveSouth) + (weightNorth * blendBeforeAboveNorth);
		float blendAfterBelow  = (weightSouth * blendAfterBelowSouth)  + (weightNorth * blendAfterBelowNorth);
		float blendAfterAbove  = (weightSouth * blendAfterAboveSouth)  + (weightNorth * blendAfterAboveNorth);
		float blendBefore = (weightBelow * blendBeforeBelow) + (weightAbove * blendBeforeAbove);
		float blendAfter  = (weightBelow * blendAfterBelow) + (weightAbove * blendAfterAbove);
		float blendTotal = (weightBefore * blendBefore) + (weightAfter * blendAfter);
		float noiseThisOctave = blendTotal * (1.f / 0.6875f); // 4D Perlin is in [-.6875,.6875]; map to ~[-1,1]

		// Accumulate results and prepare for next octave (if any)
		totalNoise += noiseThisOctave * currentAmplitude;
		totalAmplitude += currentAmplitude;
		currentAmplitude *= octavePersistence;
		currentPos *= octaveScale;
		currentPos.x += OCTAVE_OFFSET; // Add "irrational" offset to de-align octave grids
		currentPos.y += OCTAVE_OFFSET; // Add "irrational" offset to de-align octave grids
		currentPos.z += OCTAVE_OFFSET; // Add "irrational" offset to de-align octave grids
		currentPos.w += OCTAVE_OFFSET; // Add "irrational" offset to de-align octave grids
		++ seed; // Eliminates octaves "echoing" each other (since each octave is uniquely seeded)
	}

	// Re-normalize total noise to within [-1,1] and fix octaves pulling us far away from limits
	if( renormalize && totalAmplitude > 0.f )
	{
		totalNoise /= totalAmplitude;				// Amplitude exceeds 1.0 if octaves are used
		totalNoise = (totalNoise * 0.5f) + 0.5f;	// Map to [0,1]
		totalNoise = Easing::SmoothStep( totalNoise );		// Push towards extents (octaves pull us away)
		totalNoise = (totalNoise * 2.0f) - 1.f;		// Map back to [-1,1]
	}

	return totalNoise;
}

} // SquirrelNoise4

