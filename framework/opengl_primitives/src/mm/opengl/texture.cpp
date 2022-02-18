#include "./texture.hpp"

#ifdef MM_OPENGL_3_GLES
	#include <GLES3/gl3.h>
#else
	#include <glad/glad.h>
#endif


namespace MM::OpenGL {

uint32_t Texture::getHandle(void) const {
	return _handle;
}

Texture::Texture(
	uint32_t handle,
	int32_t width_, int32_t height_,
	int32_t internalFormat, int32_t format, int32_t type,
	uint32_t samples_
) : _handle(handle), width(width_), height(height_),
	samples(samples_),
	_internalFormat(internalFormat), _format(format), _type(type) {
}

Texture::~Texture(void) {
	glDeleteTextures(1, &_handle);
}

void Texture::unbind(void) const {
	// TODO: do i need ms variant?
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::bind(uint32_t slot) const {
	glActiveTexture(GL_TEXTURE0 + slot);
	if (samples == 0) {
		glBindTexture(GL_TEXTURE_2D, _handle);
	} else {
#ifndef MM_OPENGL_3_GLES
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, _handle);
#else
		assert(false && "GLES has no multisampling support");
#endif
	}
}

void Texture::resize(int32_t new_width, int32_t new_height) {
	// if (glTexImage2D == true)

	if (samples == 0) {
		glBindTexture(GL_TEXTURE_2D, _handle);
		glTexImage2D(GL_TEXTURE_2D, 0, _internalFormat, new_width, new_height, 0, _format, _type, NULL);
	} else {
#ifndef MM_OPENGL_3_GLES
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, _handle);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, _internalFormat, new_width, new_height, 0);
#else
		assert(false && "GLES has no multisampling support");
#endif
	}

	// HACK: super dirty
	*(const_cast<int32_t*>(&width)) = new_width;
	*(const_cast<int32_t*>(&height)) = new_height;
}

Texture::handle_t Texture::createEmpty(int32_t internalFormat, int32_t width, int32_t height, int32_t format, int32_t type) {
	uint32_t id;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	return handle_t(new Texture(id, width, height, internalFormat, format, type));
}

Texture::handle_t Texture::createEmptyMultiSampled(int32_t internalFormat, int32_t width, int32_t height, uint32_t samples) {
#ifndef MM_OPENGL_3_GLES
	uint32_t id;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, id);

	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

	// HACK: format + type?
	return handle_t(new Texture(id, width, height, internalFormat, 0, 0, samples));
#else
	(void)internalFormat;
	(void)width;
	(void)height;
	(void)samples;
	assert(false && "GLES has no multisampling support");
#endif
}

} // MM::OpenGL

