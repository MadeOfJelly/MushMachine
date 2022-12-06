#pragma once

#include <mm/s6zer/serialize.hpp>

#include <mm/components/color.hpp>

namespace MM::Components {

MM_DEFINE_SERIALIZE(Color,
	MM_S6ZER_BAIL(stream.serializeFloat(data.color.r))
	MM_S6ZER_BAIL(stream.serializeFloat(data.color.g))
	MM_S6ZER_BAIL(stream.serializeFloat(data.color.b))
	MM_S6ZER_BAIL(stream.serializeFloat(data.color.a))
)

} // MM::Components

