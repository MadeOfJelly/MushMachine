#include "./clear.hpp"

//#include <tracy/Tracy.hpp>

namespace MM::OpenGL::RenderTasks {

Clear::Clear(Engine&) {
}

Clear::~Clear(void) {
}

void Clear::render(Services::OpenGLRenderer& rs, Engine&) {
	//ZoneScopedN("MM::OpenGL::RenderTasks::Clear::render");
	rs.targets[target_fbo]->clear(r,g,b,a,mask);
}

} // MM::OpenGL::RenderTasks

