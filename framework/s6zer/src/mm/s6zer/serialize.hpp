#pragma once

#include "./stream.hpp"

namespace MM::s6zer {

// serialize macros

// TODO: make use of ADL, like nlohmann::json does.

/*
	defines mm_serialize functions for you.
	a "stream" object is in scope (StreamWriter/StreamReader),
	as well as the object of Type called "data".
eg:
MM_DEFINE_SERIALIZE(Test1,
	MM_S6ZER_BAIL(stream.serializeBits(data.seq, 16))
	MM_S6ZER_BAIL(stream.serializeBits(data.data1, 8))
)
*/
// TODO: refine, so we dont have to do MM_S6ZER_BAIL() everytime
#define MM_DEFINE_SERIALIZE(Type, ...) \
inline bool mm_serialize(MM::s6zer::StreamWriter& stream, const Type& data) { \
	__VA_ARGS__ \
	return true; \
} \
inline bool mm_serialize(MM::s6zer::StreamReader& stream, Type& data) { \
	__VA_ARGS__ \
	return true; \
}

} // MM::s6zer

