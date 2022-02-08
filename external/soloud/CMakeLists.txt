cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

project(soloud CXX)

set(CPP_FILES
	#core
	soloud/src/core/soloud.cpp
	soloud/src/core/soloud_audiosource.cpp
	soloud/src/core/soloud_bus.cpp
	soloud/src/core/soloud_core_3d.cpp
	soloud/src/core/soloud_core_basicops.cpp
	soloud/src/core/soloud_core_faderops.cpp
	soloud/src/core/soloud_core_filterops.cpp
	soloud/src/core/soloud_core_getters.cpp
	soloud/src/core/soloud_core_setters.cpp
	soloud/src/core/soloud_core_voicegroup.cpp
	soloud/src/core/soloud_core_voiceops.cpp
	soloud/src/core/soloud_fader.cpp
	soloud/src/core/soloud_fft.cpp
	soloud/src/core/soloud_fft_lut.cpp
	soloud/src/core/soloud_file.cpp
	soloud/src/core/soloud_filter.cpp
	soloud/src/core/soloud_misc.cpp
	soloud/src/core/soloud_queue.cpp
	soloud/src/core/soloud_thread.cpp


	#backends
	soloud/src/backend/alsa/soloud_alsa.cpp
	soloud/src/backend/coreaudio/soloud_coreaudio.cpp
	soloud/src/backend/null/soloud_null.cpp
	#soloud/src/backend/openal/soloud_openal.cpp
	#soloud/src/backend/openal/soloud_openal_dll.c
	soloud/src/backend/opensles/soloud_opensles.cpp
	#soloud/src/backend/oss/soloud_oss.cpp
	#soloud/src/backend/portaudio/soloud_portaudio.cpp
	#soloud/src/backend/portaudio/soloud_portaudio_dll.c
	#soloud/src/backend/sdl/soloud_sdl1.cpp
	#soloud/src/backend/sdl/soloud_sdl1_dll.c
	#soloud/src/backend/sdl/soloud_sdl2.cpp
	#soloud/src/backend/sdl/soloud_sdl2_dll.c
	#soloud/src/backend/sdl_static/soloud_sdl_static.cpp
	soloud/src/backend/sdl2_static/soloud_sdl2_static.cpp
	#soloud/src/backend/vita_homebrew/soloud_vita_homebrew.cpp
	soloud/src/backend/wasapi/soloud_wasapi.cpp
	soloud/src/backend/winmm/soloud_winmm.cpp
	soloud/src/backend/xaudio2/soloud_xaudio2.cpp

	#filter
	soloud/src/filter/soloud_bassboostfilter.cpp
	soloud/src/filter/soloud_biquadresonantfilter.cpp
	soloud/src/filter/soloud_dcremovalfilter.cpp
	soloud/src/filter/soloud_echofilter.cpp
	soloud/src/filter/soloud_fftfilter.cpp
	soloud/src/filter/soloud_freeverbfilter.cpp
	soloud/src/filter/soloud_flangerfilter.cpp
	soloud/src/filter/soloud_lofifilter.cpp
	soloud/src/filter/soloud_robotizefilter.cpp
	soloud/src/filter/soloud_waveshaperfilter.cpp


	#sources
	#monotone
	soloud/src/audiosource/monotone/soloud_monotone.cpp

	#noise
	soloud/src/audiosource/noise/soloud_noise.cpp

	#openmpt
	soloud/src/audiosource/openmpt/soloud_openmpt.cpp
	soloud/src/audiosource/openmpt/soloud_openmpt_dll.c

	#sfxr
	soloud/src/audiosource/sfxr/soloud_sfxr.cpp

	#speech
	soloud/src/audiosource/speech/darray.cpp
	soloud/src/audiosource/speech/darray.h
	#soloud/src/audiosource/speech/Elements.def
	soloud/src/audiosource/speech/klatt.cpp
	soloud/src/audiosource/speech/klatt.h
	soloud/src/audiosource/speech/resonator.cpp
	soloud/src/audiosource/speech/resonator.h
	soloud/src/audiosource/speech/soloud_speech.cpp
	soloud/src/audiosource/speech/tts.cpp
	soloud/src/audiosource/speech/tts.h

	#tedsid
	soloud/src/audiosource/tedsid/sid.cpp
	soloud/src/audiosource/tedsid/sid.h
	soloud/src/audiosource/tedsid/soloud_tedsid.cpp
	soloud/src/audiosource/tedsid/ted.cpp
	soloud/src/audiosource/tedsid/ted.h

	#vic
	soloud/src/audiosource/vic/soloud_vic.cpp

	#vizsn
	soloud/src/audiosource/vizsn/soloud_vizsn.cpp

	#wav
	soloud/src/audiosource/wav/soloud_wav.cpp
	soloud/src/audiosource/wav/soloud_wavstream.cpp
	# TODO: move to extra lib
	soloud/src/audiosource/wav/dr_flac.h
	soloud/src/audiosource/wav/dr_impl.cpp
	soloud/src/audiosource/wav/dr_mp3.h
	soloud/src/audiosource/wav/dr_wav.h
	soloud/src/audiosource/wav/stb_vorbis.c
	soloud/src/audiosource/wav/stb_vorbis.h
)

