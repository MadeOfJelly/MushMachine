cmake_minimum_required(VERSION 3.2)
project(opengl_renderer CXX)

add_library(opengl_renderer_s
	src/mm/opengl/render_task.hpp

	src/mm/services/opengl_renderer.hpp
	src/mm/services/opengl_renderer.cpp

	src/mm/opengl/camera_3d.hpp
	src/mm/opengl/camera_3d.cpp
)

target_include_directories(opengl_renderer_s PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

add_definitions(-DGLM_ENABLE_EXPERIMENTAL)
target_link_libraries(opengl_renderer_s
	engine
	logger

	opengl_primitives
	sdl_service
)

if(EMSCRIPTEN)
	set_target_properties(opengl_renderer_s PROPERTIES COMPILE_FLAGS "-s USE_SDL=2")
	set_target_properties(opengl_renderer_s PROPERTIES LINK_FLAGS "-s USE_SDL=2")
endif()

############# imgui opengl renderer tools ###########

add_library(imgui_opengl_renderer_tools
	src/mm/services/opengl_renderer_tools.hpp
	src/mm/services/opengl_renderer_tools.cpp
)

target_link_libraries(imgui_opengl_renderer_tools
	opengl_renderer_s
	imgui_service
)

############# imgui render task ###########

add_library(imgui_render_task
	src/mm/opengl/render_tasks/imgui.hpp
	src/mm/opengl/render_tasks/imgui.cpp
)

target_include_directories(imgui_render_task PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

#add_definitions(-DGLM_ENABLE_EXPERIMENTAL)
target_link_libraries(imgui_render_task
	opengl_renderer_s

	imgui
	icon_font_cpp_headers
)

############# simple rect render task ###########

add_library(simple_rect_render_task
	src/mm/opengl/render_tasks/simple_rect.hpp
	src/mm/opengl/render_tasks/simple_rect.cpp
)

target_include_directories(simple_rect_render_task PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_link_libraries(simple_rect_render_task
	opengl_renderer_s
	common_components
	engine
)

############# simple sprite render task ###########

add_library(simple_sprite_render_task
	src/mm/opengl/render_tasks/simple_sprite.hpp
	src/mm/opengl/render_tasks/simple_sprite.cpp
)

target_include_directories(simple_sprite_render_task PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_link_libraries(simple_sprite_render_task
	opengl_renderer_s
	common_components
	engine
)

############# simple spritesheet render task ###########

add_library(simple_spritesheet_render_task
	src/mm/opengl/render_tasks/simple_spritesheet.hpp
	src/mm/opengl/render_tasks/spritesheet_renderable.hpp

	src/mm/opengl/render_tasks/simple_spritesheet.cpp
)

target_include_directories(simple_spritesheet_render_task PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_link_libraries(simple_spritesheet_render_task
	opengl_renderer_s
	common_components
	engine
)

############# batched spritesheet render task ###########

add_library(batched_spritesheet_render_task
	src/mm/opengl/render_tasks/batched_spritesheet.hpp
	src/mm/opengl/render_tasks/spritesheet_renderable.hpp # dup?

	src/mm/opengl/render_tasks/batched_spritesheet.cpp
)

target_include_directories(batched_spritesheet_render_task PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_link_libraries(batched_spritesheet_render_task
	opengl_renderer_s
	common_components
	engine
)

############# clear render task ###########

add_library(clear_render_task
	src/mm/opengl/render_tasks/clear.hpp
	src/mm/opengl/render_tasks/clear.cpp
)

target_include_directories(clear_render_task PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_link_libraries(clear_render_task
	opengl_renderer_s
	engine
)

############# blit fb render task ###########

add_library(blit_fb_render_task
	src/mm/opengl/render_tasks/blit_fb.hpp
	src/mm/opengl/render_tasks/blit_fb.cpp
)

target_include_directories(blit_fb_render_task PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_link_libraries(blit_fb_render_task
	opengl_renderer_s
	engine
)

############# copy to fb render task ###########

add_library(copy_to_fb_render_task
	src/mm/opengl/render_tasks/copy_to_fb.hpp
	src/mm/opengl/render_tasks/copy_to_fb.cpp
)

target_include_directories(copy_to_fb_render_task PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_link_libraries(copy_to_fb_render_task
	opengl_renderer_s
	engine
)

############# blur render task ###########

add_library(blur_render_task
	src/mm/opengl/render_tasks/blur.hpp
	src/mm/opengl/render_tasks/blur.cpp
)

target_include_directories(blur_render_task PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_link_libraries(blur_render_task
	opengl_renderer_s
	engine
)

############# tilemap renderer ###########

add_library(tilemap_render_task
	src/mm/opengl/render_tasks/tilemap.hpp
	src/mm/opengl/render_tasks/tilemap_renderable.hpp

	src/mm/opengl/render_tasks/tilemap.cpp
)

target_include_directories(tilemap_render_task PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_link_libraries(tilemap_render_task
	opengl_renderer_s
	engine
	common_components
)

############# fast sky render task ###########

add_library(fast_sky_render_task
	src/mm/opengl/render_tasks/fast_sky_render_task.hpp
	src/mm/opengl/render_tasks/fast_sky_render_task.cpp
)

target_include_directories(fast_sky_render_task PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_link_libraries(fast_sky_render_task
	opengl_renderer_s
	engine
)

########################

if (BUILD_TESTING)
	add_subdirectory(test)
endif()

