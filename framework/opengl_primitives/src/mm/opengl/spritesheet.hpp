#pragma once

#include "./texture.hpp"

namespace MM::OpenGL {

// a SpriteSheet is a texture divided evenly into a Grid
struct SpriteSheet {
	MM::OpenGL::Texture::handle_t tex = nullptr;
	struct {
		uint32_t x = 1;
		uint32_t y = 1;
	} tile_count;
};

} // MM::OpenGL

