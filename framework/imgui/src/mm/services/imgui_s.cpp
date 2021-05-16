#include "./imgui_s.hpp"

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_sdl.h>

#ifdef MM_OPENGL_3
#include <imgui/backends/imgui_impl_opengl3.h>
#endif

#include <IconsIonicons.h>

#if defined(__GNUC__) || defined(__clang__)
#define USE_BASE85
#endif

#ifdef USE_BASE85
	#include "../../../res/ionicons/ionicons.ttf.base85.h"
#else
	#include "../../../res/ionicons/ionicons.ttf.h"
#endif

#include <tracy/Tracy.hpp>

namespace MM::Services {

bool ImGuiService::enable(Engine& engine, std::vector<UpdateStrategies::TaskInfo>& task_array) {
	IMGUI_CHECKVERSION();

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavNoCaptureKeyboard;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // TODO: dont, if ingame ?

	auto& sdl_ss = engine.getService<MM::Services::SDLService>();
#ifdef MM_OPENGL_3
	ImGui_ImplSDL2_InitForOpenGL(sdl_ss.win, sdl_ss.gl_context);
	ImGui_ImplOpenGL3_Init();
#endif

	// style
	{
		ImGui::StyleColorsDark();

		auto& style = ImGui::GetStyle();
		style.WindowRounding = 2.f;
	}

	// font
	{
		io.Fonts->AddFontDefault();
		static const ImWchar icons_ranges[] = {ICON_MIN_II, ICON_MAX_II, 0};
		ImFontConfig icons_config;
		icons_config.MergeMode = true;
		icons_config.PixelSnapH = true;
		icons_config.FontDataOwnedByAtlas = false;
		icons_config.GlyphOffset = {0.f, 4.f};
		//io.Fonts->AddFontFromMemoryTTF(ionicons_ttf, ionicons_ttf_len, 16.f, &icons_config, icons_ranges);
#ifdef USE_BASE85
		io.Fonts->AddFontFromMemoryCompressedBase85TTF(ionicons_ttf_compressed_data_base85, 16.f*1.3f, &icons_config, icons_ranges);
#else
		io.Fonts->AddFontFromMemoryTTF(ionicons_ttf, ionicons_ttf_len, 16.f, &icons_config, icons_ranges);
#endif
	}

	_event_handle = sdl_ss.addEventHandler(
		[this](const SDL_Event& e) { return handle_sdl_event(e); }
	);

	// add task
	task_array.push_back(
		UpdateStrategies::TaskInfo{"ImGuiService::new_frame"}
		.phase(UpdateStrategies::update_phase_t::PRE)
		.fn([this](Engine& e) { this->imgui_new_frame(e); })
		.succeed("SDLService::events")
	);

	return true;
}

void ImGuiService::disable(Engine& engine) {
	auto& sdl_ss = engine.getService<MM::Services::SDLService>();
	sdl_ss.removeEventHandler(_event_handle);

	ImGui::EndFrame(); // making sure, does not work????

#ifdef MM_OPENGL_3
	ImGui_ImplOpenGL3_Shutdown();
#endif
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}

void ImGuiService::imgui_new_frame(Engine& engine) {
	ZoneScopedN("MM::Services::ImGuiService::imgui_new_frame");

#ifdef MM_OPENGL_3
	ImGui_ImplOpenGL3_NewFrame();
#endif

	ImGui_ImplSDL2_NewFrame(engine.getService<MM::Services::SDLService>().win);
	ImGui::NewFrame();
}

bool ImGuiService::handle_sdl_event(const SDL_Event& e) {
	if (!ImGui_ImplSDL2_ProcessEvent(&e)) {
		return false; // if the event was not relevant to imgui anyway
	}

	auto& io = ImGui::GetIO();
	// keyboard
	if ((
			e.type == SDL_KEYDOWN ||
			e.type == SDL_KEYUP ||
			e.type == SDL_TEXTEDITING ||
			e.type == SDL_TEXTINPUT
		)
		&& io.WantCaptureKeyboard) {
		return true;
	}
	// mouse
	if ((
			e.type == SDL_MOUSEMOTION ||
			e.type == SDL_MOUSEBUTTONUP ||
			e.type == SDL_MOUSEBUTTONDOWN ||
			e.type == SDL_MOUSEWHEEL // ????
		)
		&& io.WantCaptureMouse) {
		return true;
	}
	//// controller ???
	//if ((
			//e.type == SDL_CONTROLLERBUTTONUP ||
			//e.type == SDL_CONTROLLERBUTTONDOWN
		//)[>
		//&& io.WantCaptureKeyboard*/) {
		//return false;
	//}

	return false;
}

} // namespace MM::Services

