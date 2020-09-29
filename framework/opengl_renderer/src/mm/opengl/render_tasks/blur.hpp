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

	class Blur : public RenderTask {
		private:
			std::shared_ptr<Shader> _shader;
			std::unique_ptr<Buffer> _vertexBuffer;
			std::unique_ptr<VertexArrayObject> _vao;

		public:
			Blur(Engine& engine);
			~Blur(void);

			void render(Services::OpenGLRenderer& rs, Engine& engine) override;

		public:
			const char* vertexPath = "shader/blur_render_task/vert.glsl";
			const char* fragmentPath = "shader/blur_render_task/frag.glsl";

			std::string target_fbo = "blur_out";
			std::string src_fbo = "blur_in";

		private:
			void setupShaderFiles(void);
	};

} // MM::OpenGL::RenderTasks

