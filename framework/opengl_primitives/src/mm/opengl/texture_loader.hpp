#pragma once

#include "./texture.hpp"

#include <utility>

// fwd
typedef struct SDL_Surface SDL_Surface;
namespace MM {
	class Engine;
}

namespace MM::OpenGL {

	struct TextureLoaderFile final {
		std::shared_ptr<Texture> load(Engine& engine, const std::string& path) const;
	};

	struct TextureLoaderConstBuffer final {
		std::shared_ptr<Texture> load(const uint8_t* data, size_t size) const;
	};

	struct TextureLoaderSDLSurface final {
		std::shared_ptr<Texture> load(SDL_Surface* surface) const;
	};

	struct TextureLoaderEmpty final {
		template<typename... Args>
		std::shared_ptr<Texture> load(Args&& ... args) const {
			return Texture::createEmpty(std::forward<Args>(args)...);
		}
	};

	struct TextureLoaderEmptyMultiSampled final {
		template<typename... Args>
		std::shared_ptr<Texture> load(Args&& ... args) const {
			return Texture::createEmptyMultiSampled(std::forward<Args>(args)...);
		}
	};

} // MM::OpenGL

