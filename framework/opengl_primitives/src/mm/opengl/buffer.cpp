//
// Created by FlaXxy on 21.05.2018.
//

#include "./buffer.hpp"

namespace MM::OpenGL {

Buffer::Buffer(const void* data, std::size_t size, GLenum usage) : _size(size) {
	glGenBuffers(1, &_handle);
	glBindBuffer(GL_ARRAY_BUFFER, _handle);
	glBufferData(GL_ARRAY_BUFFER, size, data, usage);
}

Buffer::~Buffer(void) {
	glDeleteBuffers(1,&_handle);
}

void Buffer::bind(GLenum target) const {
	glBindBuffer(target, _handle);
}

void Buffer::unbind(GLenum target) const {
	glBindBuffer(target, 0);
}

std::size_t Buffer::getSize(void) const {
	return _size;
}

} // MM::OpenGL

