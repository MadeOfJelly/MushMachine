#pragma once

#include <mm/opengl/texture.hpp>

namespace MM::ImGuiWidgets {

#define IMGUI_PAYLOAD_TYPE_MM_REND_TEXTURE "MM_REND_TEXTURE"

void Texture(MM::OpenGL::Texture::handle_t& texture, bool dropTarget = true);
void LabelTexture(const char* label, MM::OpenGL::Texture::handle_t& texture, bool dropTarget = true);

}

