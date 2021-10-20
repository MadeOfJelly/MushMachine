//
// Created by FlaXxy on 25.08.2018.
//

#pragma once

#ifdef MM_OPENGL_3_GLES
	#include <GLES3/gl3.h>
#else
	#include <glad/glad.h>
#endif

#include <cstdint>
//#include <logger.hpp>

namespace MM::OpenGL {

	template<typename TInstance>
	class InstanceBuffer {
		private:
			GLuint _handle;
			std::size_t _size;

		public:
			explicit InstanceBuffer(void) : _handle(0), _size(0) {
				glGenBuffers(1, &_handle);
			}

			~InstanceBuffer(void) {
				glDeleteBuffers(1, &_handle);
			}

			void bind(GLenum target = GL_ARRAY_BUFFER) const {
				glBindBuffer(target, _handle);
			}

			void unbind(GLenum target = GL_ARRAY_BUFFER) const {
				glBindBuffer(target, 0);
			}

			// for transform feedback
			void bindBase(GLuint index, GLenum target = GL_TRANSFORM_FEEDBACK_BUFFER) const {
				glBindBufferBase(target, index, _handle);
			}

			void resize(std::size_t size, GLenum usage) {
				glBindBuffer(GL_ARRAY_BUFFER, _handle);
				glBufferData(GL_ARRAY_BUFFER, size * sizeof(TInstance), nullptr, usage);
				_size = size;
			}

			std::size_t getSize(void) const {
				return _size;
			}

			std::size_t getSizeBytes(void) const {
				return _size * sizeof(TInstance);
			}

			TInstance* map(std::size_t size, GLenum usage = GL_DYNAMIC_DRAW, bool shrink = false) {
				if (size > _size || (shrink && (size < _size)))
					resize(size, usage);

				glBindBuffer(GL_ARRAY_BUFFER, _handle);

				auto* res = static_cast<TInstance*>(
					glMapBufferRange(
						GL_ARRAY_BUFFER,
						0,
						size * sizeof(TInstance),
						GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT
					)
				);

				return res;
			}

			TInstance* mapRange(std::size_t offset, std::size_t size, GLbitfield access = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT) {
				glBindBuffer(GL_ARRAY_BUFFER, _handle);

				auto* res = static_cast<TInstance*>(
					glMapBufferRange(
						GL_ARRAY_BUFFER,
						offset * sizeof(TInstance),
						size * sizeof(TInstance),
						access
					)
				);

				return res;
			}

			void unmap(void) {
				glBindBuffer(GL_ARRAY_BUFFER,_handle);
				glUnmapBuffer(GL_ARRAY_BUFFER);
			}
	};
} // MM::OpenGL

