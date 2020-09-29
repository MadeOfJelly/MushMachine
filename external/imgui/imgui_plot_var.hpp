#pragma once

#include "imgui/imgui.h"

// USAGE
// Call once a frame with current value
// ImGui::PlotVar("Speed", current_speed);

namespace ImGui {

// Plot value over time
// Pass FLT_MAX value to draw without adding a new value
void	PlotVar(const char* label, float value, float scale_min = FLT_MAX, float scale_max = FLT_MAX, int buffer_size = 120);

// Call this periodically to discard old/unused data
void	PlotVarFlushOldEntries();

}

