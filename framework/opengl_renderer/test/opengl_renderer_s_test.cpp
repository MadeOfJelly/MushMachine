#include <gtest/gtest.h>

#include <mm/engine.hpp>
#include <mm/services/sdl_service.hpp>
#include <mm/services/opengl_renderer.hpp>

TEST(opengl_renderer_s, basic) {
	MM::Engine engine;

	engine.addService<MM::Services::SDLService>();
	ASSERT_TRUE(engine.enableService<MM::Services::SDLService>());

	engine.addService<MM::Services::OpenGLRenderer>();
	ASSERT_TRUE(engine.enableService<MM::Services::OpenGLRenderer>());

	engine.update();
}

int main(int argc, char** argv) {
	//argv0 = argv[0];

	::testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}

