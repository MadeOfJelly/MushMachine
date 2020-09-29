#pragma once

#include <mm/services/screen_director.hpp>

namespace MM::Screens {

void create_mm_logo(MM::Engine& engine, MM::Services::ScreenDirector::Screen& screen, const std::string next_screen = "menu", float anim_duration = 2.f, float screen_duration = 2.f);

} // MM::Screens

