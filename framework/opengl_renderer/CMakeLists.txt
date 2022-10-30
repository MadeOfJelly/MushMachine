cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

project(opengl_renderer CXX)

add_library(opengl_renderer_s
	src/mm/opengl/render_task.hpp

	src/mm/services/opengl_renderer.hpp
	src/mm/services/opengl_renderer.cpp

	src/mm/opengl/camera_3d.hpp
	src/mm/opengl/camera_3d.cpp

	src/mm/opengl/res/shaders_builtin.hpp
	src/mm/opengl/res/shaders_builtin.cpp
)

target_include_directories(opengl_renderer_s PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_link_libraries(opengl_renderer_s
	engine
	logger

	opengl_primitives
	sdl_service
)

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

############# bloom_extraction render task ###########

add_library(bloom_extraction_render_task
	src/mm/opengl/render_tasks/bloom_extraction.hpp
	src/mm/opengl/render_tasks/bloom_extraction.cpp
)

target_include_directories(bloom_extraction_render_task PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_link_libraries(bloom_extraction_render_task
	opengl_renderer_s
	engine
)

############# bloom_combine render task ###########

add_library(bloom_combine_render_task
	src/mm/opengl/render_tasks/bloom_combine.hpp
	src/mm/opengl/render_tasks/bloom_combine.cpp
)

target_include_directories(bloom_combine_render_task PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_link_libraries(bloom_combine_render_task
	opengl_renderer_s
	engine
)

############# composition render task ###########
# intendet for bloom compositing and tonemapping

add_library(composition_render_task
	src/mm/opengl/render_tasks/composition.hpp
	src/mm/opengl/render_tasks/composition.cpp
)

target_include_directories(composition_render_task PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_link_libraries(composition_render_task
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

############# bloom ###########

add_library(bloom
	src/mm/opengl/bloom.hpp
	src/mm/opengl/bloom.cpp
)

target_include_directories(bloom PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_link_libraries(bloom
	bloom_extraction_render_task
	blur_render_task
	bloom_combine_render_task
)

############# lite_particles2d ###########

add_library(lite_particles2d
	src/mm/opengl/lite_particles2d_type.hpp
	src/mm/opengl/lite_particles2d_type_loader.hpp
	src/mm/opengl/lite_particles2d_type_loader.cpp

	src/mm/opengl/components/lite_particles2d.hpp

	src/mm/opengl/render_tasks/lite_particles2d.hpp
	src/mm/opengl/render_tasks/lite_particles2d.cpp
)

target_include_directories(lite_particles2d PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_link_libraries(lite_particles2d
	engine
	opengl_renderer_s
	common_components_serialize_json # glm serl
)

########################

if (BUILD_TESTING)
	add_subdirectory(test)
endif()

