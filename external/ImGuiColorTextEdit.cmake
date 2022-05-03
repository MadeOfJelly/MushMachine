cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

add_library(imgui_color_text_edit
	"${CMAKE_CURRENT_LIST_DIR}/ImGuiColorTextEdit/TextEditor.h"
	"${CMAKE_CURRENT_LIST_DIR}/ImGuiColorTextEdit/TextEditor.cpp"
)
target_compile_features(imgui_color_text_edit PUBLIC cxx_std_11)
target_include_directories(imgui_color_text_edit PUBLIC "${CMAKE_CURRENT_LIST_DIR}/ImGuiColorTextEdit")
target_link_libraries(imgui_color_text_edit
	imgui
)

