# Basic Engine Usage

## General Headers

```c++
#include <mm/engine.hpp>
// or
#include <mm/engine_fwd.hpp> // forward header
```

# Engine Example Setup

```c++
#include <mm/engine.hpp>

// Services you want to use:
// I recommend to do this in a seperate translation unit, for good mesure.
#include <mm/services/sdl_service.hpp>
#include <mm/services/filesystem.hpp> // requires SDL2 if not headless.
#include <mm/services/opengl_renderer.hpp> // requires sdl_service.
#include <mm/services/imgui_s.hpp> // every one <3 DearImGui!
#include <mm/services/simple_scene.hpp> // a simple ECS using EnTT.
#include <mm/services/sound_service.hpp> // SoLoud for Sound!
#include <mm/services/screen_director_simple.hpp> // you want this, go to screen director doc for more info.

MM::Engine engine; // at global scope, only required for emscripten.

// adds Screens, definition at EOF.
void add_screens(MM::Engine& engine, MM::Services::ScreenDirectorSimple& sds);

// argc, argv required for sdl2 on windows.
int main(int argc, char** argv) {
	{ // setup ALL Services ever used during runtime.
		// 1. setup SDL
		// this adds the SDLService to the engine, also returns a ref to the Service.
		auto& sdl_ss = engine.addService<MM::Services::SDLService>(
			SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS | SDL_INIT_GAMECONTROLLER
		); // emscripten does not like haptic ...

		// this Enables the Service (calles service.enable()), THIS also returns if the enable was successfull!!, for simplicity omitted...
		engine.enableService<MM::Services::SDLService>();

		// this creates a OpenGL(3.3) ready Window.
		sdl_ss.createGLWindow("My MM Game", 1280, 720);
		// set icon here if needed.


		// 2. filesystem, can be 1. too.
		// the Construction of the fs Service takes in to arguments:
		// - the path to the executable, it should be the first string in argv, but can be left empty.
		// - the name of the application, of sorts. it is used for the folder to write into, so saves and configs edited by the player get written to this directory. it is not located relative to the exe/install.
		engine.addService<MM::Services::FilesystemService>(argv[0], "my_mm_game");

		// and immediatly enable.
		engine.enableService<MM::Services::FilesystemService>();

		// 3. add the the remaining Services, which get enabled, disabled during runtime, by eg. ScreenDirector.
		engine.addService<MM::Services::ImGuiService>();
		engine.addService<MM::Services::SimpleSceneService>();
		engine.addService<MM::Services::SoundService>(); // we also could just enable this at setup
		engine.addService<MM::Services::OpenGLRenderer>();

		// 4. and not add a ScreenDirector, which manages Screens, which enable/disable a set of Services.
		auto& sds = engine.addService<MM::Services::ScreenDirectorSimple>();

		// add/setup screens, i really recommend doing this somewhere else.
		add_screens(engine, sds);

		// also queue the first Screen to be enabled.
		sds.queued_screen_id = "MM::Screens::mm_logo";

		// and enable, this also switches to the queued screen, if there is one.
		engine.enableService<MM::Services::ScreenDirectorSimple>();
	}

	// run Engine
	engine.run(); // blocks
	// you can also call engine.update() and engine.fixedUpdate() by hand.

	// calling engine.stop() from anywhere will make engine.run() return.

	engine.cleanup(); // required for global scope engine. don't do this for emscripten, bc emsripten will leave main() on startup, since you can't have blocking JavaSript...
	return 0;
}

void add_screens(MM::Engine& engine, MM::Services::ScreenDirectorSimple& sds) {
	// add built-in MM-Logo-Splash-Screen
	MM::Screens::create_mm_logo(
		engine,
		sds.screens["MM::Screens::mm_logo"],
		"MM::Screens::mm_logo", // mm_logo specific: a Screen to play after mm_logo has finished. (we loop in this example)
	);

	// add additional Screens here.
}
```
