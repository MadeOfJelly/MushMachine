#include "./opengl_renderer.hpp"

#include <memory>
#include <mm/services/filesystem.hpp>
#include <mm/services/sdl_service.hpp>

#include <mm/opengl/fbo_builder.hpp>
#include <mm/opengl/frame_buffer_object.hpp>
#include <mm/resource_manager.hpp>
#include <mm/opengl/texture_loader.hpp>
#include "../opengl/res/default_texture.h" // data
#include "../opengl/res/errig_texture.h" // data
#include "mm/update_strategies/update_strategy.hpp"

#include <tracy/Tracy.hpp>
#ifndef MM_OPENGL_3_GLES
	#include <tracy/TracyOpenGL.hpp>
#else
	#define TracyGpuContext
	#define TracyGpuCollect
#endif


//#define LOGRS_SDL(x) LOGRS(std::string(SDL_GetError()) + x)
#include <mm/logger.hpp>
#define LOG_CRIT(...)		__LOG_CRIT(	"OpenGLRenderer", __VA_ARGS__)
#define LOG_ERROR(...)		__LOG_ERROR("OpenGLRenderer", __VA_ARGS__)
#define LOG_WARN(...)		__LOG_WARN(	"OpenGLRenderer", __VA_ARGS__)
#define LOG_INFO(...)		__LOG_INFO(	"OpenGLRenderer", __VA_ARGS__)
#define LOG_DEBUG(...)		__LOG_DEBUG("OpenGLRenderer", __VA_ARGS__)
#define LOG_TRACE(...)		__LOG_TRACE("OpenGLRenderer", __VA_ARGS__)


namespace MM::Services {

using namespace entt::literals;

OpenGLRenderer::OpenGLRenderer(void) {
	MM::Logger::initSectionLogger("OpenGL");
	MM::Logger::initSectionLogger("OpenGLRenderer");
}

OpenGLRenderer::~OpenGLRenderer(void) {
}

bool OpenGLRenderer::enable(Engine& engine, std::vector<UpdateStrategies::TaskInfo>& task_array) {
	if (!engine.tryService<SDLService>()) {
		LOG_ERROR("OpenGLRenderer requires SDLService");
		return false;
	}

	auto& sdl_s = engine.getService<SDLService>();

	if (!sdl_s.win) {
		if (!sdl_s.createGLWindow("MushMachine (OpenGLRenderer)", 800, 600)) {
			LOG_ERROR("couldn't create window with gl context");
			return false;
		}
	} else if (!sdl_s.gl_context) {
		if (!sdl_s.createGLContext()) {
			LOG_ERROR("couldn't create gl context");
			return false;
		}
	}

	// "creating" display fbo
	{
		targets["display"].reset(new OpenGL::FrameBufferObject);
		targets["display"]->_fboID = 0;
		targets["display"]->_resize = false; // its done for us
	}

	_sdl_event_handle = sdl_s.addEventHandler([this, &engine](const SDL_Event& e) -> bool {
		if (e.type == SDL_WINDOWEVENT) {
			if (e.window.event == SDL_WINDOWEVENT_RESIZED || e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
				auto& sdl = engine.getService<MM::Services::SDLService>();
				auto new_window_size = sdl.getWindowSize();
				glViewport(0, 0, new_window_size.first, new_window_size.second);

				// TODO: recreate fbos, dirvers seem to have problems otherwise
				//std::vector<std::pair<std::string,std::shared_ptr<MM::OpenGL::FrameBufferObject>>> new_fbos;

				for (auto& map_entry : targets) {
					if (map_entry.second->_resize) {
						auto& fbo = map_entry.second;
						float new_width = fbo->_resize_factor_width * new_window_size.first;
						float new_height = fbo->_resize_factor_height * new_window_size.second;

						for (auto& tex : fbo->_texAttachments) {
							if (tex->height == new_height && tex->width == new_width) {
								continue;
							}

							tex->resize(static_cast<int32_t>(new_width), static_cast<int32_t>(new_height));
						}

						//auto new_fbo = std::make_shared<MM::OpenGL::FrameBufferObject>();
						//auto new_fbo = OpenGL::FBOBuilder::start().
						//new_fbos.emplace_back(map_entry.first, new_fbo);
					}
				}

				//if (!new_fbos.empty()) {
				//}
			}
		}
		return false;
	});

	if (!_sdl_event_handle) {
		LOG_ERROR("couldn't add sdl event function!");
		return false;
	}

	{ // default texures
		auto& rm_t = MM::ResourceManager<MM::OpenGL::Texture>::ref();
		if (!rm_t.contains("default"_hs)) {
			if (!rm_t.load<MM::OpenGL::TextureLoaderConstBuffer>("default", default_png, default_png_len)) {
				LOG_WARN("couldn't load 'default' texture!");
			}
		}
		if (!rm_t.contains("errig"_hs)) {
			if (!rm_t.load<MM::OpenGL::TextureLoaderConstBuffer>("errig", errig_jpg, errig_jpg_len)) {
				LOG_WARN("couldn't load 'errig' texture!");
			}
		}
	}

	{ // add task
		task_array.push_back(
			UpdateStrategies::TaskInfo{"OpenGLRenderer::render"}
			.phase(UpdateStrategies::update_phase_t::POST)
			.fn([this](Engine& e){ this->render(e); })
		);
	}

	return true;
}

void OpenGLRenderer::disable(Engine&) {
	// TODO: do we need this??
	targets.clear();
	render_tasks.clear();

	// TODO: reallly?
	MM::ResourceManager<MM::OpenGL::Texture>::ref().clear();
}

void OpenGLRenderer::render(Engine& engine) {
	ZoneScopedN("MM::Services::OpenGLRenderer::render");

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	//targets["display"]->bind(FrameBufferObject::RW);
	//glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	//glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	targets["display"]->clear(0.1f, 0.1f, 0.1f, 1.0f, GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	for (auto& r : render_tasks) {
		r->render(*this, engine);
	}

	SDL_GL_SwapWindow(engine.getService<SDLService>().win);
	TracyGpuCollect;
}

} // namespace MM::Services