set(HPP_FILES
	#c_api
	#soloud/include/soloud_c.h

	#core
	soloud/include/soloud.h
	soloud/include/soloud_internal.h
	soloud/include/soloud_audiosource.h
	soloud/include/soloud_bus.h
	soloud/include/soloud_error.h
	soloud/include/soloud_fader.h
	soloud/include/soloud_fft.h
	soloud/include/soloud_file.h
	soloud/include/soloud_file_hack_off.h
	soloud/include/soloud_file_hack_on.h
	soloud/include/soloud_filter.h
	soloud/include/soloud_queue.h
	soloud/include/soloud_thread.h

	#filter
	soloud/include/soloud_bassboostfilter.h
	soloud/include/soloud_biquadresonantfilter.h
	soloud/include/soloud_dcremovalfilter.h
	soloud/include/soloud_echofilter.h
	soloud/include/soloud_fftfilter.h
	soloud/include/soloud_flangerfilter.h
	soloud/include/soloud_lofifilter.h
	soloud/include/soloud_robotizefilter.h
	soloud/include/soloud_waveshaperfilter.h


	#sources
	soloud/include/soloud_wav.h
	soloud/include/soloud_wavstream.h
	soloud/include/soloud_monotone.h
	soloud/include/soloud_openmpt.h
	soloud/include/soloud_sfxr.h
	soloud/include/soloud_speech.h
	soloud/include/soloud_tedsid.h
	soloud/include/soloud_vic.h
	soloud/include/soloud_vizsn.h
)

add_library(soloud ${CPP_FILES} ${HPP_FILES})

target_compile_definitions(soloud PRIVATE WITH_NULL)
#target_compile_definitions(soloud PRIVATE WITH_SDL2)
target_compile_definitions(soloud PRIVATE WITH_SDL2_STATIC)

target_include_directories(soloud SYSTEM PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/soloud/include")

#if not android or emscripten
if(VCPKG_TARGET_TRIPLET)
	target_link_libraries(soloud SDL2::SDL2 SDL2::SDL2main SDL2::SDL2-static)
else()
	target_include_directories(soloud PUBLIC "${SDL2_INCLUDE_DIR}")
	target_link_libraries(soloud ${SDL2_LIBRARY})
endif()

#if android
#target_link_libraries(soloud SDL)
#endif

if(EMSCRIPTEN)
	set_target_properties(soloud PROPERTIES COMPILE_FLAGS "-s USE_SDL=2")
	set_target_properties(soloud PROPERTIES LINK_FLAGS "-s USE_SDL=2")
endif()


# TODO: remove
if(UNIX)
	target_link_libraries(soloud
		-ldl
	)
endif()

add_executable(soloud_demo_simplest EXCLUDE_FROM_ALL soloud/demos/simplest/main.cpp)
target_link_libraries(soloud_demo_simplest
	soloud
)

add_executable(soloud_demo_enumerate EXCLUDE_FROM_ALL ./soloud/demos/enumerate/main.cpp)
target_link_libraries(soloud_demo_enumerate
	soloud
)

add_executable(soloud_demo_welcome EXCLUDE_FROM_ALL ./soloud/demos/welcome/main.cpp)
target_link_libraries(soloud_demo_welcome
	soloud
)

