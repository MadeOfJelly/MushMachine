#include "./blit_fb.hpp"

#include <mm/services/sdl_service.hpp>

#include <tracy/Tracy.hpp>

namespace MM::OpenGL::RenderTasks {

BlitFB::BlitFB(Engine& engine) {
	// assuming fbo textures are the size of the window buffer
	auto [x, y] = engine.getService<MM::Services::SDLService>().getWindowSize();

	srcX1 = x;
	srcY1 = y;

	dstX1 = x;
	dstY1 = y;
}

BlitFB::~BlitFB(void) {
}

void BlitFB::render(Services::OpenGLRenderer& rs, Engine&) {
	ZoneScopedN("MM::OpenGL::RenderTasks::BlitFB::render");

	rs.targets[read_fbo]->bind(FrameBufferObject::R);
	rs.targets[write_fbo]->bind(FrameBufferObject::W);

	glBlitFramebuffer(
		srcX0, srcY0,
		srcX1, srcY1,
		dstX0, dstY0,
		dstX1, dstY1,
		mask,
		filter
	);
}

} // MM::OpenGL::RenderTasks

