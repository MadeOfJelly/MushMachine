cmake_minimum_required(VERSION 3.9...3.16 FATAL_ERROR)

project(imgui C CXX)

add_library(imgui
	"imgui/imgui.h"
	"imgui/imgui.cpp"
	"imgui/imgui_draw.cpp"
	"imgui/imgui_widgets.cpp"
	"imgui/imgui_tables.cpp"
	"imgui/imgui_demo.cpp"

	"imgui/imstb_rectpack.h"
	"imgui/imstb_textedit.h"
	"imgui/imstb_truetype.h"

	"imgui/misc/cpp/imgui_stdlib.h"
	"imgui/misc/cpp/imgui_stdlib.cpp"

	"imgui_plot_var.hpp"
	"imgui_plot_var.cpp"

	# TODO: seperate backends into libs
	"imgui/backends/imgui_impl_sdl2.h"
	"imgui/backends/imgui_impl_sdl2.cpp"
	"imgui/backends/imgui_impl_opengl3.h"
	"imgui/backends/imgui_impl_opengl3.cpp"
)

target_compile_features(imgui PUBLIC cxx_std_11)

if(MM_OPENGL_3_GLES)
	target_compile_definitions(imgui PUBLIC IMGUI_IMPL_OPENGL_ES3)
	target_compile_definitions(imgui PUBLIC IMGUI_IMPL_OPENGL_LOADER_CUSTOM) # bug?
else()
	target_compile_definitions(imgui PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLAD)
	target_link_libraries(imgui PUBLIC glad)
endif()

target_include_directories(imgui PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
#target_include_directories(imgui PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/imgui")
target_include_directories(imgui PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/imgui") # im sad

#target_compile_definitions(imgui PUBLIC IMGUI_DISABLE_OBSOLETE_FUNCTIONS=1) # more sad

if(EMSCRIPTEN)
	target_compile_options(imgui PUBLIC -sUSE_SDL=2)
	target_link_libraries(imgui PUBLIC -sUSE_SDL=2 -sUSE_WEBGL2=1 -sFULL_ES3=1)
elseif(ANDROID)
	target_link_libraries(imgui PUBLIC SDL)
elseif(VCPKG_TARGET_TRIPLET)
	target_link_libraries(imgui
		PUBLIC
			SDL2::SDL2
		#PRIVATE
			#SDL2::SDL2main
			#SDL2::SDL2-static
	)
else()
	target_include_directories(imgui PUBLIC "${SDL2_INCLUDE_DIR}")
	target_link_libraries(imgui PUBLIC ${SDL2_LIBRARY})
endif()

target_compile_definitions(imgui PUBLIC ImTextureID=unsigned\ long)

