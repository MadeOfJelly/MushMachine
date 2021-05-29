#pragma once

#include <SDL.h>

#include <mm/engine.hpp>

//#include <vector> // engine.hpp
//#include <functional> // engine.hpp

namespace MM::Services {

	class SDLService : public Service {
		public:
			uint32_t sdl_init_flags = SDL_INIT_EVERYTHING;

			SDL_Window* win{nullptr};
			SDL_GLContext gl_context{nullptr};

			bool setDefaultWindowIcon = true;

			using EventHandlerType = std::function<bool(const SDL_Event&)>;
			using EventHandlerHandle = EventHandlerType*;

		private:
			std::vector<std::unique_ptr<EventHandlerType>> _event_handler_list;

		public:
			SDLService(uint32_t sdl_init_flags = SDL_INIT_EVERYTHING);
			~SDLService(void);

		public:
			bool enable(Engine& engine, std::vector<UpdateStrategies::TaskInfo>& task_array) override;
			void disable(Engine& engine) override; // destroys windows and ass contexts

			const char* name(void) override { return "SDLService"; }

		public:
			// creates window, returns if created successfully
			bool createWindow(const char* title, int screen_width, int screen_height, uint32_t sdl_window_flags = SDL_WINDOW_SHOWN);
			void destroyWindow(void);

			std::pair<int, int> getWindowSize(SDL_Window* win_ptr = nullptr);

			bool createGLContext(SDL_Window* win_ptr = nullptr);

			// creates a window and a gl_context
			bool createGLWindow(const char* title, int screen_width, int screen_height, uint32_t sdl_window_flags = SDL_WINDOW_SHOWN);

		private:
			void processEvents(Engine& engine);

		public:
			EventHandlerHandle addEventHandler(std::function<bool(const SDL_Event&)> function);
			void removeEventHandler(EventHandlerHandle function_handle);

			// sets the icon of the "current" window
			void setDefaultIcon(void);
			void setIcon(SDL_Surface* surf);
			void setIcon(void* data, size_t width, size_t height, size_t bytes_per_pixel);
	};

} // namespace MM::Services

