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

	// ==================== hashing.glsl ==================== START
	FS_CONST_MOUNT_FILE("/shaders/builtin/hashing.glsl",
R"(
#ifndef INCLUDE_BUILTIN_HASHING
#define INCLUDE_BUILTIN_HASHING

// https://www.shadertoy.com/view/4djSRW
// Hash without Sine
// MIT License...
/* Copyright (c)2014 David Hoskins.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/

//----------------------------------------------------------------------------------------
// 1 out, 1 in...
float hash11(float p)
{
	p = fract(p * .1031);
	p *= p + 33.33;
	p *= p + p;
	return fract(p);
}

//----------------------------------------------------------------------------------------
// 1 out, 2 in...
float hash12(vec2 p)
{
	vec3 p3  = fract(vec3(p.xyx) * .1031);
	p3 += dot(p3, p3.yzx + 33.33);
	return fract((p3.x + p3.y) * p3.z);
}

//----------------------------------------------------------------------------------------
// 1 out, 3 in...
float hash13(vec3 p3)
{
	p3  = fract(p3 * .1031);
	p3 += dot(p3, p3.zyx + 31.32);
	return fract((p3.x + p3.y) * p3.z);
}

//----------------------------------------------------------------------------------------
// TODO: test properly
// 1 out, 4 in...
float hash14(vec4 p4)
{
	p4  = fract(p4 * .1031);
	p4 += dot(p4, p4.zywx + 31.32);
	return fract((p4.x + p4.y + p4.w) * p4.z);
}

//----------------------------------------------------------------------------------------
// 2 out, 1 in...
vec2 hash21(float p)
{
	vec3 p3 = fract(vec3(p) * vec3(.1031, .1030, .0973));
	p3 += dot(p3, p3.yzx + 33.33);
	return fract((p3.xx+p3.yz)*p3.zy);
}

//----------------------------------------------------------------------------------------
// 2 out, 2 in...
vec2 hash22(vec2 p)
{
	vec3 p3 = fract(vec3(p.xyx) * vec3(.1031, .1030, .0973));
	p3 += dot(p3, p3.yzx+33.33);
	return fract((p3.xx+p3.yz)*p3.zy);
}

//----------------------------------------------------------------------------------------
// 2 out, 3 in...
vec2 hash23(vec3 p3)
{
	p3 = fract(p3 * vec3(.1031, .1030, .0973));
	p3 += dot(p3, p3.yzx+33.33);
	return fract((p3.xx+p3.yz)*p3.zy);
}

//----------------------------------------------------------------------------------------
// TODO: test properly
// 2 out, 4 in...
vec2 hash24(vec4 p4)
{
	p4 = fract(p4 * vec4(.1031, .1030, .0973, .1337));
	p4 += dot(p4, p4.ywzx+33.33);
	return fract((p4.wx+p4.yz)*p4.zy);
}

//----------------------------------------------------------------------------------------
// 3 out, 1 in...
vec3 hash31(float p)
{
	vec3 p3 = fract(vec3(p) * vec3(.1031, .1030, .0973));
	p3 += dot(p3, p3.yzx+33.33);
	return fract((p3.xxy+p3.yzz)*p3.zyx);
}


//----------------------------------------------------------------------------------------
// 3 out, 2 in...
vec3 hash32(vec2 p)
{
	vec3 p3 = fract(vec3(p.xyx) * vec3(.1031, .1030, .0973));
	p3 += dot(p3, p3.yxz+33.33);
	return fract((p3.xxy+p3.yzz)*p3.zyx);
}

//----------------------------------------------------------------------------------------
// 3 out, 3 in...
vec3 hash33(vec3 p3)
{
	p3 = fract(p3 * vec3(.1031, .1030, .0973));
	p3 += dot(p3, p3.yxz+33.33);
	return fract((p3.xxy + p3.yxx)*p3.zyx);
}

//----------------------------------------------------------------------------------------
// 4 out, 1 in...
vec4 hash41(float p)
{
	vec4 p4 = fract(vec4(p) * vec4(.1031, .1030, .0973, .1099));
	p4 += dot(p4, p4.wzxy+33.33);
	return fract((p4.xxyz+p4.yzzw)*p4.zywx);
}

//----------------------------------------------------------------------------------------
// 4 out, 2 in...
vec4 hash42(vec2 p)
{
	vec4 p4 = fract(vec4(p.xyxy) * vec4(.1031, .1030, .0973, .1099));
	p4 += dot(p4, p4.wzxy+33.33);
	return fract((p4.xxyz+p4.yzzw)*p4.zywx);
}

//----------------------------------------------------------------------------------------
// 4 out, 3 in...
vec4 hash43(vec3 p)
{
	vec4 p4 = fract(vec4(p.xyzx)  * vec4(.1031, .1030, .0973, .1099));
	p4 += dot(p4, p4.wzxy+33.33);
	return fract((p4.xxyz+p4.yzzw)*p4.zywx);
}

//----------------------------------------------------------------------------------------
// 4 out, 4 in...
vec4 hash44(vec4 p4)
{
	p4 = fract(p4  * vec4(.1031, .1030, .0973, .1099));
	p4 += dot(p4, p4.wzxy+33.33);
	return fract((p4.xxyz+p4.yzzw)*p4.zywx);
}

#endif
)")
	// ==================== hashing.glsl ==================== END
}

} // MM::OpenGL

