#include "nlohmann/json_fwd.hpp"
#include <gtest/gtest.h>

#include <mm/engine.hpp>

// services
#include <mm/services/sdl_service.hpp>
#include <mm/services/filesystem.hpp>
#include <mm/services/opengl_renderer.hpp>
#include <mm/services/imgui_s.hpp>

#include <mm/opengl/render_tasks/imgui.hpp>

#include <imgui/imgui.h>

#include <mm/imgui/widgets/imgui_json_editor.hpp>

const char* argv0;

TEST(imgui_json_editor, basic) {
	MM::Engine engine;

	auto& sdl_ss = engine.addService<MM::Services::SDLService>();
	ASSERT_TRUE(engine.enableService<MM::Services::SDLService>());

	sdl_ss.createGLWindow("imgui_json_editor_test", 1280, 720);

	engine.addService<MM::Services::FilesystemService>(argv0, "imgui_json_editor_test");
	ASSERT_TRUE(engine.enableService<MM::Services::FilesystemService>());

	engine.addService<MM::Services::ImGuiService>();
	ASSERT_TRUE(engine.enableService<MM::Services::ImGuiService>());

	auto& rs = engine.addService<MM::Services::OpenGLRenderer>();
	ASSERT_TRUE(engine.enableService<MM::Services::OpenGLRenderer>());

	rs.addRenderTask<MM::OpenGL::RenderTasks::ImGuiRT>(engine);

	class TestWindow : public MM::Services::Service {
		public:
			const char* name(void) override { return "TestWindow"; }

			bool enable(MM::Engine&, std::vector<MM::UpdateStrategies::TaskInfo>& task_array) override {
				task_array.push_back(
					MM::UpdateStrategies::TaskInfo{"testwindow"}
					.fn([this](MM::Engine&) { render(); })
				);
				return true;
			}

			void disable(MM::Engine&) override {}

			void render(void) {
				if (ImGui::Begin("test window")) {
					MM::ImGuiWidgets::JsonViewerSimple("test", _j);

					ImGui::Separator();

					MM::ImGuiWidgets::JsonViewerTree("test", _j);

					ImGui::Separator();

					try {
						MM::ImGuiWidgets::JsonEditor("test", _j);
						//using tmp_type = decltype(_j.items().begin());
						//tmp_type item{_j.begin()};
						//item.key();
					} catch(...) {
						assert(false);
					}

					_j.type();

					ImGui::Separator();

					const auto string = _j.dump(4);
					ImGui::TextUnformatted(string.c_str());
				}
				ImGui::End();
			}

			//nlohmann::ordered_json _j{
				//true,
				//"tseta",
				//1.f,
				//1,
				//1u,
				//nullptr,
				//nlohmann::ordered_json::object({{"hi", 10}})
			//};

			//nlohmann::ordered_json _j = R"({"hi": 12})"_json;
			//nlohmann::ordered_json _j = nlohmann::ordered_json::object({{"hi", 10}});
			nlohmann::ordered_json _j = R"(
{
	"data": {
		"cooldown": 6.0,
		"igs": [
			{
				"data": {
					"aoe_color": [
						0.2980392277240753,
						0.23137255012989044,
						0.1921568661928177,
						0.7764706015586853
					],
					"interactions": [
						{
							"data": {
								"atb": {
									"ag": {
										"base": -0.25,
										"per_slvl": -0.0375
									},
									"en": {
										"base": -0.35,
										"per_slvl": -0.0525
									},
									"in": {
										"base": -0.35,
										"per_slvl": -0.0525
									},
									"ne": null,
									"pe": null
								}
							},
							"type": "interaction::hp_dst"
						},
						{
							"data": {
								"buffs": [
									{
										"atb": {
											"ag": {
												"base": -0.15,
												"per_slvl": -0.0075
											},
											"in": null,
											"ne": {
												"base": -0.05,
												"per_slvl": -0.0025
											},
											"pe": null
										},
										"duration": {
											"base": 2.0,
											"per_slvl": 0.0
										},
										"type": "FAC"
									}
								]
							},
							"type": "interaction::debuffs_dst"
						}
					],
					"offset": 0.5,
					"pierce": true,
					"pierce_walls": false,
					"radius": 0.75,
					"target_own_team": false
				},
				"type": "ig::aoe"
			},
			{
				"data": {
					"interactions": [
						{
							"data": {
								"trauma_add": 0.04
							},
							"type": "interaction::camera_trauma_dst"
						}
					],
					"interactions_via": []
				},
				"type": "ig::self"
			}
		]
	},
	"type": "spell::one_shot"
}
)"_json;
	};

	engine.addService<TestWindow>(); // engine dtr ?????????????????

	ASSERT_TRUE(engine.enableService<TestWindow>());

	engine.run();

	sdl_ss.destroyWindow();
	engine.cleanup();
}

int main(int argc, char** argv) {
	argv0 = argv[0];

	::testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}

