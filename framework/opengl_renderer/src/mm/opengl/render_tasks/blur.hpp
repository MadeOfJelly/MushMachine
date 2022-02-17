#pragma once

#include "../render_task.hpp"
#include <mm/services/opengl_renderer.hpp>

#include <glm/vec2.hpp>

// fwd
namespace MM::OpenGL {
	class Shader;
	class Buffer;
	class VertexArrayObject;
}

namespace MM::OpenGL::RenderTasks {

	// this task expects to read and write to textures
	class Blur : public RenderTask {
		private:
			std::shared_ptr<Shader> _hShader;
			std::shared_ptr<Shader> _vShader;
			std::unique_ptr<Buffer> _vertexBuffer;
			std::unique_ptr<VertexArrayObject> _vao;

		public:
			Blur(Engine& engine);
			~Blur(void);

			const char* name(void) override { return "Blur"; }

			void render(Services::OpenGLRenderer& rs, Engine& engine) override;

		public:
			const char* vertexPath = "shader/blur_render_task/vert.glsl";
			const char* fragmentHPath = "shader/blur_render_task/frag_h.glsl";
			const char* fragmentVPath = "shader/blur_render_task/frag_v.glsl";

			std::string out_fbo = "blur_io";
			std::string temp_fbo = "blur_temp";

			// bc of it beeing a 2 pass, we need to flipflop
			std::string in_tex = "blur_io";
			std::string out_tex = "blur_io";
			std::string temp_tex = "blur_temp";

			// determines the kernel lookup offset. "ideal" is 1/tex_size
			glm::vec2 tex_offset {0.001f, 0.001f};

		private:
			void setupShaderFiles(void);
	};

} // MM::OpenGL::RenderTasks

