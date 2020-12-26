#include "./sdl_service.hpp"

#include <entt/core/hashed_string.hpp>

#include <tracy/Tracy.hpp>

#ifdef MM_OPENGL_3_GLES
	//#include <SDL_opengles2_gl2.h>
	#include <GLES3/gl3.h>
#else
	#include <glad/glad.h>
#endif

#ifndef MM_OPENGL_3_GLES
	#include <tracy/TracyOpenGL.hpp>
#else
	#define TracyGpuContext
#endif

#include <string>
#include <algorithm>

#include <mm/logger.hpp>
#define LOG_CRIT(...)		__LOG_CRIT(	"SDLService", __VA_ARGS__)
#define LOG_ERROR(...)		__LOG_ERROR("SDLService", __VA_ARGS__)
#define LOG_WARN(...)		__LOG_WARN(	"SDLService", __VA_ARGS__)
#define LOG_INFO(...)		__LOG_INFO(	"SDLService", __VA_ARGS__)
#define LOG_DEBUG(...)		__LOG_DEBUG("SDLService", __VA_ARGS__)
#define LOG_TRACE(...)		__LOG_TRACE("SDLService", __VA_ARGS__)

namespace MM::Services {

SDLService::SDLService(uint32_t sdl_init_flags) {
	MM::Logger::initSectionLogger("SDLService");

	LOG_TRACE("constructing SDLService...");

//#ifdef __EMSCRIPTEN__
	//sdl_init_flags &= ~SDL_INIT_HAPTIC;
//#endif

	if (SDL_Init(sdl_init_flags)) {
		LOG_CRIT("SDL_Init() failed: {}", SDL_GetError());
		return;
	}
}

SDLService::~SDLService(void) {
	LOG_TRACE("deconstructing SDLService...");

	if (gl_context) {
		SDL_GL_DeleteContext(gl_context);
	}

	if (win) {
		LOG_WARN("destroying open window");
		destroyWindow();
	}

	SDL_Quit();
}

std::vector<UpdateStrategies::UpdateCreationInfo> SDLService::registerUpdates(void) {
	return {
		{
			"SDLService::events"_hs,
			"SDLService::events",
			[this](Engine& e) { this->processEvents(e); },
			UpdateStrategies::update_phase_t::PRE
		}
	};
}

bool SDLService::enable(Engine&) {
	bool succ = true;
	return succ;
}

void SDLService::disable(Engine&) {
	// destroy stuff
	if (gl_context) {
		SDL_GL_DeleteContext(gl_context);
	}

	if (win) {
		LOG_WARN("destroying open window");
		destroyWindow();
	}
}

bool SDLService::createWindow(const char* title, int screen_width, int screen_height, uint32_t sdl_window_flags) {
	if (!win) {
		win = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_width, screen_height, sdl_window_flags);
		if (setDefaultWindowIcon) {
			setDefaultIcon();
		}
	} else {
		LOG_WARN("tried to create window while already existing. ignoring");
	}
	return win; // implicit conversion to bool
}

void SDLService::destroyWindow(void) {
	if (win) {
		SDL_DestroyWindow(win);
		win = nullptr;
	} else {
		LOG_WARN("tried to destroy window while no window exists. ignoring");
	}
}

std::pair<int, int> SDLService::getWindowSize(SDL_Window* win_ptr) {
	if (!win_ptr)
		win_ptr = win; // defaulting to currently managed window
	if (!win_ptr) {
		LOG_ERROR("cannot get size of nullptr window");
		return {0, 0};
	}
	int w, h;
	//SDL_GetWindowSize(win_ptr, &w, &h); // does not support hdpi
	SDL_GL_GetDrawableSize(win_ptr, &w, &h);
	return {w, h};
}

