#include <gtest/gtest.h>

#include <mm/engine.hpp>
#include <mm/services/sdl_service.hpp>

TEST(sdl_service, add_en_dis) {
	MM::Engine engine;

	engine.addService<MM::Services::SDLService>();
	ASSERT_TRUE(engine.enableService<MM::Services::SDLService>());

	auto* sdl_ss_ptr = engine.tryService<MM::Services::SDLService>();
	ASSERT_NE(sdl_ss_ptr, nullptr);

	ASSERT_EQ(sdl_ss_ptr->win, nullptr);

	engine.disableService<MM::Services::SDLService>();
}

TEST(sdl_service, window_basic) {
	MM::Engine engine;

	engine.addService<MM::Services::SDLService>();
	ASSERT_TRUE(engine.enableService<MM::Services::SDLService>());

	auto* sdl_ss_ptr = engine.tryService<MM::Services::SDLService>();
	ASSERT_NE(sdl_ss_ptr, nullptr);

	ASSERT_EQ(sdl_ss_ptr->win, nullptr);
	ASSERT_TRUE(sdl_ss_ptr->createWindow("test", 800, 600));
	ASSERT_NE(sdl_ss_ptr->win, nullptr);

	engine.disableService<MM::Services::SDLService>();

	ASSERT_EQ(sdl_ss_ptr->win, nullptr);
}

TEST(sdl_service, window_opengl_3_3_core) {
	MM::Engine engine;

	engine.addService<MM::Services::SDLService>();
	ASSERT_TRUE(engine.enableService<MM::Services::SDLService>());

	auto* sdl_ss_ptr = engine.tryService<MM::Services::SDLService>();
	ASSERT_NE(sdl_ss_ptr, nullptr);

	ASSERT_EQ(sdl_ss_ptr->win, nullptr);

	//Use OpenGL 3.3 core
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	ASSERT_TRUE(sdl_ss_ptr->createWindow("test", 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN));
	ASSERT_NE(sdl_ss_ptr->win, nullptr);

	ASSERT_EQ(sdl_ss_ptr->gl_context, nullptr);
	ASSERT_TRUE(sdl_ss_ptr->createGLContext());
	ASSERT_NE(sdl_ss_ptr->gl_context, nullptr);

	engine.disableService<MM::Services::SDLService>();

	ASSERT_EQ(sdl_ss_ptr->win, nullptr);
}

TEST(sdl_service, window_opengl_3_3_core_single_call) {
	MM::Engine engine;

	engine.addService<MM::Services::SDLService>();
	ASSERT_TRUE(engine.enableService<MM::Services::SDLService>());

	auto* sdl_ss_ptr = engine.tryService<MM::Services::SDLService>();
	ASSERT_NE(sdl_ss_ptr, nullptr);

	ASSERT_EQ(sdl_ss_ptr->win, nullptr);
	ASSERT_EQ(sdl_ss_ptr->gl_context, nullptr);

	ASSERT_TRUE(sdl_ss_ptr->createGLWindow("test", 800, 600));

	ASSERT_NE(sdl_ss_ptr->win, nullptr);
	ASSERT_NE(sdl_ss_ptr->gl_context, nullptr);

	engine.disableService<MM::Services::SDLService>();

	ASSERT_EQ(sdl_ss_ptr->win, nullptr);
}

TEST(sdl_service, event_handle_reg) {
	MM::Engine engine;

	engine.addService<MM::Services::SDLService>();
	ASSERT_TRUE(engine.enableService<MM::Services::SDLService>());

	auto* sdl_ss_ptr = engine.tryService<MM::Services::SDLService>();
	ASSERT_NE(sdl_ss_ptr, nullptr);

	// produces fake "1" press
	auto event_forge_f = [](MM::Engine&) {
		SDL_Event sdlevent;
		sdlevent.type = SDL_KEYDOWN;
		sdlevent.key.keysym.sym = SDLK_1;

		ASSERT_EQ(SDL_PushEvent(&sdlevent), 1);
	};

	// register forge
	engine.getUpdateStrategy().addDeferred(event_forge_f);

	// register sdl event handler to just stop the engine
	auto* stop_event_hook_h = sdl_ss_ptr->addEventHandler([&engine](const SDL_Event&){ engine.stop(); return true; });
	ASSERT_NE(stop_event_hook_h, nullptr);

	engine.run();

	engine.disableService<MM::Services::SDLService>();
}

