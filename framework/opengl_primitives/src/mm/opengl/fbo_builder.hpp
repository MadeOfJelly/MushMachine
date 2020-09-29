#pragma once

#include <memory>

#include "./frame_buffer_object.hpp"

#include "./texture.hpp"

namespace MM::OpenGL {

	class FBOBuilder {
		private:
			std::shared_ptr<FrameBufferObject> _fbo;
			FrameBufferObject::Target _currTarget = FrameBufferObject::Target::RW;

		private:
			FBOBuilder(void);

		public:
			~FBOBuilder(void);
			FBOBuilder& operator=(FBOBuilder&) = delete;

			static FBOBuilder start(void);
			std::shared_ptr<FrameBufferObject> finish(void);

			FBOBuilder& setTarget(FrameBufferObject::Target target);

			FBOBuilder& attachTexture(std::shared_ptr<Texture> tex, GLuint attachment_type = GL_COLOR_ATTACHMENT0);
			FBOBuilder& setResize(bool enable = true);
			FBOBuilder& setResizeFactors(float width = 1.f, float height = 1.f);
	};

} // MM::OpenGL

