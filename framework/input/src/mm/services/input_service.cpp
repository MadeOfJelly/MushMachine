#include "./input_service.hpp"

#include <entt/core/hashed_string.hpp>

#include <glm/common.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtc/constants.hpp>

#include <tracy/Tracy.hpp>

#include <mm/logger.hpp>
#define LOG_CRIT(...)		__LOG_CRIT(	"InputService", __VA_ARGS__)
#define LOG_ERROR(...)		__LOG_ERROR("InputService", __VA_ARGS__)
#define LOG_WARN(...)		__LOG_WARN(	"InputService", __VA_ARGS__)
#define LOG_INFO(...)		__LOG_INFO(	"InputService", __VA_ARGS__)
#define LOG_DEBUG(...)		__LOG_DEBUG("InputService", __VA_ARGS__)
#define LOG_TRACE(...)		__LOG_TRACE("InputService", __VA_ARGS__)

namespace MM::Services {

InputService::input_action_t InputService::keyboard_mapping::get(SDL_Keycode key) {
	for (unsigned int i = 0; i < InputService::input_action_t::INPUT_ACTION_MAX; i++) {
		if (actions[i] == key)
			return (InputService::input_action_t)i;
	}

	return INPUT_ACTION_MAX;
}

InputService::input_action_t InputService::controller_mapping::get(SDL_GameControllerButton button) {
	for (unsigned int i = 0; i < InputService::input_action_t::INPUT_ACTION_MAX; i++) {
		if (actions[i] == button)
			return (InputService::input_action_t)i;
	}

	return INPUT_ACTION_MAX;
}

InputService::InputService(void) {
	MM::Logger::initSectionLogger("InputService");
	LOG_TRACE("constructing InputService...");

	// TODO: integrate with config system
	_k_map.move_up =	loadKey("keyboard_move_up",		SDLK_w);
	_k_map.move_down =	loadKey("keyboard_move_down",	SDLK_s);
	_k_map.move_left =	loadKey("keyboard_move_left",	SDLK_a);
	_k_map.move_right =	loadKey("keyboard_move_right",	SDLK_d);

	_k_map.actions[SPELL_WEAPON] = loadKey("keyboard_spell_weapon", SDLK_SPACE);

	_k_map.actions[SPELL_1] = loadKey("keyboard_spell_1",		SDLK_1);
	_k_map.actions[SPELL_2] = loadKey("keyboard_spell_2",		SDLK_2);
	_k_map.actions[SPELL_3] = loadKey("keyboard_spell_3",		SDLK_3);
	_k_map.actions[SPELL_4] = loadKey("keyboard_spell_4",		SDLK_4);
	_k_map.actions[SPELL_5] = loadKey("keyboard_spell_5",		SDLK_5);
	_k_map.actions[SPELL_6] = loadKey("keyboard_spell_6",		SDLK_6);

	_k_map.actions[USE] = loadKey("keyboard_use",				SDLK_f);

	_c_map.actions[SPELL_WEAPON] = loadButton("controller_spell_weapon", SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);

	_c_map.actions[SPELL_1] = loadButton("controller_spell_1",	SDL_CONTROLLER_BUTTON_LEFTSHOULDER);
	_c_map.actions[SPELL_2] = loadButton("controller_spell_2",	SDL_CONTROLLER_BUTTON_A);
	_c_map.actions[SPELL_3] = loadButton("controller_spell_3",	SDL_CONTROLLER_BUTTON_DPAD_UP);
	_c_map.actions[SPELL_4] = loadButton("controller_spell_4",	SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
	_c_map.actions[SPELL_5] = loadButton("controller_spell_5",	SDL_CONTROLLER_BUTTON_DPAD_DOWN);
	_c_map.actions[SPELL_6] = loadButton("controller_spell_6",	SDL_CONTROLLER_BUTTON_DPAD_LEFT);

	_c_map.actions[USE] = loadButton("controller_use",	SDL_CONTROLLER_BUTTON_X);


	for (auto& it : _player_active)
		it = false;
}

InputService::~InputService(void) {
}

bool InputService::enable(Engine& engine) {
	auto* sdl_ss = engine.tryService<SDLService>();
	if (!sdl_ss) {
		LOG_ERROR("InputService requires SDLService in engine!");
		return false;
	}

	_event_handle = sdl_ss->addEventHandler([this, &engine](const SDL_Event& e) -> bool { return this->handleSDL_Event(e, engine); });
	if (!_event_handle) {
		LOG_ERROR("couldnt register event handler!");
		return false;
	}

	return true;
}

void InputService::disable(Engine& engine) {
	if (_event_handle) {
		auto* sdl_ss = engine.tryService<SDLService>();
		if (!sdl_ss) {
			// error?
			return;
		}

		sdl_ss->removeEventHandler(_event_handle);
	}
}

std::vector<UpdateStrategies::UpdateCreationInfo> InputService::registerUpdates(void) {
	using namespace entt::literals;
	return {
		{
			"InputService::update"_hs,
			"InputService::update",
			[this](MM::Engine&) {
				for (size_t i = 0; i < 4; i++) {
					if (!_player_active[i])
						continue;

					auto& p = _player[i];
					if (p.is_controller) {
					} else {
						updateKPlayerDirs(p);
					}
				}
			}
		}
	};
}

bool InputService::handleSDL_Event(const SDL_Event& e, MM::Engine& engine) {
	ZoneScopedN("MM::InputService::handleSDL_Event");
	switch (e.type) {
		case SDL_KEYDOWN:
			{
				auto p_id = findPlayerState(false, 0);
				if (p_id == UINT16_MAX)
					break;

				auto& p = _player[p_id];

				input_action_t a = _k_map.get(e.key.keysym.sym);
				if (a != INPUT_ACTION_MAX) {
					p.active[a] = true;
					return true;
				} else if (e.key.keysym.sym == _k_map.move_up) {
					p.u.keyboard.up = true;
					//updateKPlayerDirs(p);
					return true;
				} else if (e.key.keysym.sym == _k_map.move_down) {
					p.u.keyboard.down = true;
					//updateKPlayerDirs(p);
					return true;
				} else if (e.key.keysym.sym == _k_map.move_left) {
					p.u.keyboard.left = true;
					//updateKPlayerDirs(p);
					return true;
				} else if (e.key.keysym.sym == _k_map.move_right) {
					p.u.keyboard.right = true;
					//updateKPlayerDirs(p);
					return true;
				} else if (e.key.keysym.sym == SDLK_ESCAPE) {
					if (p.u.keyboard.view_mode == k_view_mode_t::MOUSE_RELATIVE) {
						p.u.keyboard.mouse_button_down = false;
						SDL_SetRelativeMouseMode(SDL_FALSE);
						return true;
					}
				}
			}
			break;
		case SDL_KEYUP:
			{
				auto p_id = findPlayerState(false, 0);
				if (p_id == UINT16_MAX)
					break;

				auto& p = _player[p_id];

				input_action_t a = _k_map.get(e.key.keysym.sym);
				if (a != INPUT_ACTION_MAX) {
					p.active[a] = false;
					return true;
				} else if (e.key.keysym.sym == _k_map.move_up) {
					p.u.keyboard.up = false;
					//updateKPlayerDirs(p);
					return true;
				} else if (e.key.keysym.sym == _k_map.move_down) {
					p.u.keyboard.down = false;
					//updateKPlayerDirs(p);
					return true;
				} else if (e.key.keysym.sym == _k_map.move_left) {
					p.u.keyboard.left = false;
					//updateKPlayerDirs(p);
					return true;
				} else if (e.key.keysym.sym == _k_map.move_right) {
					p.u.keyboard.right = false;
					//updateKPlayerDirs(p);
					return true;
				}
			}
			break;
		case SDL_MOUSEMOTION:
			{
				auto p_id = findPlayerState(false, 0);
				if (p_id == UINT16_MAX)
					break;

				auto* sdl_ss = engine.tryService<MM::Services::SDLService>();
				auto[win_x, win_y] = sdl_ss->getWindowSize();
				auto& p = _player[p_id];

				if (p.u.keyboard.view_mode == k_view_mode_t::MOUSE_RELATIVE) {
					p.u.keyboard.curr_x += e.motion.xrel;
					p.u.keyboard.curr_y += e.motion.yrel;
				} else {
					p.u.keyboard.curr_x = e.motion.x;
					p.u.keyboard.curr_y = e.motion.y;
				}

				if (p.u.keyboard.view_mode == k_view_mode_t::MOUSE_RELATIVE_TO_CENTER) {
					glm::vec2 mouse_vec{e.motion.x - win_x/2., -(e.motion.y - win_y/2.)};

					//LOGIS("mouse vec x:" + std::to_string(mouse_vec.x) + " y:" + std::to_string(mouse_vec.y));

					p.view_dir = glm::mod(glm::atan(mouse_vec.y, mouse_vec.x) + 2*glm::pi<float>(), 2*glm::pi<float>());
					p.view_force = glm::length(mouse_vec) * 0.01f;
				}
			}
			break;
		case SDL_MOUSEBUTTONDOWN:
			{
				auto p_id = findPlayerState(false, 0);
				if (p_id == UINT16_MAX)
					break;

				if (e.button.button == SDL_BUTTON_LEFT) {
					auto& p = _player[p_id];
					if (p.u.keyboard.view_mode == k_view_mode_t::MOUSE_DRAG || p.u.keyboard.view_mode == k_view_mode_t::MOUSE_RELATIVE) {
						p.u.keyboard.mouse_button_down = true;
						if (p.u.keyboard.view_mode == k_view_mode_t::MOUSE_RELATIVE) {
							SDL_SetHint(SDL_HINT_MOUSE_RELATIVE_MODE_WARP, "1"); // not needed if fullscreen
							SDL_SetRelativeMouseMode(SDL_TRUE);
						}
					}
				}
			}
			break;
		case SDL_MOUSEBUTTONUP:
			{
				auto p_id = findPlayerState(false, 0);
				if (p_id == UINT16_MAX)
					break;

				if (e.button.button == SDL_BUTTON_LEFT) {
					auto& p = _player[p_id];
					if (p.u.keyboard.view_mode == k_view_mode_t::MOUSE_DRAG) {
						p.u.keyboard.mouse_button_down = false;
					}
				}
			}
			break;
		case SDL_CONTROLLERAXISMOTION:
			{
				auto p_id = findPlayerState(true, e.caxis.which);
				if (p_id == UINT16_MAX)
					break;

				auto& p = _player[p_id];
				switch (e.caxis.axis) {
					case SDL_CONTROLLER_AXIS_LEFTX:
						p.u.controller.left_x = e.caxis.value;
						updateCPlayerDirs(p);
						return true;
					case SDL_CONTROLLER_AXIS_LEFTY:
						p.u.controller.left_y = e.caxis.value;
						updateCPlayerDirs(p);
						return true;
					case SDL_CONTROLLER_AXIS_RIGHTX:
						p.u.controller.right_x = e.caxis.value;
						updateCPlayerDirs(p);
						return true;
					case SDL_CONTROLLER_AXIS_RIGHTY:
						p.u.controller.right_y = e.caxis.value;
						updateCPlayerDirs(p);
						return true;
				}
			}
			break;
		case SDL_CONTROLLERBUTTONDOWN:
			{
				auto p_id = findPlayerState(true, e.caxis.which);
				if (p_id == UINT16_MAX)
					break;

				auto& p = _player[p_id];

				input_action_t a = _c_map.get((SDL_GameControllerButton)e.cbutton.button);
				if (a != INPUT_ACTION_MAX) {
					p.active[a] = true;
					return true;
				}
			}
			break;
		case SDL_CONTROLLERBUTTONUP:
			{
				auto p_id = findPlayerState(true, e.caxis.which);
				if (p_id == UINT16_MAX)
					break;

				auto& p = _player[p_id];

				input_action_t a = _c_map.get((SDL_GameControllerButton)e.cbutton.button);
				if (a != INPUT_ACTION_MAX) {
					p.active[a] = false;
					return true;
				}
			}
			break;
	}

	return false;
}

Input::PlayerID InputService::addPlayer(bool keyboard, SDL_JoystickID controller_id) {
	Input::PlayerID p_id = UINT16_MAX;
	for (Input::PlayerID i = 0; i < max_player_id; i++) {
		if (!_player_active[i]) {
			p_id = i;
			break;
		}
	}

	if (p_id == UINT16_MAX) {
		LOG_ERROR("adding player, all player slots active");
		return p_id;
	}

	LOG_DEBUG("added new player with id {}", p_id);

	_player_active[p_id] = true;
	// TODO: reset player

	auto& p = _player[p_id];
	p.is_controller = !keyboard;
	p.controller_instance_id = controller_id;

	// initialize player
	if (p.is_controller) {
		p.u.controller = {0, 0, 0, 0};
	} else {
		p.u.keyboard = {
			false, false, false, false,
			false,
			0, 0,
			0, 0,
			k_view_mode_t::NO_VIEW,
		};
	}

	return p_id;
}

bool InputService::get(Input::PlayerID id, InputService::input_action_t action) {
	assert(id < max_player_id);
	if (!_player_active[id]) {
		LOG_WARN("use of inactive player {}", id);
		return false;
	}

	return _player[id].active[action];
}

const char* InputService::getKeyString(Input::PlayerID id, InputService::input_action_t action) {
	assert(id < max_player_id);
	if (!_player_active[id]) {
		LOG_WARN("use of inactive player {}", id);
		return "INACTIVE_PLAYER";
	}

	if (_player[id].is_controller) {
		return SDL_GameControllerGetStringForButton(_c_map.actions[action]);
	} else {
		return SDL_GetKeyName(_k_map.actions[action]);
	}

	//return _player[id].active[action];
	return "UNK";
}

float InputService::getMoveForce(Input::PlayerID id) {
	assert(id < max_player_id);
	if (!_player_active[id]) {
		LOG_WARN("use of inactive player {}", id);
		return 0.f;
	}

	if (_player[id].move_force <= 0.01f)
		return 0.f;

	return _player[id].move_force;

}

float InputService::getViewForce(Input::PlayerID id) {
	assert(id < max_player_id);
	if (!_player_active[id]) {
		LOG_WARN("use of inactive player {}", id);
		return 0.f;
	}

	if (_player[id].view_force <= 0.01f)
		return 0.f;

	return _player[id].view_force;

}

glm::vec2 InputService::getMoveVec(Input::PlayerID id) {
	assert(id < max_player_id);
	if (!_player_active[id]) {
		LOG_WARN("use of inactive player {}", id);
		return {0.f, 0.f};
	}

	return {
		glm::cos(_player[id].move_dir) * getMoveForce(id),
		-glm::sin(_player[id].move_dir) * getMoveForce(id)
	};
}

glm::vec2 InputService::getViewVec(Input::PlayerID id) {
	assert(id < max_player_id);
	if (!_player_active[id]) {
		LOG_WARN("use of inactive player {}", id);
		return {0.f, 0.f};
	}

	return {
		glm::cos(_player[id].view_dir),
		-glm::sin(_player[id].view_dir)
	};
}

float InputService::getMoveDir(Input::PlayerID id) {
	assert(id < max_player_id);
	if (!_player_active[id]) {
		LOG_WARN("use of inactive player {}", id);
		return 0.f;
	}

	return glm::mod(_player[id].move_dir + 2*glm::pi<float>(), 2*glm::pi<float>());
}

float InputService::getViewDir(Input::PlayerID id) {
	assert(id < max_player_id);
	if (!_player_active[id]) {
		LOG_WARN("use of inactive player {}", id);
		return 0.f;
	}

	return glm::mod(_player[id].view_dir + 2*glm::pi<float>(), 2*glm::pi<float>());
}

// TODO: use config service
SDL_Keycode InputService::loadKey(const std::string& name, const SDL_Keycode default_key) {
	(void) name;
	//auto k = SDL_GetKeyFromName(_pref->get(name.c_str(), SDL_GetKeyName(default_key)).c_str());
	auto k = SDLK_UNKNOWN;
	if (k == SDLK_UNKNOWN) {
		//_pref->set(name.c_str(), SDL_GetKeyName(default_key));
		return default_key;
	}

	return k;

}

// TODO: use config service
SDL_GameControllerButton InputService::loadButton(const std::string& name, const SDL_GameControllerButton default_button) {
	(void) name;
	//auto b = SDL_GameControllerGetButtonFromString(_pref->get(name.c_str(), SDL_GameControllerGetStringForButton(default_button)).c_str());
	auto b = SDL_CONTROLLER_BUTTON_INVALID;
	if (b == SDL_CONTROLLER_BUTTON_INVALID) {
		//_pref->set(name.c_str(), SDL_GameControllerGetStringForButton(default_button));
		return default_button;
	}

	return b;

}

bool InputService::setKPlayerViewMode(Input::PlayerID id, k_view_mode_t mode) {
	if (!_player_active[id]) {
		return false;
	}

	if (_player[id].is_controller) {
		return false;
	}

	_player[id].u.keyboard.view_mode = mode;

	return true;
}

Input::PlayerID InputService::findPlayerState(bool controller, SDL_JoystickID controller_id) {
	for (Input::PlayerID id = 0; id < max_player_id; id++) {
		if (!_player_active[id])
			continue;

		auto& it = _player[id];
		if (it.is_controller == controller && it.controller_instance_id == controller_id) {
			return id;
		}
	}

	return UINT16_MAX;
}

void InputService::updateKPlayerDirs(player_input_action_state& p) {
	int x = 0;
	int y = 0;

	if (p.u.keyboard.right)
		x++;
	if (p.u.keyboard.left)
		x--;
	if (p.u.keyboard.up)
		y--;
	if (p.u.keyboard.down)
		y++;

	if (x == -1 && y == -1)
		p.move_dir = (glm::pi<float>()*3.f)/4.f;
	else if (x == 0 && y == -1)
		p.move_dir = glm::pi<float>()/2.f;
	else if (x == 1 && y == -1)
		p.move_dir = glm::pi<float>()/4.f;
	else if (x == -1 && y == 0)
		p.move_dir = glm::pi<float>();
	else if (x == 1 && y == 0)
		p.move_dir = 0.f;
	else if (x == -1 && y == 1)
		p.move_dir = -(glm::pi<float>()*3.f)/4.f;
	else if (x == 0 && y == 1)
		p.move_dir = -glm::pi<float>()/2.f;
	else if (x == 1 && y == 1)
		p.move_dir = -glm::pi<float>()/4.f;

	//p.view_dir = 0.f; // TODO: view

	if (!x && !y)
		p.move_force = 0.f;
	else
		p.move_force = 1.f;

	glm::vec2 mouse_delta;
	if (p.u.keyboard.view_mode == k_view_mode_t::MOUSE_RELATIVE) {
		mouse_delta = {-p.u.keyboard.curr_x, p.u.keyboard.curr_y}; // invert
		p.u.keyboard.curr_x = 0;
		p.u.keyboard.curr_y = 0;
	} else {
		mouse_delta = {
			p.u.keyboard.curr_x - p.u.keyboard.last_x,
			-(p.u.keyboard.curr_y - p.u.keyboard.last_y)
		};
	}

	//std::cout << "got mouse delta: " << mouse_delta.x << " " << mouse_delta.y << "\n";

	if (p.u.keyboard.view_mode == k_view_mode_t::MOUSE_DRAG || p.u.keyboard.view_mode == k_view_mode_t::MOUSE_RELATIVE) {
		if (p.u.keyboard.mouse_button_down) {
			p.view_dir = glm::mod(glm::atan(mouse_delta.y, mouse_delta.x) + glm::pi<float>(), glm::two_pi<float>());
			p.view_force = glm::length(mouse_delta) * 0.08f;
		} else {
			p.view_force = 0.f;
		}
	}

	// new gos old
	p.u.keyboard.last_x = p.u.keyboard.curr_x;
	p.u.keyboard.last_y = p.u.keyboard.curr_y;
}

void InputService::updateCPlayerDirs(player_input_action_state& p) {
	p.move_dir = glm::atan(p.u.controller.left_y / -32768.f, p.u.controller.left_x / 32767.f);
	p.view_dir = glm::atan(p.u.controller.right_y / -32768.f, p.u.controller.right_x / 32767.f);

	p.move_force = (glm::abs(p.u.controller.left_x * glm::cos(p.move_dir)) + glm::abs(p.u.controller.left_y * glm::sin(p.move_dir)) + 0.5f) / 32768.f;
	if (p.move_force > 1.f) {
		p.move_force = 1.f;
	} else if (p.move_force < -1.f) {
		p.move_force = -1.f;
	}

	p.view_force = (glm::abs(p.u.controller.right_x * glm::cos(p.view_dir)) + glm::abs(p.u.controller.right_y * glm::sin(p.view_dir)) + 0.5f) / 32768.f;
	p.view_force = glm::clamp(p.view_force, -1.f, 1.f);
}

} // namespace MM::Services

