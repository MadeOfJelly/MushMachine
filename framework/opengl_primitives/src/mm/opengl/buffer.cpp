//
// Created by FlaXxy on 21.05.2018.
//

#include "./buffer.hpp"

namespace MM::OpenGL {

Buffer::Buffer(const void* data, std::size_t size, GLenum usage, GLenum target) : _size(size), _target(target) {
	glGenBuffers(1, &_handle);
	glBindBuffer(_target, _handle);
	glBufferData(_target, size, data, usage);
}

Buffer::~Buffer(void) {
	glDeleteBuffers(1, &_handle);
}

void Buffer::bind(void) const {
	glBindBuffer(_target, _handle);
}

void Buffer::bind(GLenum target) const {
	glBindBuffer(target, _handle);
}

void Buffer::unbind(void) const {
	glBindBuffer(_target, 0);
}

void Buffer::unbind(GLenum target) const {
	glBindBuffer(target, 0);
}

std::size_t Buffer::getSize(void) const {
	return _size;
}

GLuint Buffer::getHandle(void) const {
	return _handle;
}

} // MM::OpenGL

