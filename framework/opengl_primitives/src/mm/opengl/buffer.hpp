//
// Created by FlaXxy on 21.05.2018.
//

#pragma once

#ifdef MM_OPENGL_3_GLES
	#include <GLES3/gl3.h>
#else
	#include <glad/glad.h>
#endif

#include <cstdint>
#include <cstddef> // size_t

namespace MM::OpenGL {
	/**
	 * A fixed size buffer
	 */
	class Buffer {
		private:
			GLuint _handle = 0;
			std::size_t _size = 0;

		public:
			Buffer(const void* data, std::size_t size, GLenum usage);
			~Buffer(void);

			void bind(GLenum target) const;
			void unbind(GLenum target) const;

			std::size_t getSize(void) const;
	};
} // MM::OpenGL

