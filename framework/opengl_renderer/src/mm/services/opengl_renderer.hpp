#pragma once

#include <mm/engine.hpp>
#include <mm/services/sdl_service.hpp>

#include <mm/opengl/frame_buffer_object.hpp>
#include <mm/opengl/render_task.hpp>

#include <vector>
#include <unordered_map>

namespace MM::Services {

	class OpenGLRenderer : public Service {
		public:
			OpenGLRenderer(void);
			~OpenGLRenderer(void);

		public:
			std::vector<std::unique_ptr<OpenGL::RenderTask>> render_tasks;
			std::unordered_map<std::string, std::shared_ptr<MM::OpenGL::FrameBufferObject>> targets;

			// add rendertask helper
			// TODO: forward args
			template<typename RT_T>
			RT_T& addRenderTask(::MM::Engine& engine) {
				return *(RT_T*)render_tasks.emplace_back(std::make_unique<RT_T>(engine)).get();
			}

		private:
			SDLService::EventHandlerHandle _sdl_event_handle = nullptr;

		public:
			bool enable(Engine& engine, std::vector<UpdateStrategies::TaskInfo>& task_array) override;
			void disable(Engine& engine) override;

			const char* name(void) override { return "OpenGLRendererService"; }

		private:
			void render(Engine& engine);
	};

} // namespace MM::Services

