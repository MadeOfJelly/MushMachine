#include "./fbo_builder.hpp"

namespace MM::OpenGL {

FBOBuilder::FBOBuilder(void) {
}

FBOBuilder::~FBOBuilder(void) {
}

FBOBuilder FBOBuilder::start(void) {
	FBOBuilder fbob{};

	fbob._fbo.reset(new FrameBufferObject());
	fbob._fbo->bind(fbob._currTarget);

	return fbob;
}

std::shared_ptr<FrameBufferObject> FBOBuilder::finish(void) {
	const auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status == GL_FRAMEBUFFER_COMPLETE) {
		return _fbo;
	}

	const char* error_str = "UNK";
	switch (status) {
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			error_str = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
			break;
#ifdef MM_OPENGL_3_GLES
		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
			error_str = "GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS";
			break;
#endif
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			error_str = "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
			break;
		case GL_FRAMEBUFFER_UNSUPPORTED:
			error_str = "GL_FRAMEBUFFER_UNSUPPORTED";
			break;
	}
	SPDLOG_ERROR("framebuffer status: {}", error_str);

	return nullptr;
}

FBOBuilder& FBOBuilder::setTarget(FrameBufferObject::Target target) {
	_currTarget = target;
	_fbo->bind(target);

	return *this;
}

FBOBuilder& FBOBuilder::attachTexture(std::shared_ptr<Texture> tex, GLuint attachment_type) {
	GLenum target;
	switch (_currTarget) {
		case FrameBufferObject::Target::RW:
			target = GL_FRAMEBUFFER;
			break;
		case FrameBufferObject::Target::R:
			target = GL_READ_FRAMEBUFFER;
			break;
		case FrameBufferObject::Target::W:
			target = GL_DRAW_FRAMEBUFFER;
			break;
	}

	if (tex->samples == 0u) {
		glFramebufferTexture2D(target, attachment_type, GL_TEXTURE_2D, tex->getHandle(), 0);
	} else {
#ifndef MM_OPENGL_3_GLES
		glFramebufferTexture2D(target, attachment_type, GL_TEXTURE_2D_MULTISAMPLE, tex->getHandle(), 0);
#else
		assert(false && "GLES has no multisampling support");
#endif
	}
	_fbo->_texAttachments.push_back(tex); // keep a ref at the fbo

	return *this;
}

FBOBuilder& FBOBuilder::setResize(bool enable) {
	_fbo->_resize = enable;

	return *this;
}

FBOBuilder& FBOBuilder::setResizeFactors(float width, float height) {
	assert(width > 0.f);
	assert(height > 0.f);

	_fbo->_resize_factor_width = width;
	_fbo->_resize_factor_height = height;

	return *this;
}

} // MM::OpenGL

