#pragma once

#include <mm/opengl/render_task.hpp>
#include <mm/services/opengl_renderer.hpp>

//#include <glm/fwd.hpp>
#include <glm/vec3.hpp>

// fwd
namespace MM::OpenGL {
	class Shader;
	class Buffer;
	class VertexArrayObject;
}

namespace MM::OpenGL::RenderTasks {

	class Tilemap : public RenderTask {
		private:
			std::shared_ptr<Shader> _shader;
			std::unique_ptr<Buffer> _vertexBuffer;
			std::unique_ptr<VertexArrayObject> _vao;

		public:
			Tilemap(MM::Engine& engine);
			~Tilemap(void);

			void render(MM::Services::OpenGLRenderer& rs, MM::Engine& engine) override;

		public:
			const char* vertexPath = "shader/tilemap_render_task/vert.glsl";
			const char* fragmentPath = "shader/tilemap_render_task/frag.glsl";

			std::string target_fbo = "display";

			glm::vec3 ambient_color {1.f, 1.f, 1.f};

		private:
			void setupShaderFiles(void);
	};

} // MM::OpenGL::RenderTasks

