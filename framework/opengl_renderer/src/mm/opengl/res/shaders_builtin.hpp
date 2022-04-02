#pragma once

namespace MM::OpenGL {

// loads glsl files into const fs at "/shaders/builtin/"
// file list:
// - sampling.glsl
// - tonemapping.glsl
// - hashing.glsl
void load_builtin_shaders_fs(void);

} // MM::OpenGL

