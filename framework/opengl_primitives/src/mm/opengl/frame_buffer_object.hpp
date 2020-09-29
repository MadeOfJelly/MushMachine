#pragma once

#include <cstdint>
#include <vector>
#include <memory>

#ifdef MM_OPENGL_3_GLES
	#include <GLES3/gl3.h>
#else
	#include <glad/glad.h>
#endif

namespace MM::Services {
	class OpenGLRenderer;
} // fwd

namespace MM::OpenGL {

	class Texture;

	class FrameBufferObject {
		friend class FBOBuilder;
		friend class MM::Services::OpenGLRenderer;

		private:
			uint32_t _fboID;

			bool _resize = true;
			float _resize_factor_width = 1.f;
			float _resize_factor_height = 1.f;

		public:
			std::vector<std::shared_ptr<Texture>> _texAttachments;

		private:
			FrameBufferObject(void);

		public:
			~FrameBufferObject(void);

			enum Target {
				R = 1 << 0,
				W = 1 << 1,
				RW = R | W
			};
			void bind(Target target) const;
			//void unbind(void) const;

			void clear(float r, float g, float b, float a, GLbitfield target_mask = GL_COLOR_BUFFER_BIT);
	};
} // MM::OpenGL

