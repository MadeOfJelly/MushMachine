#pragma once

#include "../render_task.hpp"
#include <mm/services/opengl_renderer.hpp>

#include <mm/opengl/camera_3d.hpp>

//#include <glm/fwd.hpp>
#include <glm/vec4.hpp>

// fwd
namespace MM::OpenGL {
	class Shader;
	class Buffer;
	class VertexArrayObject;
}

namespace MM::OpenGL::RenderTasks {

	class SimpleSpriteSheet : public RenderTask {
		private:
			std::shared_ptr<Shader> _shader;
			std::unique_ptr<Buffer> _vertexBuffer;
			std::unique_ptr<VertexArrayObject> _vao;

		public:
			glm::vec4 default_color {1.f, 1.f, 1.f, 1.f};

			Camera3D default_cam;

			SimpleSpriteSheet(Engine& engine);
			~SimpleSpriteSheet(void);

			const char* name(void) override { return "SimpleSpriteSheet"; }

			void render(Services::OpenGLRenderer& rs, Engine& engine) override;

		public:
			const char* vertexPath = "shader/simple_spritesheet_render_task/vert.glsl";
			const char* fragmentPath = "shader/simple_spritesheet_render_task/frag.glsl";

			std::string target_fbo = "display";

		private:
			void setupShaderFiles(void);
	};

} // MM::OpenGL::RenderTasks

