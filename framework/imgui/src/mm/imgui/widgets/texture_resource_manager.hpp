#pragma once

#include <mm/resource_manager.hpp>

#include <mm/opengl/texture.hpp>
#include "./texture.hpp"

// fwd
namespace MM {
	class Engine;
}

namespace MM::ImGuiWidgets {

void TextureResourceManagerList(void);

void TextureResourceManagerLoader(Engine& engine);

}


