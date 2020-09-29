#pragma once

#include <mm/opengl/texture.hpp>

#include <mm/opengl/instance_buffer.hpp>

#include <mm/opengl/spritesheet.hpp>

#include <cstring>
#include <string>
#include <vector>

namespace MM::OpenGL {

struct TilemapRenderable {
	struct Tile {
		float pos[2];
		uint32_t sprite_sheet_index;
		//float color[3] = {1.f, 1.f, 1.f};

		static void setupGLBindings(void) {
			static_assert(std::is_standard_layout<Tile>::value); // check if offsetof() is usable

			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Tile), (void*) offsetof(Tile, pos));
			glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, sizeof(Tile), (void*) offsetof(Tile, sprite_sheet_index));
			//glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Tile), (void*) offsetof(Tile, color));
		}
	} /*__attribute__((packed))*/;

	struct SpriteLayer {
		MM::OpenGL::SpriteSheet sprite_sheet;

		std::vector<Tile> map; // TODO: move this to tilemap
		std::shared_ptr<MM::OpenGL::InstanceBuffer<Tile>> map_buffer = std::make_shared<MM::OpenGL::InstanceBuffer<Tile>>();

		void syncMapBuffer(void) {
			auto* arr = map_buffer->map(map.size(), GL_STATIC_DRAW);
			std::memcpy(arr, map.data(), map.size() * sizeof(Tile));

			map_buffer->unmap();
		}
	};

	std::vector<SpriteLayer> sprite_layer;
	float z = 0.f;
};

} // MM::OpenGL

