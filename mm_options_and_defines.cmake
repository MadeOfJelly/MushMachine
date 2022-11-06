
# include guard
if(NOT MM_INTERNAL_OPTIONS)
	set(MM_INTERNAL_OPTIONS TRUE)

	# MM options
	option(BUILD_TESTING "Enable testing with ctest." OFF)
	option(MM_AUTOTEST "Quits tests after some frames. for ci." ON)
	option(MM_HEADLESS "Headless mode for MM, disables all kind of rendering stuff." OFF)
	option(MM_NETWORKING "control networking implementations for MM." OFF)

	if(NOT MM_HEADLESS)
		option(MM_OPENGL_3 "use the MM opengl 3 implementations (opengl 3.3 or opengl es 3.0 (webgl 2)" ON)
		option(MM_OPENGL_3_GLES "use gles over gl" OFF)
		if(NOT MM_OPENGL_3 AND MM_OPENGL_3_GLES)
			message(WARN "MM_OPENGL_3_GLES on, but MM_OPENGL_3 off, ignoring")
			set(MM_OPENGL_3_GLES OFF)
		endif()
	endif()

	# MM options defines
	macro(GEN_COMPILE_DEFINITION OPTION_NAME)
		if(${OPTION_NAME})
			add_definitions("-D${OPTION_NAME}")
			#set_property(GLOBAL PROPERTY COMPILE_DEFINITIONS "-D${OPTION_NAME}")
		endif()
	endmacro()

	gen_compile_definition(MM_AUTOTEST)
	gen_compile_definition(MM_HEADLESS)
	gen_compile_definition(MM_NETWORKING)
	gen_compile_definition(MM_OPENGL_3)
	gen_compile_definition(MM_OPENGL_3_GLES)
endif()

