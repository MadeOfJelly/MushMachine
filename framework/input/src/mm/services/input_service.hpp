#pragma once

#include "mm/services/service.hpp"
#include <glm/vec2.hpp>

#include <mm/services/sdl_service.hpp>

#include <array>

namespace MM::Input {
	using PlayerID = uint16_t;
} // MM::Input

namespace MM::Services {

	class InputService : public Service {
		public:
			// TODO: rework
			enum input_action_t {
				SPELL_WEAPON,
				SPELL_1,
				SPELL_2,
				SPELL_3,
				SPELL_4,
				SPELL_5,
				SPELL_6,
				USE,
				INPUT_ACTION_MAX
			};

			enum class k_view_mode_t {
				NO_VIEW, // disables any kplayer view processing
				USE_ARROWS, // TODO: implement
				MOUSE_DRAG,
				MOUSE_RELATIVE, // TODO: implement
				MOUSE_RELATIVE_TO_CENTER, // TODO: refactor
			};

			struct keyboard_mapping {
				SDL_Keycode move_up;
				SDL_Keycode move_down;
				SDL_Keycode move_left;
				SDL_Keycode move_right;

				SDL_Keycode actions[INPUT_ACTION_MAX];

				input_action_t get(SDL_Keycode key);
			};

			struct controller_mapping {
				SDL_GameControllerButton actions[INPUT_ACTION_MAX];

				input_action_t get(SDL_GameControllerButton key);
			};

		private:
			struct player_input_action_state {

				bool is_controller;
				SDL_JoystickID controller_instance_id;
				union {
					struct {
						int16_t left_x;
						int16_t left_y;

						int16_t right_x;
						int16_t right_y;
					} controller;
					struct {
						bool up;
						bool down;
						bool left;
						bool right;

						bool mouse_button_down; // for MOUSE_DRAG

						int32_t last_x, last_y;
						int32_t curr_x, curr_y;

						k_view_mode_t view_mode;
					} keyboard;
				} u; // union

				bool active[INPUT_ACTION_MAX] = {false};

				float move_force = 0.f;
				float move_dir = 0.f;
				float view_force = 0.f;
				float view_dir = 0.f;
			};

		public: // HACK: proper methods
			keyboard_mapping	_k_map;
			controller_mapping	_c_map;

		private:
			// TODO: max player
			static const Input::PlayerID max_player_id = 4;
			std::array<player_input_action_state, max_player_id> _player;
			std::array<bool, max_player_id> _player_active;

		public:
			InputService(void);
			~InputService(void);

		private:
			SDLService::EventHandlerHandle _event_handle = nullptr;

		public:
			bool enable(Engine& engine, std::vector<UpdateStrategies::TaskInfo>& task_array) override;
			void disable(Engine& engine) override;

			const char* name(void) override { return "InputService"; }

		public:
			// returns true if event was relevant
			bool handleSDL_Event(const SDL_Event& e, MM::Engine& engine);

			Input::PlayerID addPlayer(bool keyboard, SDL_JoystickID controller_id);

			bool setKPlayerViewMode(Input::PlayerID id, k_view_mode_t mode);

			// returns UINT16_MAX on error
			Input::PlayerID findPlayerState(bool controller, SDL_JoystickID controller_id);

			// check if action is set
			bool get(Input::PlayerID id, input_action_t action);
			const char* getKeyString(Input::PlayerID id, input_action_t action);

			float getMoveForce(Input::PlayerID id);
			float getViewForce(Input::PlayerID id);

			glm::vec2 getMoveVec(Input::PlayerID id);
			glm::vec2 getViewVec(Input::PlayerID id);

			// returns values [0, 2PI)
			float getMoveDir(Input::PlayerID id);

			// returns values [0, 2PI)
			float getViewDir(Input::PlayerID id);

		private:
			SDL_Keycode					loadKey(const std::string& name, const SDL_Keycode default_key);
			SDL_GameControllerButton	loadButton(const std::string& name, const SDL_GameControllerButton default_button);


			void updateKPlayerDirs(player_input_action_state& p);
			void updateCPlayerDirs(player_input_action_state& p);
	};
} // namespace MM::Services

