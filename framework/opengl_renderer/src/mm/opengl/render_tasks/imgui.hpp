#pragma once

#include "../render_task.hpp"
#include <mm/services/opengl_renderer.hpp>

namespace MM::OpenGL::RenderTasks {

	// RT to avoid name collisons
	class ImGuiRT : public RenderTask {
		public:
			ImGuiRT(Engine& engine);
			~ImGuiRT(void);

			void render(Services::OpenGLRenderer& rs, Engine& engine) override;
	};

} // MM::OpenGL::RenderTasks

