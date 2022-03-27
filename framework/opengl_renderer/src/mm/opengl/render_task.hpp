#pragma once

#include <vector>

// fwd
namespace MM {
	class Engine;
	namespace Services {
		class OpenGLRenderer;
	}
}

#ifdef MM_OPENGL_3_GLES
#define GLSL_VERSION_STRING "#version 300 es\n"
#else
#define GLSL_VERSION_STRING "#version 330 core\n"
#endif

namespace MM::OpenGL {

	class RenderTask {
		public:
			virtual ~RenderTask(void) = default;

			virtual const char* name(void) = 0;//{ return "NoName"; };

			virtual void render(Services::OpenGLRenderer& rs, Engine& engine) = 0;

			// a place to reload/compile shaders etc.
			//virtual void reload(void) {} // TODO: remove
			//virtual std::vector<const char*> getShaderPaths(void) {return {};} // TODO: remove
	};
} // MM:OpenGL

