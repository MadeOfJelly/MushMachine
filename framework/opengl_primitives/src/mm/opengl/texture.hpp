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
				int32_t width, int32_t height,
				int32_t internalFormat, int32_t format, int32_t type
			);

		public:
			using handle = std::shared_ptr<Texture>;

			int32_t const width;
			int32_t const height;
			//int32_t const bpp; // bits per pixel
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

			static handle createEmpty(int32_t internalFormat, int32_t width, int32_t height, int32_t format, int32_t type);
	};

} // MM::OpenGL

