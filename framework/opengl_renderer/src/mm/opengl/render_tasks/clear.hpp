#pragma once

#include <mm/services/opengl_renderer.hpp>

namespace MM::OpenGL::RenderTasks {

	class Clear : public RenderTask {
		public:
			Clear(Engine& engine);
			~Clear(void);

			const char* name(void) override { return "Clear"; }

			void render(Services::OpenGLRenderer& rs, Engine& engine) override;

		public:
			std::string target_fbo = "display";

			float r = 0.f;
			float g = 0.f;
			float b = 0.f;
			float a = 1.f;

			GLbitfield mask = GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT;
	};

} // MM::OpenGL::RenderTasks

