#include "./frame_buffer_object.hpp"

namespace MM::OpenGL {

FrameBufferObject::FrameBufferObject(void) {
	glGenFramebuffers(1, &_fboID);
}

FrameBufferObject::~FrameBufferObject(void) {
	glDeleteFramebuffers(1, &_fboID);
}

void FrameBufferObject::bind(Target target) const {
	switch (target) {
		case Target::RW:
			glBindFramebuffer(GL_FRAMEBUFFER, _fboID);
			break;
		case Target::R:
			glBindFramebuffer(GL_READ_FRAMEBUFFER, _fboID);
			break;
		case Target::W:
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fboID);
			break;
	}
}

// TODO: refactor, make depth and stencil set-able
void FrameBufferObject::clear(float r, float g, float b, float a, GLbitfield target_mask) {
	bind(FrameBufferObject::RW);

	glClearColor(r, g, b, a);
	//glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glClear(target_mask);
}

} // MM::OpenGL

