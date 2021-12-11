#include "./mm_logo_screen.hpp"

#include <glm/gtc/constants.hpp>

#include <mm/services/organizer_scene.hpp>
#include <mm/services/opengl_renderer.hpp>

#include <entt/entity/registry.hpp>

#include <mm/opengl/render_tasks/simple_sprite.hpp>

#include <mm/opengl/texture_loader.hpp>
#include <mm/opengl/camera_3d.hpp>

#include <mm/components/transform2d.hpp>
#include <mm/components/color.hpp>
#include <mm/opengl/components/texture.hpp>

#include <random>

namespace MM::Screens {

#include "../res/mush_machine_logo_1.svg.png.h"

void create_mm_logo(MM::Engine& engine, MM::Services::ScreenDirector::Screen& screen,
		const std::string next_screen, float anim_duration, float screen_duration) {
	using namespace entt::literals;

	screen.start_enable.push_back(engine.type<MM::Services::OpenGLRenderer>());
	screen.start_enable.push_back(engine.type<MM::Services::SimpleSceneService>());

	screen.start_provide.push_back({engine.type<MM::Services::SceneServiceInterface>(), engine.type<MM::Services::SimpleSceneService>()});

	screen.end_disable.push_back(engine.type<MM::Services::SimpleSceneService>());

	screen.start_fn = [anim_duration, screen_duration, next_screen](MM::Engine& engine) {
		auto& rs = engine.getService<MM::Services::OpenGLRenderer>();

		//rss->renderers.clear();

		bool found = false;
		for (auto& r_sptr : rs.render_tasks) {
			if (auto* r_ptr = dynamic_cast<MM::OpenGL::RenderTasks::SimpleSprite*>(r_sptr.get()); r_ptr != nullptr) {
				found = true;
				break;
			}
		}

		if (!found) {
			rs.addRenderTask<MM::OpenGL::RenderTasks::SimpleSprite>(engine);
		}

		// =================================================

		auto& rm_t = MM::ResourceManager<MM::OpenGL::Texture>::ref();
		if (!rm_t.contains("mm_logo_white"_hs)) {
			rm_t.load<MM::OpenGL::TextureLoaderConstBuffer>("mm_logo_white", mush_machine_logo_1_svg_png, mush_machine_logo_1_svg_png_len);
		}

		// =================================================

		engine.getService<MM::Services::SceneServiceInterface>().changeSceneNow(std::make_unique<MM::Scene>());
		auto& scene = engine.getService<MM::Services::SceneServiceInterface>().getScene();

		scene.ctx_or_set<MM::Engine*>(&engine);

		struct easing {
			glm::vec2 start{0.f, 0.f};
			glm::vec2 end{0.f, 0.f};

			float accumulator = 0.f;
			float duration = 1.f;
		};

		struct screen_timer {
			float accumulator = 0.f;
		};

		MM::AddSystemToScene(scene, [screen_duration, next_screen](MM::Scene& scene, float delta) {
			auto& sc_timer = scene.ctx<screen_timer>();
			sc_timer.accumulator += delta;

			if (sc_timer.accumulator >= screen_duration) {
				scene.ctx<MM::Engine*>()->getService<MM::Services::ScreenDirector>().queueChangeScreenTo(next_screen);
			}
		});
		scene.set<screen_timer>();

		// elastic scale easing
		MM::AddSystemToScene(scene, [](MM::Scene& scene, float delta) {
			auto view = scene.view<MM::Components::Transform2D, easing>();

			for (auto& e : view) {
				auto& t = view.get<MM::Components::Transform2D>(e);
				auto& easing_comp = view.get<easing>(e);
				easing_comp.accumulator += delta;

				//auto elasticOut = [](float x) -> float {
					//const float c4 = (2.f * glm::pi<float>()) / 3.f;
					//return x <= 0.f ? 0.f : x >= 1.f ? 1.f // limit output to [0,1]
					//: glm::pow(2.f, -10.f * x) * glm::sin((x * 10.f - 0.75f) * c4) + 1.f;
				//};
				// taken from https://github.com/warrenm/AHEasing
				// licensed under WTFPL
				auto elasticOut = [](float x) -> float {
					return glm::sin(-13.f * glm::half_pi<float>() * (x + 1.f)) * glm::pow(2.f, -10.f * x) + 1.f;
				};

				t.scale.x = glm::mix(easing_comp.start.x, easing_comp.end.x, elasticOut(easing_comp.accumulator / easing_comp.duration));
				t.scale.y = glm::mix(easing_comp.start.y, easing_comp.end.y, elasticOut(easing_comp.accumulator / easing_comp.duration));

			}
		});

		auto& cam = scene.set<MM::OpenGL::Camera3D>();
		cam.horizontalViewPortSize = 89.f;
		cam.setOrthographic();
		cam.updateView();

		{
			auto e_logo = scene.create();
			auto& t = scene.emplace<MM::Components::Transform2D>(e_logo);
			t.scale = {0,0};

			auto& easing_comp = scene.emplace<easing>(e_logo);
			easing_comp.start = {0.f, 0.f};
			easing_comp.end = {50.f * 1.12609649122807017543f, 50.f};
			easing_comp.duration = anim_duration;

			auto& tex_comp = scene.emplace<MM::Components::OpenGL::Texture>(e_logo);
			tex_comp.tex = rm_t.get("mm_logo_white"_hs);

			//color1=ff66cc
			//color2=ccff66
			//color3=66ccff

			glm::vec3 color1{
				1.f,
				0x66/(float)0xff,
				0xcc/(float)0xff,
			};

			glm::vec3 color2{
				0xcc/(float)0xff,
				1.f,
				0x66/(float)0xff,
			};

			glm::vec3 color3{
				0x66/(float)0xff,
				0xcc/(float)0xff,
				1.f,
			};

			std::mt19937_64 mt{std::random_device{}()};

			std::vector<glm::vec3> colors {color1, color2, color3};
			std::shuffle(colors.begin(), colors.end(), mt);

			auto& col = scene.emplace<MM::Components::Color>(e_logo);
			col.color = {colors.front(), 1.f};
		}

	};

	screen.end_fn = [](MM::Engine& engine) {
		engine.getService<MM::Services::SceneServiceInterface>().changeSceneNow(std::make_unique<MM::Scene>());
	};
}

} // MM::Screens

