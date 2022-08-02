#pragma once

#include <mm/opengl/render_task.hpp>
#include <mm/opengl/shader.hpp>
#include <mm/opengl/buffer.hpp>
#include <mm/opengl/vertex_array_object.hpp>

namespace MM::OpenGL::RenderTasks {

	class Composition : public MM::OpenGL::RenderTask {
		private:
			std::shared_ptr<MM::OpenGL::Shader> _shader;
			std::unique_ptr<MM::OpenGL::Buffer> _vertexBuffer;
			std::unique_ptr<MM::OpenGL::VertexArrayObject> _vao;

		public:
			Composition(MM::Engine& engine);
			~Composition(void);

			const char* name(void) override { return "Composition"; }

			void render(MM::Services::OpenGLRenderer& rs, MM::Engine& engine) override;

		public:
			const char* vertexPath {"shader/composition_render_task/vert.glsl"};
			const char* fragmentPath {"shader/composition_render_task/frag.glsl"};

			std::string target_fbo {"display"};

			std::string color_tex {"hdr_color"};
			std::string bloom_tex {"bloom"};

			float bloom_factor {0.8f};

			void reloadShaders(MM::Engine& engine);

		private:
			void setupShaderFiles(void);
	};

} // MM::OpenGL::RenderTasks

