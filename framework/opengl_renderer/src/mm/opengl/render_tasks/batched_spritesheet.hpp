#pragma once

#include "../render_task.hpp"
#include <mm/services/opengl_renderer.hpp>

#include <mm/opengl/camera_3d.hpp>

//#include <glm/fwd.hpp>
#include <glm/vec4.hpp>

#include <mm/opengl/instance_buffer.hpp>

// fwd
namespace MM::OpenGL {
	class Shader;
	class Buffer;
	class VertexArrayObject;
}


namespace MM::OpenGL::RenderTasks {

	class BatchedSpriteSheet : public RenderTask {
		private:
			std::shared_ptr<Shader> _shader;
			std::unique_ptr<Buffer> _vertexBuffer;
			std::unique_ptr<VertexArrayObject> _vao;

			struct gl_instance_data {
				glm::mat4 pos_trans;
				glm::vec4 color;
				uint32_t tile_index;
			};
			std::unique_ptr<MM::OpenGL::InstanceBuffer<gl_instance_data>> gl_inst_buffer;


		public:
			glm::vec4 default_color {1.f, 1.f, 1.f, 1.f};

			OpenGL::Camera3D default_cam;

			BatchedSpriteSheet(Engine& engine);
			~BatchedSpriteSheet(void);

			const char* name(void) override { return "BatchedSpriteSheet"; }

			void render(Services::OpenGLRenderer& rs, Engine& engine) override;

		public:
			const char* vertexPath = "shader/batched_spritesheet_render_task/vert.glsl";
			const char* fragmentPath = "shader/batched_spritesheet_render_task/frag.glsl";

			std::string target_fbo = "display";

		private:
			void setupShaderFiles(void);
	};

} // MM::OpenGL::RenderTasks

