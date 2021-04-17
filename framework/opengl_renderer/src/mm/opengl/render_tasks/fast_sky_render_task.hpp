#pragma once

#include <mm/services/opengl_renderer.hpp>

#include <glm/vec3.hpp>

// fwd
namespace MM::OpenGL {
	class Shader;
	class Buffer;
	class VertexArrayObject;

	namespace RenderTasks {

		struct FastSkyContext {
			float time = 0.f;

			float cirrus = 0.4f;
			float cumulus = 0.8f;

			glm::vec3 fsun {0,0,1};
		};

		class FastSky : public MM::OpenGL::RenderTask {
			private:
				std::shared_ptr<MM::OpenGL::Shader> _shader;
				std::unique_ptr<MM::OpenGL::Buffer> _vertexBuffer;
				std::unique_ptr<MM::OpenGL::VertexArrayObject> _vao;

				FastSkyContext _default_context;

			public:
				FastSky(MM::Engine& engine);
				~FastSky(void);

				const char* name(void) override { return "FastSky"; }

				void render(MM::Services::OpenGLRenderer& rs, MM::Engine& engine) override;

			public:
				const char* vertexPath = "shader/fast_sky_render_task/vert.glsl";
				const char* fragmentPath = "shader/fast_sky_render_task/frag.glsl";

				std::string target_fbo = "display";

				void reloadShaders(MM::Engine& engine);

			private:
				void setupShaderFiles(void);
		};

	} // RenderTasks

} // MM::OpenGL

