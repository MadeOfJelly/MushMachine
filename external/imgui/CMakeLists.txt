cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

project(imgui C CXX)

set(CPP_FILES
	"imgui/imgui.cpp"
	"imgui/imgui_draw.cpp"
	"imgui/imgui_widgets.cpp"
	"imgui/imgui_tables.cpp"
	"imgui/imgui_demo.cpp"

	"imgui/misc/cpp/imgui_stdlib.cpp"

	"imgui_plot_var.cpp"

	#"imgui_impl_sdl_gl3.cpp" # very old
	#"imgui_impl_sdl.cpp" # old
	#"imgui_impl_opengl3.cpp" # old
	"imgui/backends/imgui_impl_sdl.cpp"
	"imgui/backends/imgui_impl_opengl3.cpp"
)

set(HPP_FILES
	"imgui/imgui.h"
	"imgui/imstb_rectpack.h"
	"imgui/imstb_textedit.h"
	"imgui/imstb_truetype.h"

	"imgui/misc/cpp/imgui_stdlib.h"

	"imgui_plot_var.hpp"

	#"imgui_impl_sdl_gl3.h" # very old
	#"imgui_impl_sdl.h" # old
	#"imgui_impl_opengl3.h" # old
	"imgui/backends/imgui_impl_sdl.h"
	"imgui/backends/imgui_impl_opengl3.h"
)

add_library(imgui ${CPP_FILES} ${HPP_FILES})

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

target_compile_definitions(imgui PUBLIC IMGUI_DISABLE_OBSOLETE_FUNCTIONS=1)

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

