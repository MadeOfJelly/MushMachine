#include <gtest/gtest.h>

#include <mm/engine.hpp>

#include <mm/services/input_service.hpp>
#include <mm/services/sdl_service.hpp>

TEST(input_service, add_en_dis) {
	MM::Engine engine;

	// sdl dep
	{
		engine.addService<MM::Services::SDLService>();
		ASSERT_TRUE(engine.enableService<MM::Services::SDLService>());

		auto* sdl_ss_ptr = engine.tryService<MM::Services::SDLService>();
		ASSERT_NE(sdl_ss_ptr, nullptr);
	}

	engine.addService<MM::Services::InputService>();
	ASSERT_TRUE(engine.enableService<MM::Services::InputService>());

	auto* input_service_ptr = engine.tryService<MM::Services::InputService>();
	ASSERT_NE(input_service_ptr, nullptr);

	////

	engine.disableService<MM::Services::InputService>();

	engine.disableService<MM::Services::SDLService>();
}

TEST(input_service, initial_error) {
	MM::Engine engine;

	// sdl dep
	{
		engine.addService<MM::Services::SDLService>();
		ASSERT_TRUE(engine.enableService<MM::Services::SDLService>());

		auto* sdl_ss_ptr = engine.tryService<MM::Services::SDLService>();
		ASSERT_NE(sdl_ss_ptr, nullptr);
	}

	engine.addService<MM::Services::InputService>();
	ASSERT_TRUE(engine.enableService<MM::Services::InputService>());

	auto* input_service_ptr = engine.tryService<MM::Services::InputService>();
	ASSERT_NE(input_service_ptr, nullptr);

	auto& input = *input_service_ptr;

	// actions (spells)
	{
		for (unsigned int action = 0; action < MM::Services::InputService::INPUT_ACTION_MAX; action++) {
			ASSERT_FALSE(input.get(0, (MM::Services::InputService::input_action_t)action));
		}

		ASSERT_FALSE(input.get(2, MM::Services::InputService::SPELL_1));
	}

	// move and view
	{
		{
			auto m = input.getMoveVec(0);
			ASSERT_EQ(m.x, 0.f);
			ASSERT_EQ(m.y, 0.f);
		}

		{
			auto v = input.getViewVec(0);
			ASSERT_EQ(v.x, 0.f);
			ASSERT_EQ(v.y, 0.f);
		}
	}
}

TEST(input_service, adding_players) {
	MM::Engine engine;

	// sdl dep
	{
		engine.addService<MM::Services::SDLService>();
		ASSERT_TRUE(engine.enableService<MM::Services::SDLService>());

		auto* sdl_ss_ptr = engine.tryService<MM::Services::SDLService>();
		ASSERT_NE(sdl_ss_ptr, nullptr);
	}

	engine.addService<MM::Services::InputService>();
	ASSERT_TRUE(engine.enableService<MM::Services::InputService>());

	auto* input_service_ptr = engine.tryService<MM::Services::InputService>();
	ASSERT_NE(input_service_ptr, nullptr);

	auto& input = *input_service_ptr;

	MM::Input::PlayerID p1_id = input.addPlayer(true, 0);
	ASSERT_NE(p1_id, UINT16_MAX);

	{
		// TODO: check
		//ASSERT_EQ(input.addPlayer(true, 0), UINT16_MAX);
	}

	// actions (spells)
	{
		for (unsigned int action = 0; action < MM::Services::InputService::INPUT_ACTION_MAX; action++) {
			ASSERT_FALSE(input.get(p1_id, (MM::Services::InputService::input_action_t)action));
		}
	}

	// move and view
	{
		{
			auto m = input.getMoveVec(p1_id);
			ASSERT_EQ(m.x, 0.f);
			ASSERT_EQ(m.y, 0.f);
		}

		{
			// never (0, 0)
			auto v = input.getViewVec(p1_id);
			ASSERT_EQ(v.x, 1.f);
			ASSERT_EQ(v.y, 0.f);
		}
	}
}

