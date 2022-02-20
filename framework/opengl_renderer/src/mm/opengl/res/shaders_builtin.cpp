#include "./shaders_builtin.hpp"

#include <mm/fs_const_archiver.hpp>

namespace MM::OpenGL {

void load_builtin_shaders_fs(void) {

	// ==================== sampling.glsl ==================== START
	FS_CONST_MOUNT_FILE("/shaders/builtin/sampling.glsl",
R"(
#ifndef INCLUDE_BUILTIN_SAMPLING
#define INCLUDE_BUILTIN_SAMPLING

// kernel defined 9tap sampling
// samper, uv, 1.0/textureSize(), kernel, sum of kernel
vec3 sampling2D_kernel3x3_vec3(in sampler2D tex, in vec2 pos, in vec2 dir_step, in mat3x3 kernel, in float kernel_weight) {
	vec3 color =
		// upper row
		texture(tex, pos + dir_step * vec2(-1.0, 1.0)).rgb * kernel[0][0]
		+ texture(tex, pos + dir_step * vec2(0.0, 1.0)).rgb * kernel[0][1]
		+ texture(tex, pos + dir_step * vec2(1.0, 1.0)).rgb * kernel[0][2]

		// middle row
		+ texture(tex, pos + dir_step * vec2(-1.0, 0.0)).rgb * kernel[1][0]
		+ texture(tex, pos).rgb * kernel[1][1]
		+ texture(tex, pos + dir_step * vec2(1.0, 0.0)).rgb * kernel[1][2]

		// lower row
		+ texture(tex, pos + dir_step * vec2(-1.0, -1.0)).rgb * kernel[2][0]
		+ texture(tex, pos + dir_step * vec2(0.0, -1.0)).rgb * kernel[2][1]
		+ texture(tex, pos + dir_step * vec2(1.0, -1.0)).rgb * kernel[2][2]
	;

	return color / vec3(kernel_weight);
}

// 3x3 9tap tent filter
// 1 2 1
// 2 4 2
// 1 2 1
vec3 sampling2D_tent3x3_vec3(in sampler2D tex, in vec2 pos, in vec2 dir_step) {
	const mat3x3 tent_kernel = mat3x3(
		vec3(1.0, 2.0, 1.0),
		vec3(2.0, 4.0, 2.0),
		vec3(1.0, 2.0, 1.0)
	);
	const float tent_kernel_weight = 16.0;

	return sampling2D_kernel3x3_vec3(
		tex,
		pos,
		dir_step,
		tent_kernel,
		tent_kernel_weight
	);
}

#endif
)")
	// ==================== sampling.glsl ==================== END

	// ==================== tonemapping.glsl ==================== START
	FS_CONST_MOUNT_FILE("/shaders/builtin/tonemapping.glsl",
R"(
#ifndef INCLUDE_BUILTIN_TONEMAPPING
#define INCLUDE_BUILTIN_TONEMAPPING

// https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
vec3 tonemapACESFilm(vec3 x) {
	const float a = 2.51;
	const float b = 0.03;
	const float c = 2.43;
	const float d = 0.59;
	const float e = 0.14;

	return clamp((x*(a*x+b))/(x*(c*x+d)+e), vec3(0.0), vec3(1.0));
}

vec3 tonemapReinhard(vec3 x) {
	return x / (x + vec3(1.0));
}

#endif
)")
	// ==================== tonemapping.glsl ==================== END
}

} // MM::OpenGL

