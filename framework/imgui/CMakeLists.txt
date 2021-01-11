cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(imgui_lib CXX)

################## imgui_service

add_library(imgui_service
	./src/mm/services/imgui_s.hpp
	./src/mm/services/imgui_s.cpp

	# sneak
	./src/mm/services/imgui_menu_bar.hpp
	./src/mm/services/imgui_menu_bar.cpp
)

target_include_directories(imgui_service PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_link_libraries(imgui_service
	engine
	imgui_render_task
)
################## imgui_widgets

add_library(imgui_widgets
	./src/mm/imgui/widgets/knob.hpp
	./src/mm/imgui/widgets/plot_radar.hpp
	./src/mm/imgui/widgets/camera.hpp
	./src/mm/imgui/widgets/entity.hpp
	./src/mm/imgui/widgets/filesystem.hpp
	./src/mm/imgui/widgets/spritesheet.hpp
	./src/mm/imgui/widgets/texture.hpp
	./src/mm/imgui/widgets/texture_resource_manager.hpp

	./src/mm/imgui/widgets/auto_wrap.hpp

	./src/mm/imgui/widgets/components/name.hpp
	./src/mm/imgui/widgets/components/transform2d.hpp
	./src/mm/imgui/widgets/components/transform3d.hpp
	./src/mm/imgui/widgets/components/velocity2d.hpp
	./src/mm/imgui/widgets/components/view_dir2d.hpp
	./src/mm/imgui/widgets/components/view_dir3d.hpp
	./src/mm/imgui/widgets/components/color.hpp
	./src/mm/imgui/widgets/components/texture.hpp

############

	./src/mm/imgui/widgets/knob.cpp
	./src/mm/imgui/widgets/plot_radar.cpp
	./src/mm/imgui/widgets/camera.cpp
	./src/mm/imgui/widgets/entity.cpp
	./src/mm/imgui/widgets/filesystem.cpp
	./src/mm/imgui/widgets/spritesheet.cpp
	./src/mm/imgui/widgets/texture.cpp
	./src/mm/imgui/widgets/texture_resource_manager.cpp

	./src/mm/imgui/widgets/components/name.cpp
	./src/mm/imgui/widgets/components/transform2d.cpp
	./src/mm/imgui/widgets/components/transform3d.cpp
	./src/mm/imgui/widgets/components/velocity2d.cpp
	./src/mm/imgui/widgets/components/view_dir2d.cpp
	./src/mm/imgui/widgets/components/view_dir3d.cpp
	./src/mm/imgui/widgets/components/color.cpp
	./src/mm/imgui/widgets/components/texture.cpp
)

target_include_directories(imgui_widgets PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_link_libraries(imgui_widgets
	opengl_renderer_s
	imgui_render_task
	engine

	common_components
)

################## imgui_tools

add_library(imgui_tools
	./src/mm/imgui/imgui_entt_entity_editor.hpp
	./src/mm/imgui/fps_overlay.hpp
	./src/mm/imgui/file_text_editor.hpp
	./src/mm/imgui/file_shader_editor.hpp

	./src/mm/imgui/fps_overlay.cpp
	./src/mm/imgui/file_text_editor.cpp
	./src/mm/imgui/file_shader_editor.cpp

	./src/mm/services/scene_tools.hpp
	./src/mm/services/scene_tools.cpp

	./src/mm/services/engine_tools.hpp
	./src/mm/services/engine_tools.cpp
)

target_include_directories(imgui_tools PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_link_libraries(imgui_tools
	imgui_render_task
	imgui_widgets
	imgui_color_text_edit
)

################## imgui_sound

add_library(imgui_sound
	./src/mm/services/sound_tools.hpp
	./src/mm/services/sound_tools.cpp

	./src/mm/imgui/sound_info.hpp
	./src/mm/imgui/sound_pref.hpp

	./src/mm/imgui/widgets/soloud.hpp

	./src/mm/imgui/sound_info.cpp
	./src/mm/imgui/sound_pref.cpp

	# soloud.hpp imps:
	./src/mm/imgui/widgets/soloud_sfxr.cpp
	./src/mm/imgui/widgets/soloud_filter.cpp
)

target_include_directories(imgui_sound PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_link_libraries(imgui_sound
	imgui_render_task
	imgui_widgets
	sound_service
)

################## imgui_tilemap

add_library(imgui_tilemap
	./src/mm/imgui/widgets/components/tilemap_renderable.hpp

	./src/mm/imgui/widgets/components/tilemap_renderable.cpp
)

target_include_directories(imgui_tilemap PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_link_libraries(imgui_tilemap
	imgui_render_task
	imgui_widgets
	tilemap
)

if (BUILD_TESTING)
	add_subdirectory(test)
endif()