TEST(input_service, basic_handling_keyboard) {
	MM::Engine engine;

	// sdl dep
	{
		engine.addService<MM::Services::SDLService>();
		ASSERT_TRUE(engine.enableService<MM::Services::SDLService>());

		auto* sdl_ss_ptr = engine.tryService<MM::Services::SDLService>();
		ASSERT_NE(sdl_ss_ptr, nullptr);
	}

	engine.addService<MM::Services::InputService>();
	ASSERT_TRUE(engine.enableService<MM::Services::InputService>());

	auto* input_service_ptr = engine.tryService<MM::Services::InputService>();
	ASSERT_NE(input_service_ptr, nullptr);

	auto& input = *input_service_ptr;

	MM::Input::PlayerID p_id = input.addPlayer(true, 0);
	ASSERT_NE(p_id, UINT16_MAX);

	// actions (spells)
	{
		for (unsigned int action = 0; action < MM::Services::InputService::INPUT_ACTION_MAX; action++) {
			ASSERT_FALSE(input.get(p_id, (MM::Services::InputService::input_action_t)action));
		}

		// forging sdl event spell 1
		{
			SDL_Event tmp_e;
			tmp_e.key.keysym.sym = SDLK_1;
			tmp_e.type = SDL_KEYDOWN;

			ASSERT_TRUE(input.handleSDL_Event(tmp_e, engine));
		}


		ASSERT_FALSE(input.get(p_id, MM::Services::InputService::SPELL_WEAPON));
		ASSERT_TRUE(input.get(p_id, MM::Services::InputService::SPELL_1));
		ASSERT_FALSE(input.get(p_id, MM::Services::InputService::SPELL_2));
		ASSERT_FALSE(input.get(p_id, MM::Services::InputService::SPELL_3));
		ASSERT_FALSE(input.get(p_id, MM::Services::InputService::SPELL_4));
		ASSERT_FALSE(input.get(p_id, MM::Services::InputService::SPELL_5));
		ASSERT_FALSE(input.get(p_id, MM::Services::InputService::SPELL_6));
		ASSERT_FALSE(input.get(p_id, MM::Services::InputService::USE));

		// forging sdl event spell weapon
		{
			SDL_Event tmp_e;
			tmp_e.key.keysym.sym = SDLK_SPACE;
			tmp_e.type = SDL_KEYDOWN;

			ASSERT_TRUE(input.handleSDL_Event(tmp_e, engine));
		}

		ASSERT_TRUE(input.get(p_id, MM::Services::InputService::SPELL_WEAPON));
		ASSERT_TRUE(input.get(p_id, MM::Services::InputService::SPELL_1));
		ASSERT_FALSE(input.get(p_id, MM::Services::InputService::SPELL_2));
		ASSERT_FALSE(input.get(p_id, MM::Services::InputService::SPELL_3));
		ASSERT_FALSE(input.get(p_id, MM::Services::InputService::SPELL_4));
		ASSERT_FALSE(input.get(p_id, MM::Services::InputService::SPELL_5));
		ASSERT_FALSE(input.get(p_id, MM::Services::InputService::SPELL_6));
		ASSERT_FALSE(input.get(p_id, MM::Services::InputService::USE));

		// forging sdl event spell 1 end
		{
			SDL_Event tmp_e;
			tmp_e.key.keysym.sym = SDLK_1;
			tmp_e.type = SDL_KEYUP;

			ASSERT_TRUE(input.handleSDL_Event(tmp_e, engine));
		}


		ASSERT_TRUE(input.get(p_id, MM::Services::InputService::SPELL_WEAPON));
		ASSERT_FALSE(input.get(p_id, MM::Services::InputService::SPELL_1));
		ASSERT_FALSE(input.get(p_id, MM::Services::InputService::SPELL_2));
		ASSERT_FALSE(input.get(p_id, MM::Services::InputService::SPELL_3));
		ASSERT_FALSE(input.get(p_id, MM::Services::InputService::SPELL_4));
		ASSERT_FALSE(input.get(p_id, MM::Services::InputService::SPELL_5));
		ASSERT_FALSE(input.get(p_id, MM::Services::InputService::SPELL_6));
		ASSERT_FALSE(input.get(p_id, MM::Services::InputService::USE));
	}
}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

