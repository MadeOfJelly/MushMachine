#pragma once

#include <mm/opengl/render_task.hpp>
#include <mm/services/opengl_renderer.hpp>

#include <glm/fwd.hpp>

// TODO: make the renderer provide a time delta
#include <chrono>
#include <string>
#include <unordered_map>

// fwd
namespace MM::OpenGL {
	class Shader;
	class Buffer;
	class VertexArrayObject;

	template<typename TInstance>
	class InstanceBuffer;
}

namespace MM::OpenGL::RenderTasks {

	class LiteParticles2D : public RenderTask {
		private:
			std::shared_ptr<Shader> _tf_shader;
			std::shared_ptr<Shader> _points_shader;

			std::unordered_map<uint32_t, uint16_t> _type_map {}; // maps id to "index" in type texture

			// double buffered
			bool first_particles_buffer {true};
			std::unique_ptr<VertexArrayObject> _tf_vao[2];
			std::unique_ptr<VertexArrayObject> _render_vao[2];

			// vec2 pos (2 floats)
			// half vec2 vel (1 float)
			// fixedpoint age + uint16 type (1 float)
			std::unique_ptr<MM::OpenGL::InstanceBuffer<glm::vec4>> _particles_0_buffers[2];

			uint32_t _particle_buffer_size {10'000};
			size_t _particle_buffer_next_index {0};

			using clock = std::chrono::high_resolution_clock;
			std::chrono::time_point<clock> _last_time;
			float _time {0};

		public:
			//glm::vec3 env_vec{0, 1, 0};
			//float env_force{0.3};
			//float noise_force{0.5};
			//float dampening{0.99};

			LiteParticles2D(Engine& engine);
			~LiteParticles2D(void);

			const char* name(void) override { return "LiteParticles2D"; }

			void uploadParticles(Services::OpenGLRenderer& rs, Scene& scene);
			void computeParticles(Services::OpenGLRenderer& rs, Scene& scene);
			void renderParticles(Services::OpenGLRenderer& rs, Scene& scene);
			void render(Services::OpenGLRenderer& rs, Engine& engine) override;

			void resetBuffers(void);
			void resetTypeBuffers(void);

		public:
			const char* commonPathTF {"shader/lite_particles2d/common.glsl"};

			const char* vertexPathTF {"shader/lite_particles2d/tf_vert.glsl"};
			const char* fragmentPathTF {"shader/lite_particles2d/tf_frag.glsl"}; // "empty" bc of webgl (es)

			const char* vertexPathPoints {"shader/lite_particles2d/point_vert.glsl"};
			const char* fragmentPathPoints {"shader/lite_particles2d/point_frag.glsl"};

			std::string target_fbo {"display"};

		private:
			void setupShaderFiles(void);
	};

} // MM::OpenGL::RenderTasks

