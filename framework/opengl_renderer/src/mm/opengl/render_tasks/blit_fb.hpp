#pragma once

#include "../render_task.hpp"

#include <mm/services/opengl_renderer.hpp>

namespace MM::OpenGL::RenderTasks {

	class BlitFB : public RenderTask {
		public:
			BlitFB(Engine& engine);
			~BlitFB(void);

			void render(Services::OpenGLRenderer& rs, Engine& engine) override;

		public:
			std::string read_fbo = "game_view";
			std::string write_fbo = "display";

			// blit params
			GLint srcX0 = 0;
			GLint srcY0 = 0;
			GLint srcX1 = 0; // u will want to set this
			GLint srcY1 = 0; // u will want to set this

			GLint dstX0 = 0;
			GLint dstY0 = 0;
			GLint dstX1 = 0; // u will want to set this
			GLint dstY1 = 0; // u will want to set this

			GLbitfield mask = GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT;
			GLenum filter = GL_NEAREST;
	};

} // MM::OpenGL::RenderTasks

