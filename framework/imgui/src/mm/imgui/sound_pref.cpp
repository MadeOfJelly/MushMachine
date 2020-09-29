#include "sound_pref.hpp"

#include <imgui/imgui.h>

#include <mm/services/sound_service.hpp>

namespace MM {

	void ImGuiSoundPref(Engine& engine) {
		if (ImGui::Begin("Sound Preferences")) {
			auto& e = engine.getService<MM::Services::SoundService>().engine;

			auto gvolume = e.getGlobalVolume();
			ImGui::SliderFloat("Global Volume", &gvolume, 0.f, 1.f);
			e.setGlobalVolume(gvolume);
		}
		ImGui::End();
	}

} // MM

