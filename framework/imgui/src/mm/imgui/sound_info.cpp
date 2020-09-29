#include "./sound_info.hpp"

#include <imgui/imgui.h>

#include <mm/services/sound_service.hpp>

#include <mm/engine.hpp>

namespace MM {

	void ImGuiSoundInfo(Engine& engine) {
		if (ImGui::Begin("Sound Info")) {
			auto& sound = engine.getService<MM::Services::SoundService>();

			ImGui::Text("SoLoud v%d", sound.engine.getVersion());
			ImGui::Text("Backend: %s, ch: %d, rate: %d, buffersize: %d",
					sound.engine.getBackendString(),
					sound.engine.getBackendChannels(),
					sound.engine.getBackendSamplerate(),
					sound.engine.getBackendBufferSize());
			ImGui::Text("Max Active Voice Count: %d", sound.engine.getMaxActiveVoiceCount());

			ImGui::Text("Active Voice Count: %d", sound.engine.getActiveVoiceCount());

			sound.engine.getWave();
			ImGui::PlotLines("##Wave", sound.engine.getWave(), 256, 0, "Wave", -1.f, 1.f, ImVec2(0, 80));
			ImGui::PlotHistogram("##Spectrum", sound.engine.calcFFT(), 256 / 2, 0, "FFT", 0.f, 20.f, ImVec2(0, 80));
		}
		ImGui::End();
	}

} // MM

