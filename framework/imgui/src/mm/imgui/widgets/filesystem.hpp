#pragma once

#include <string>

#include <mm/services/filesystem.hpp>

namespace MM::ImGuiWidgets {

//#define IMGUI_PAYLOAD_TYPE_MM_FILE "MM_FILE"
//#define IMGUI_PAYLOAD_TYPE_MM_FILE_PATH "MM_FILE_PATH"

void FilePicker(const char* label, MM::Services::FilesystemService& fs, std::string& path, bool save = false);

}

