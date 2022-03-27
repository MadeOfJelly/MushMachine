#pragma once

#include <string>

#include "./render_task.hpp"

namespace MM::OpenGL {

	// helper function to setup a bloom blur and recombine chain
	// creates (texture) rendertarget, fbos and rendertasks
	// outputs blur to "blur_tmp1" texture
	//
	// you still need to add the Composition rendertask (or eqv) your self, eg:
	//	auto& comp = rs.addRenderTask<MM::OpenGL::RenderTasks::Composition>(engine);
	//	comp.color_tex = "hdr_color";
	//	comp.bloom_tex = "blur_tmp1";
	//	comp.target_fbo = "display";
	void setup_bloom(
		MM::Engine& engine,
		const std::string color_src_tex = "hdr_color", // texture to extract color from
		const size_t bloom_phases = 5, // number of downsampled blurs (4 prob fine for 720, 5 for 1080)
		const float bloom_in_scale = 0.5f, // scale of bloom extraction layer (1 - 0.5 best, lower for more perf)
		const float bloom_phase_scale = 0.5f // ammount of scaling per downsampling
	);

} // MM::OpenGL