bool SDLService::createGLContext(SDL_Window* win_ptr) {
	if (!win_ptr)
		win_ptr = win; // defaulting to currently managed window

	if (!win_ptr) {
		LOG_ERROR("cannot create gl context with nullptr window");
		return false;
	}

	gl_context = SDL_GL_CreateContext(win_ptr);
	if (!gl_context) {
		LOG_ERROR("SDL_GL_CreateContext(): {}", SDL_GetError());
		return false;
	}

#ifndef MM_OPENGL_3_GLES
	// glad
	if (!gladLoadGLLoader((GLADloadproc) SDL_GL_GetProcAddress)) {
		LOG_ERROR("Failed to initialize OpenGL context");
		return false;
	}
#endif

	if (SDL_GL_MakeCurrent(win, gl_context)) {
		LOG_ERROR("SDL_GL_MakeCurrent(): {}", SDL_GetError());
		return false;
	}

	TracyGpuContext;

	LOG_INFO("gl: {}", glGetString(GL_VERSION));

	return true;
}

bool SDLService::createGLWindow(const char* title, int screen_width, int screen_height, uint32_t sdl_window_flags) {
#ifdef MM_OPENGL_3_GLES
	// opengl es 3.0
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#else
	//Use OpenGL 3.3 core
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
#endif
	if (!createWindow(title, screen_width, screen_height, SDL_WINDOW_OPENGL | sdl_window_flags)) {
		return false;
	}
	if (!createGLContext()) {
		return false;
	}
	return true;
}

void SDLService::processEvents(Engine& engine) {
	ZoneScoped;
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		if (e.type == SDL_QUIT) {
			engine.stop();
		} else {
			for (auto& h : _event_handler_list) {
				if ((*h)(e))
					break;
			}
		}
	}
}

SDLService::EventHandlerHandle SDLService::addEventHandler(std::function<bool(const SDL_Event&)> function) {
	if (!function) {
		// error
		return nullptr;
	}

	auto& r = _event_handler_list.emplace_back(std::make_unique<EventHandlerType>(function));

	return r.get();
}

void SDLService::removeEventHandler(EventHandlerHandle function_handle) {
	if (!function_handle) {
		// error
		return;
	}

	_event_handler_list.erase(
		std::remove_if(_event_handler_list.begin(), _event_handler_list.end(),
			[&](std::unique_ptr<EventHandlerType>& eh) { return function_handle == eh.get(); }
		),
		_event_handler_list.end()
	);
}


void SDLService::setDefaultIcon(void) {
	if (!win) {
		LOG_WARN("tried to set icon of NO window");
		return;
	}
	#include "../logo-f6c-square.png.h"

	setIcon(
		(void*)logo_f6c_square.pixel_data,
		logo_f6c_square.width,
		logo_f6c_square.height,
		logo_f6c_square.bytes_per_pixel
	);
}

void SDLService::setIcon(SDL_Surface* surf) {
	if (!win) {
		LOG_WARN("tried to set icon of NO window");
		return;
	}

	SDL_SetWindowIcon(win, surf);
}

void SDLService::setIcon(void* data, size_t width, size_t height, size_t bytes_per_pixel) {
	if (!win) {
		LOG_WARN("tried to set icon of NO window");
		return;
	}

	// thanks to Danial at https://blog.gibson.sh/2015/04/13/how-to-integrate-your-sdl2-window-icon-or-any-image-into-your-executable/
	uint32_t rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	int shift = (bytes_per_pixel == 3) ? 8 : 0;
	rmask = 0xff000000 >> shift;
	gmask = 0x00ff0000 >> shift;
	bmask = 0x0000ff00 >> shift;
	amask = 0x000000ff >> shift;
#else // little endian, like x86
	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = (bytes_per_pixel == 3) ? 0 : 0xff000000;
#endif

	SDL_Surface* icon = SDL_CreateRGBSurfaceFrom(
		data,
		width, height,
		bytes_per_pixel * 8,
		bytes_per_pixel * width,
		rmask, gmask, bmask, amask);

	setIcon(icon);

	SDL_FreeSurface(icon);
}

} // namespace MM::Services

