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
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
		return _fbo;
	}

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

	//glFramebufferTexture2D(target, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex->getHandle(), 0);
	glFramebufferTexture2D(target, attachment_type, GL_TEXTURE_2D, tex->getHandle(), 0);
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

