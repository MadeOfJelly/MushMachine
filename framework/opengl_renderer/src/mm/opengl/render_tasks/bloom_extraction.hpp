#pragma once

#include <mm/opengl/render_task.hpp>
#include <mm/opengl/shader.hpp>
#include <mm/opengl/buffer.hpp>
#include <mm/opengl/vertex_array_object.hpp>

namespace MM::OpenGL::RenderTasks {

	class BloomExtraction : public RenderTask {
		private:
			std::shared_ptr<Shader> _shader;
			std::unique_ptr<Buffer> _vertexBuffer;
			std::unique_ptr<VertexArrayObject> _vao;

		public:
			BloomExtraction(Engine& engine);
			~BloomExtraction(void);

			const char* name(void) override { return "BloomExtraction"; }

			void render(Services::OpenGLRenderer& rs, Engine& engine) override;

		public:
			const char* vertexPath {"shader/bloom_extraction_render_task/vert.glsl"};
			const char* fragmentPath {"shader/bloom_extraction_render_task/frag.glsl"};

			std::string target_fbo {"display"};

			std::string src_tex {"hdr_color"};

			void reloadShaders(Engine& engine);

		private:
			void setupShaderFiles(void);
	};

} // MM::OpenGL::RenderTasks

