#pragma once

#include "../render_task.hpp"
#include <mm/services/opengl_renderer.hpp>

namespace MM::OpenGL::RenderTasks {

	// RT to avoid name collisons
	class ImGuiRT : public RenderTask {
		public:
			ImGuiRT(Engine& engine);
			~ImGuiRT(void);

			const char* name(void) override { return "ImGuiRT"; }

			void render(Services::OpenGLRenderer& rs, Engine& engine) override;

		public:
			std::string target_fbo = "display";
	};

} // MM::OpenGL::RenderTasks

