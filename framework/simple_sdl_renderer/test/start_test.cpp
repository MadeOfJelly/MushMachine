#include <gtest/gtest.h>

#include <mm/engine.hpp>

#include <mm/services/sdl_service.hpp>
#include <mm/services/simple_sdl_renderer.hpp>

TEST(simple_sdl_renderer, basic) {
	srand(1);
	MM::Engine engine;

	// sdl dep
	{
		engine.addService<MM::Services::SDLService>();
		ASSERT_TRUE(engine.enableService<MM::Services::SDLService>());
	}

	auto& ren = engine.addService<MM::Services::SimpleSDLRendererService>();
	ASSERT_TRUE(engine.enableService<MM::Services::SimpleSDLRendererService>());

	ren.processors.emplace_back([](auto& r, auto&) {
			r.targets["display"].set(r.renderer);

			SDL_Rect rect {
				rand() % 700,
				rand() % 500,
				100,
				100
			};

			SDL_SetRenderDrawColor(r.renderer, 255, 0, 0, 255);
			SDL_RenderFillRect(r.renderer, &rect);
		}
	);

	engine.run();
}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

