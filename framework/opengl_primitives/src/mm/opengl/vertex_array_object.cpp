#include "./vertex_array_object.hpp"

#ifdef MM_OPENGL_3_GLES
	#include <GLES3/gl3.h>
#else
	#include <glad/glad.h>
#endif

namespace MM::OpenGL {

VertexArrayObject::VertexArrayObject(void) {
	glGenVertexArrays(1, &_rendererID);
}

VertexArrayObject::~VertexArrayObject(void) {
	glDeleteVertexArrays(1, &_rendererID);
}

void VertexArrayObject::bind(void) const {
	glBindVertexArray(_rendererID);
}

void VertexArrayObject::unbind(void) const {
	glBindVertexArray(0);
}

} // MM::OpenGL

