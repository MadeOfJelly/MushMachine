#include <gtest/gtest.h>
#include <mm/services/input_service.hpp>
#include <mm/services/sdl_service.hpp>
#include <mm/services/filesystem.hpp>
#include <mm/services/opengl_renderer.hpp>
#include <mm/services/imgui_s.hpp>

#include <mm/opengl/render_tasks/imgui.hpp>

#include <entt/core/hashed_string.hpp>

#include <imgui/imgui.h>


class InputVisualizer : public MM::Services::Service {
	private:
		MM::Input::PlayerID _player_id;
		MM::Services::SDLService::EventHandlerHandle _event_handle = nullptr;

	public:
		const char* name(void) override { return "InputVisualizer"; }

		bool enable(MM::Engine& engine) override {
			_player_id = UINT16_MAX;

			auto* sdl_ss = engine.tryService<MM::Services::SDLService>();
			if (sdl_ss) {
				_event_handle = sdl_ss->addEventHandler([this, &engine](const SDL_Event& e) -> bool {
					return this->handleEvent(e, engine);
				});
			} else {
				return false;
			}


			for (int i = 0; i < SDL_NumJoysticks(); i++) {
				if (SDL_IsGameController(i)) {
					std::cerr << "got controller " << i << "\n";
					SDL_GameControllerOpen(i);
				}
			}

			return true;
		}

		void disable(MM::Engine&) override {
		}

		std::vector<MM::UpdateStrategies::UpdateCreationInfo> registerUpdates(void) override {
			return {
				{
					"InputVisualizer::render"_hs,
					"InputVisualizer::render",
					[this](MM::Engine& e){ this->renderImGui(e); },
					MM::UpdateStrategies::update_phase_t::MAIN,
					true,
					{ "InputService::update"_hs }
				}
			};
		}


	private:
		void renderImGui(MM::Engine& engine) {
			ImGui::Begin("InputVisualizer", NULL, ImGuiWindowFlags_AlwaysAutoResize);
			if (_player_id == UINT16_MAX) {
				ImGui::Text("press [SPACE](keyboard) or [A](controller)");
			} else {
				auto* input = engine.tryService<MM::Services::InputService>();
				ImGui::Text("InputAction active:");

				ImGui::Value("SPELL_WEAPON", input->get(_player_id, MM::Services::InputService::SPELL_WEAPON));
				ImGui::Value("SPELL_1", input->get(_player_id, MM::Services::InputService::SPELL_1));
				ImGui::Value("SPELL_2", input->get(_player_id, MM::Services::InputService::SPELL_2));
				ImGui::Value("SPELL_3", input->get(_player_id, MM::Services::InputService::SPELL_3));
				ImGui::Value("SPELL_4", input->get(_player_id, MM::Services::InputService::SPELL_4));
				ImGui::Value("SPELL_5", input->get(_player_id, MM::Services::InputService::SPELL_5));
				ImGui::Value("SPELL_6", input->get(_player_id, MM::Services::InputService::SPELL_6));
				ImGui::Value("USE", input->get(_player_id, MM::Services::InputService::USE));

				ImGui::Separator();

				{
					ImDrawList* draw_list = ImGui::GetWindowDrawList();

					static ImVec4 col = ImVec4(1.0f, 1.0f, 0.4f, 1.0f);
					const ImU32 col32 = ImColor(col);

					const ImVec2 p = ImGui::GetCursorScreenPos();

					float scaling = 50.f;
					float x = p.x + scaling;
					float y = p.y + scaling;
					float spacing = 8.0f;

					draw_list->AddCircleFilled(ImVec2(x, y), scaling, ImColor(0.2f, 0.2f, 0.2f, 0.7f), 24);
					auto move_vec = input->getMoveVec(_player_id);
					draw_list->AddLine(ImVec2(x, y), ImVec2(x + move_vec.x * scaling, y + move_vec.y * scaling), col32, 3.f);

					x += scaling*2 + spacing;

					draw_list->AddCircleFilled(ImVec2(x, y), scaling, ImColor(0.2f, 0.2f, 0.2f, 0.7f), 24);
					auto view_vec = input->getViewVec(_player_id);
					draw_list->AddLine(ImVec2(x, y), ImVec2(x + view_vec.x * scaling, y + view_vec.y * scaling), col32, 3.f);

					ImGui::Dummy(ImVec2(scaling*4 + spacing, scaling*2));
				}
			}
			ImGui::End();
		}

	private:
		bool handleEvent(const SDL_Event& e, MM::Engine& engine) {
			auto* input = engine.tryService<MM::Services::InputService>();
			if (_player_id == UINT16_MAX) {
				if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE) {
					_player_id = input->addPlayer(true, 0);
					return true;
				} else if (e.type == SDL_CONTROLLERDEVICEADDED) {
					SDL_GameControllerOpen(e.cdevice.which);
					std::cout << "added: " << e.cdevice.which << std::endl;
					return true;
				} else if (e.type == SDL_CONTROLLERBUTTONDOWN && e.cbutton.button == SDL_CONTROLLER_BUTTON_A) {
					_player_id = input->addPlayer(false, e.cbutton.which);
					return true;
				}
			}
			return false;
		}
};

TEST(input_service, input_visualizer) {
	MM::Engine engine;

	auto& sdl_ss = engine.addService<MM::Services::SDLService>();
	ASSERT_TRUE(engine.enableService<MM::Services::SDLService>());

	sdl_ss.createGLWindow("service_input_visualizer_test", 1280, 720);

	engine.addService<InputVisualizer>();
	ASSERT_TRUE(engine.enableService<InputVisualizer>());

	engine.addService<MM::Services::InputService>();
	ASSERT_TRUE(engine.enableService<MM::Services::InputService>());

	engine.addService<MM::Services::ImGuiService>();
	ASSERT_TRUE(engine.enableService<MM::Services::ImGuiService>());

	auto& rs = engine.addService<MM::Services::OpenGLRenderer>();
	ASSERT_TRUE(engine.enableService<MM::Services::OpenGLRenderer>());

	rs.addRenderTask<MM::OpenGL::RenderTasks::ImGuiRT>(engine);

	engine.run();

	sdl_ss.destroyWindow();
}

