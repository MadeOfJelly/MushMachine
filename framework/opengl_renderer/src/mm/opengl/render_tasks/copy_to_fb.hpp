#pragma once

#include "../render_task.hpp"
#include <mm/services/opengl_renderer.hpp>

// fwd
namespace MM::OpenGL {
	class Shader;
	class Buffer;
	class VertexArrayObject;
}

namespace MM::OpenGL::RenderTasks {

	class CopyToFB : public RenderTask {
		private:
			std::shared_ptr<Shader> _shader;
			std::unique_ptr<Buffer> _vertexBuffer;
			std::unique_ptr<VertexArrayObject> _vao;

		public:
			CopyToFB(Engine& engine);
			~CopyToFB(void);

			const char* name(void) override { return "CopyToFB"; }

			void render(Services::OpenGLRenderer& rs, Engine& engine) override;

		public:
			const char* vertexPath = "shader/copy_to_fb_render_task/vert.glsl";
			const char* fragmentPath = "shader/copy_to_fb_render_task/frag.glsl";

			std::string target_fbo = "display";
			std::string src_tex = "game_view";

			void reloadShaders(Engine& engine);

		private:
			void setupShaderFiles(void);
	};

} // MM::OpenGL::RenderTasks

