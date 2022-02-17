#pragma once

#include <mm/resource_manager.hpp>

namespace MM::OpenGL {

	struct TextureLoaderFile;
	struct TextureLoaderConstBuffer;
	struct TextureLoaderSDLSurface;

	class Texture {
		private:
			uint32_t const _handle;

		private:
			friend struct TextureLoaderFile;
			friend struct TextureLoaderConstBuffer;
			friend struct TextureLoaderSDLSurface;

			Texture(
				uint32_t handle,
				int32_t width_, int32_t height_,
				int32_t internalFormat, int32_t format, int32_t type,
				uint32_t samples = 0u
			);

		public:
			using handle_t = std::shared_ptr<Texture>;

			int32_t const width;
			int32_t const height;
			//int32_t const bpp; // bits per pixel
			uint32_t const samples{0u}; // sample count, 0 == off

		private:
			int32_t const _internalFormat;
			int32_t const _format;
			int32_t const _type;

		public:
			~Texture();

			uint32_t getHandle(void) const;

			void bind(uint32_t slot) const;
			void unbind(void) const;

			void resize(int32_t new_width, int32_t new_height);

			static handle_t createEmpty(int32_t internalFormat, int32_t width, int32_t height, int32_t format, int32_t type);
			static handle_t createEmptyMultiSampled(int32_t internalFormat, int32_t width, int32_t height, uint32_t samples);
	};

} // MM::OpenGL

