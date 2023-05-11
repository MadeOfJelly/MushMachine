#include "./bloom.hpp"

#include <mm/opengl/render_tasks/bloom_extraction.hpp>
#include <mm/opengl/render_tasks/blur.hpp>
#include <mm/opengl/render_tasks/bloom_combine.hpp>

#include <mm/opengl/fbo_builder.hpp>
#include <mm/opengl/texture_loader.hpp>

namespace MM::OpenGL {

using namespace entt::literals;

void setup_bloom(
	MM::Engine& engine,
	const std::string color_src_tex,
	const size_t bloom_phases,
	const float bloom_in_scale,
	const float bloom_phase_scale
) {
	assert(bloom_phases > 1);
	auto& rs = engine.getService<MM::Services::OpenGLRenderer>();
	auto& rm_t = MM::ResourceManager<MM::OpenGL::Texture>::ref();
	auto [w, h] = engine.getService<MM::Services::SDLService>().getWindowSize();

#ifdef MM_OPENGL_3_GLES
	#if 0 // NOPE!!
	// TODO: caps at 1, invest in half float?
	const auto bloom_internal_format = GL_RGB565; // prolly fine. NOPE its not. it causes green pixely halos
	const auto bloom_format_type = GL_UNSIGNED_BYTE;
	#else
	const auto bloom_format_type = GL_RGBA;
	const auto bloom_internal_format = GL_RGBA16F;
	//const auto bloom_internal_format = GL_R11F_G11F_B10F;
	const auto bloom_data_type = GL_FLOAT;

	//const auto bloom_format_type = GL_RGBA_INTEGER;
	//const auto bloom_internal_format = GL_RGBA16I;
	//const auto bloom_data_type = GL_SHORT;
	#endif
#else
	const auto bloom_format_type = GL_RGB;
	const auto bloom_internal_format = GL_RGB16F; // opengl silently upgrades to RGBA
	//const auto bloom_internal_format = GL_R11F_G11F_B10F; // no sign
	const auto bloom_data_type = GL_FLOAT;
#endif

	{ // bloom in (bloom extraction)
		rm_t.reload<MM::OpenGL::TextureLoaderEmpty>(
			"bloom_in",
			bloom_internal_format,
			w * bloom_in_scale, h * bloom_in_scale,
			bloom_format_type, bloom_data_type
		);
		{ // filter
			rm_t.get("bloom_in"_hs)->bind(0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}

		rs.targets["bloom_extraction"] = MM::OpenGL::FBOBuilder::start()
			.attachTexture(rm_t.get("bloom_in"_hs), GL_COLOR_ATTACHMENT0)
			.setResizeFactors(bloom_in_scale, bloom_in_scale)
			.setResize(true)
			.finish();
		assert(rs.targets["bloom_extraction"]);
	}

	// blur textures and fbos
	for (size_t i = 1; i <= bloom_phases; i++) {
		// TODO: further dedup
		std::string tex_out_name {"blur_out" + std::to_string(i)};
		auto tex_out_id = entt::hashed_string::value(tex_out_name.c_str());
		rm_t.reload<MM::OpenGL::TextureLoaderEmpty>(
			tex_out_id,
			bloom_internal_format,
			w * bloom_in_scale * glm::pow(bloom_phase_scale, i), h * bloom_in_scale * glm::pow(bloom_phase_scale, i),
			bloom_format_type, bloom_data_type
		);
		{ // filter
			rm_t.get(tex_out_id)->bind(0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}

		std::string tex_tmp_name {"blur_tmp" + std::to_string(i)};
		auto tex_tmp_id = entt::hashed_string::value(tex_tmp_name.c_str());
		rm_t.reload<MM::OpenGL::TextureLoaderEmpty>(
			tex_tmp_id,
			bloom_internal_format,
			w * bloom_in_scale * glm::pow(bloom_phase_scale, i), h * bloom_in_scale * glm::pow(bloom_phase_scale, i),
			bloom_format_type, bloom_data_type
		);
		{ // filter
			rm_t.get(tex_tmp_id)->bind(0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}

		rs.targets[tex_out_name] = MM::OpenGL::FBOBuilder::start()
			.attachTexture(rm_t.get(tex_out_id), GL_COLOR_ATTACHMENT0)
			.setResizeFactors(bloom_in_scale * glm::pow(bloom_phase_scale, i), bloom_in_scale * glm::pow(bloom_phase_scale, i))
			.setResize(true)
			.finish();
		assert(rs.targets[tex_out_name]);

		rs.targets[tex_tmp_name] = MM::OpenGL::FBOBuilder::start()
			.attachTexture(rm_t.get(tex_tmp_id), GL_COLOR_ATTACHMENT0)
			.setResizeFactors(bloom_in_scale * glm::pow(bloom_phase_scale, i), bloom_in_scale * glm::pow(bloom_phase_scale, i))
			.setResize(true)
			.finish();
		assert(rs.targets[tex_tmp_name]);
	}

	{ // render tasks
		auto& extraction = rs.addRenderTask<MM::OpenGL::RenderTasks::BloomExtraction>(engine);
		extraction.src_tex = color_src_tex;
		extraction.target_fbo = "bloom_extraction";

		const glm::vec2 blur_factor {1.f, 1.f};

		{ // blur rt
			std::string prev_out_tex = "bloom_in";
			for (size_t i = 1; i <= bloom_phases; i++) {
				auto& blur = rs.addRenderTask<MM::OpenGL::RenderTasks::Blur>(engine);
				// h
				blur.in_tex = prev_out_tex;
				blur.temp_fbo = "blur_tmp" + std::to_string(i);
				// v
				blur.temp_tex = "blur_tmp" + std::to_string(i);
				blur.out_fbo = "blur_out" + std::to_string(i);
				blur.out_tex = "blur_out" + std::to_string(i);
				blur.tex_offset_factor = blur_factor * glm::vec2{2.f, 1.f}; // the input texture is double the size

				// old blur:
				//blur.tex_offset = {1.f/(w * bloom_in_scale * bloom_in_scale), 1.f/(h * bloom_in_scale * bloom_in_scale)};

				prev_out_tex = blur.out_tex;
			}
		}

		// combine passes
		for (size_t i = bloom_phases; i > 1; i--) {
			auto& combine = rs.addRenderTask<MM::OpenGL::RenderTasks::BloomCombine>(engine);
			if (i == bloom_phases) {
				combine.tex0 = "blur_out" + std::to_string(i);
			} else {
				combine.tex0 = "blur_tmp" + std::to_string(i);
			}
			combine.tex1 = "blur_out" + std::to_string(i-1);
			combine.target_fbo = "blur_tmp" + std::to_string(i-1); // -> blur_tmpi-1
		}
	}
}

} // MM::OpenGL

