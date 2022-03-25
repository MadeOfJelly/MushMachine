#include "./mm_logo_screen.hpp"


#include <mm/services/organizer_scene.hpp>
#include <mm/services/opengl_renderer.hpp>

#include <entt/entity/registry.hpp>
#include <entt/entity/organizer.hpp>

#include <mm/opengl/render_tasks/simple_sprite.hpp>

#include <mm/opengl/texture_loader.hpp>
#include <mm/opengl/camera_3d.hpp>

#include <mm/components/position2d.hpp>
#include <mm/components/position2d_zoffset.hpp>
#include <mm/components/position3d.hpp>
#include <mm/components/scale2d.hpp>
#include <mm/components/transform4x4.hpp>
#include <mm/components/color.hpp>
#include <mm/components/time_delta.hpp>
#include <mm/opengl/components/texture.hpp>

#include <mm/systems/transform.hpp>

#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>

#include <mm/random/srng.hpp>
#include <random>


namespace MM::Screens {

namespace Components {

	struct screen_timer {
		float accumulator = 0.f;
		float duration = 0.f;
		std::string next_screen;
	};

	struct easing {
		glm::vec2 start{0.f, 0.f};
		glm::vec2 end{0.f, 0.f};

		float accumulator = 0.f;
		float duration = 1.f;
	};

} // Components

namespace Systems {

	void screen_timer_system(Components::screen_timer& sc_timer, const MM::Components::TimeDelta& td, const MM::Engine& engine) {
		sc_timer.accumulator += td.tickDelta;
		if (sc_timer.accumulator >= sc_timer.duration) {
			engine.getService<MM::Services::ScreenDirector>().queueChangeScreenTo(sc_timer.next_screen);
		}
	}

	// elastic scale easing
	void elasic_scale_easing(entt::view<entt::get_t<MM::Components::Scale2D, Components::easing>> view, const MM::Components::TimeDelta& td) {
		view.each([&td](auto& s, auto& easing_comp) {
			easing_comp.accumulator += td.tickDelta;

			// taken from https://github.com/warrenm/AHEasing
			// licensed under WTFPL
			auto elasticOut = [](float x) -> float {
				return glm::sin(-13.f * glm::half_pi<float>() * (x + 1.f)) * glm::pow(2.f, -10.f * x) + 1.f;
			};

			s.scale.x = glm::mix(easing_comp.start.x, easing_comp.end.x, elasticOut(easing_comp.accumulator / easing_comp.duration));
			s.scale.y = glm::mix(easing_comp.start.y, easing_comp.end.y, elasticOut(easing_comp.accumulator / easing_comp.duration));

		});
	}

} // Systems

#include "../res/mush_machine_logo_1.svg.png.h"

void create_mm_logo(MM::Engine& engine, MM::Services::ScreenDirector::Screen& screen,
		const std::string next_screen, float anim_duration, float screen_duration) {
	using namespace entt::literals;

	screen.start_enable.push_back(engine.type<MM::Services::OpenGLRenderer>());
	screen.start_enable.push_back(engine.type<MM::Services::OrganizerSceneService>());

	screen.start_provide.push_back({engine.type<MM::Services::SceneServiceInterface>(), engine.type<MM::Services::OrganizerSceneService>()});

	screen.end_disable.push_back(engine.type<MM::Services::OrganizerSceneService>());

	screen.start_fn = [anim_duration, screen_duration, next_screen](MM::Engine& _engine) {
		auto& rs = _engine.getService<MM::Services::OpenGLRenderer>();

		//rss->renderers.clear();

		bool found = false;
		for (auto& r_sptr : rs.render_tasks) {
			if (auto* r_ptr = dynamic_cast<MM::OpenGL::RenderTasks::SimpleSprite*>(r_sptr.get()); r_ptr != nullptr) {
				found = true;
				break;
			}
		}

		if (!found) {
			rs.addRenderTask<MM::OpenGL::RenderTasks::SimpleSprite>(_engine);
		}

		// =================================================

		auto& rm_t = MM::ResourceManager<MM::OpenGL::Texture>::ref();
		if (!rm_t.contains("mm_logo_white"_hs)) {
			rm_t.load<MM::OpenGL::TextureLoaderConstBuffer>("mm_logo_white", mush_machine_logo_1_svg_png, mush_machine_logo_1_svg_png_len);
		}

		// =================================================

		auto new_scene = std::make_unique<MM::Scene>();
		auto& scene = *new_scene;

		auto& org = scene.ctx().emplace<entt::organizer>();

		scene.ctx().emplace<MM::Engine&>(_engine); // alias

		scene.ctx().emplace<Components::screen_timer>(0.f, screen_duration, next_screen);

		org.emplace<Systems::screen_timer_system>("screen_timer_system");

		org.emplace<Systems::elasic_scale_easing>("elasic_scale_easing");
		org.emplace<MM::Systems::position3d_from_2d>("position3d_from_2d");
		org.emplace<MM::Systems::transform3d_translate>("transform3d_translate");
		//org.emplace<MM::Systems::transform3d_rotate2d>("transform3d_rotate2d");
		org.emplace<MM::Systems::transform3d_scale2d>("transform3d_scale2d");
		//org.emplace<MM::Systems::transform_clear_dirty>("transform_clear_dirty");

		auto& cam = scene.ctx().emplace<MM::OpenGL::Camera3D>();
		cam.horizontalViewPortSize = 89.f;
		cam.setOrthographic();
		cam.updateView();

		{
			auto e_logo = scene.create();
			scene.emplace<MM::Components::Position2D>(e_logo);
			scene.emplace<MM::Components::Position2D_ZOffset>(e_logo);
			scene.emplace<MM::Components::Position3D>(e_logo);
			scene.emplace<MM::Components::Scale2D>(e_logo, glm::vec2{0.f, 0.f});
			scene.emplace<MM::Components::Transform4x4>(e_logo);
			scene.emplace<MM::Components::DirtyTransformTag>(e_logo);

			auto& easing_comp = scene.emplace<Components::easing>(e_logo);
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

			MM::Random::SRNG rng{std::random_device{}(), 0};

			std::vector<glm::vec3> colors {color1, color2, color3};
			std::shuffle(colors.begin(), colors.end(), rng);

			auto& col = scene.emplace<MM::Components::Color>(e_logo);
			col.color = {colors.front(), 1.f};
		}

		_engine.getService<MM::Services::SceneServiceInterface>().changeSceneNow(std::move(new_scene));
	};

	screen.end_fn = [](MM::Engine& _engine) {
		_engine.getService<MM::Services::SceneServiceInterface>().changeSceneNow(std::make_unique<MM::Scene>());
	};
}

} // MM::Screens

