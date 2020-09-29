#pragma once

#include <SDL.h>

namespace MM::SimpleSDLRenderer {

	struct Target {
		SDL_Texture* texture = nullptr;

		~Target(void) {
			SDL_DestroyTexture(texture);
		}

		void reset(SDL_Renderer* ren, int width, int height) {
			if (texture)
				SDL_DestroyTexture(texture);

			texture = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
		}

		void clear(SDL_Renderer* ren, SDL_Color col = {0,0,0,255}) {
			set(ren);
			SDL_SetRenderDrawColor(ren, col.r, col.g, col.b, col.a);
			SDL_RenderClear(ren);
		}

		void set(SDL_Renderer* ren) {
			SDL_SetRenderTarget(ren, texture);
		}
	};

} // MM::SimpleSDLRenderer

