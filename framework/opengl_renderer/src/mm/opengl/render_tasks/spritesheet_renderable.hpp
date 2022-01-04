#pragma once

#include <mm/opengl/spritesheet.hpp>

namespace MM::OpenGL {

struct SpriteSheetRenderable {
	SpriteSheet sp;

	uint32_t tile_index = 0;
};

} // MM::OpenGL

