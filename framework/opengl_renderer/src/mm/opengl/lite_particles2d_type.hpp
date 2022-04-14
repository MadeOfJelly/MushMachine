#pragma once

#include <glm/vec4.hpp>

#include <nlohmann/json.hpp>

#include <mm/components/serialize/json_glm.hpp>
#include <vector>

namespace MM::OpenGL {

	struct LiteParticles2DType {
		struct Compute {
			float age_delta;

			glm::vec2 force_vec;

			float turbulence;
			float turbulence_noise_scale;
			float turbulence_individuality;
			float turbulence_time_scale;

			float dampening;

			static constexpr size_t _member_count = 8;
		} compute;

		struct Render {
			glm::vec4 color_start;
			glm::vec4 color_end;

			float size_start;
			float size_end;

			static constexpr size_t _member_count = 10;
		} render;

		// naive, prob fine, not time critical
		void upload(std::vector<float>& comp_vec, std::vector<float>& rend_vec) const {
			{
				comp_vec.push_back(compute.age_delta);

				comp_vec.push_back(compute.force_vec.x);
				comp_vec.push_back(compute.force_vec.y);

				comp_vec.push_back(compute.turbulence);
				comp_vec.push_back(compute.turbulence_noise_scale);
				comp_vec.push_back(compute.turbulence_individuality);
				comp_vec.push_back(compute.turbulence_time_scale);

				comp_vec.push_back(compute.dampening);
			}

			{
				rend_vec.push_back(render.color_start.r);
				rend_vec.push_back(render.color_start.g);
				rend_vec.push_back(render.color_start.b);
				rend_vec.push_back(render.color_start.a);

				rend_vec.push_back(render.color_end.r);
				rend_vec.push_back(render.color_end.g);
				rend_vec.push_back(render.color_end.b);
				rend_vec.push_back(render.color_end.a);

				rend_vec.push_back(render.size_start);
				rend_vec.push_back(render.size_end);
			}
		}
	};

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(LiteParticles2DType::Compute,
		age_delta,

		force_vec,

		turbulence,
		turbulence_noise_scale,
		turbulence_individuality,
		turbulence_time_scale,

		dampening
	)

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(LiteParticles2DType::Render,
		color_start,
		color_end,

		size_start,
		size_end
	)

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(LiteParticles2DType, compute, render)

} // MM::OpenGL

