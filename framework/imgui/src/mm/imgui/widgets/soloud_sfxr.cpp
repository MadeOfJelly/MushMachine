#include "./soloud.hpp"

#include <mm/imgui/widgets/knob.hpp>

#include <imgui/imgui.h>

#include <soloud_sfxr.h>

namespace MM::ImGuiWidgets {

bool SoLoudSfxrPlain(const char* label, SoLoud::SfxrParams* sfxr_params) {
	ImGui::PushID(label);

	bool value_changed = false;

	{
		const char* wlist[] {"SquareWave", "SawTooth", "SineWave", "Noise"};
		if (ImGui::BeginCombo("Wave Type", wlist[sfxr_params->wave_type])) {
			for (int i = 0; i < 4; i++) {
				if (ImGui::Selectable(wlist[i])) {
					sfxr_params->wave_type = i;
					value_changed = true;
				}
			}
			ImGui::EndCombo();
		}
	}

	ImGui::Separator();
	//internal_sec1();
	{
		value_changed |= ImGui::SliderFloat("Attack Time", &sfxr_params->p_env_attack, 0.f, 1.f);
		value_changed |= ImGui::SliderFloat("Sustain Time", &sfxr_params->p_env_sustain, 0.f, 1.f);
		value_changed |= ImGui::SliderFloat("Sustain Punch", &sfxr_params->p_env_punch, 0.f, 1.f);
		value_changed |= ImGui::SliderFloat("Decay Time", &sfxr_params->p_env_decay, 0.f, 1.f);
	}

	ImGui::Separator();
	//internal_sec2();
	{
		value_changed |= ImGui::SliderFloat("Start Freq", &sfxr_params->p_base_freq, 0.f, 1.f);
		value_changed |= ImGui::SliderFloat("Min Freq", &sfxr_params->p_freq_limit, 0.f, 1.f);

		value_changed |= ImGui::SliderFloat("Slide", &sfxr_params->p_freq_ramp, -1.f, 1.f);
		value_changed |= ImGui::SliderFloat("Slide Delta", &sfxr_params->p_freq_dramp, -1.f, 1.f);

		value_changed |= ImGui::SliderFloat("Vibrato Depth", &sfxr_params->p_vib_strength, 0.f, 1.f);
		value_changed |= ImGui::SliderFloat("Vibrato Speed", &sfxr_params->p_vib_speed, 0.f, 1.f);
		//ImGui::SliderFloat("Vibrato Delay", &sfxr_params->p_vib_delay, 0.f, 1.f); // useless?
	}

	ImGui::Separator();
	//internal_sec3();
	{
		value_changed |= ImGui::SliderFloat("Change Amount", &sfxr_params->p_arp_mod, -1.f, 1.f);
		value_changed |= ImGui::SliderFloat("Change Speed", &sfxr_params->p_arp_speed, 0.f, 1.f);
	}

	ImGui::Separator();
	//internal_sec4();
	{
		value_changed |= ImGui::SliderFloat("Square Duty", &sfxr_params->p_duty, 0.f, 1.f);
		value_changed |= ImGui::SliderFloat("Duty Sweep", &sfxr_params->p_duty_ramp, -1.f, 1.f);
	}

	ImGui::Separator();
	//internal_sec5();
	{
		value_changed |= ImGui::SliderFloat("Repeat Speed", &sfxr_params->p_repeat_speed, 0.f, 1.f);
	}

	ImGui::Separator();
	//internal_sec6();
	{
		value_changed |= ImGui::SliderFloat("Phaser Offset", &sfxr_params->p_pha_offset, -1.f, 1.f);
		value_changed |= ImGui::SliderFloat("Phaser Sweep", &sfxr_params->p_pha_ramp, -1.f, 1.f);
	}

	ImGui::Separator();
	//internal_sec7();
	{
		value_changed |= ImGui::SliderFloat("LP Filter Cutoff", &sfxr_params->p_lpf_freq, 0.f, 1.f);
		value_changed |= ImGui::SliderFloat("LP Filter Cutoff Sweep", &sfxr_params->p_lpf_ramp, -1.f, 1.f);
		value_changed |= ImGui::SliderFloat("LP Filter Resonace", &sfxr_params->p_lpf_resonance, 0.f, 1.f);
		value_changed |= ImGui::SliderFloat("HP Filter Cutoff", &sfxr_params->p_hpf_freq, 0.f, 1.f);
		value_changed |= ImGui::SliderFloat("HP Filter Cutoff Sweep", &sfxr_params->p_hpf_ramp, -1.f, 1.f);
	}

	ImGui::PopID();
	return value_changed;
}

bool SoLoudSfxrFancy(const char* label, SoLoud::SfxrParams* sfxr_params) {
	ImGui::PushID(label);

	bool value_changed = false;

	{
		const char* wlist[] {"SquareWave", "SawTooth", "SineWave", "Noise"};
		if (ImGui::BeginCombo("Wave Type", wlist[sfxr_params->wave_type])) {
			for (int i = 0; i < 4; i++) {
				if (ImGui::Selectable(wlist[i])) {
					sfxr_params->wave_type = i;
					value_changed = true;
				}
			}
			ImGui::EndCombo();
		}
	}

	// square wave mods
	if (sfxr_params->wave_type == 0) {
		if (ImGui::BeginChild("SquareWaveMod", ImVec2(105, 90), true)) {
			ImGui::TextDisabled("SQW Mod");

			value_changed |= KnobFloat("Duty", &sfxr_params->p_duty, 0.f, 1.f, 400.f);
			ImGui::SameLine();
			value_changed |= KnobFloat("Sweep", &sfxr_params->p_duty_ramp, -1.f, 1.f, 400.f);
		}
		ImGui::EndChild(); // SquareWaveMod
	}

	// +-------------+----+
	// |EG           |VOL |
	// |  O  O  O  O |  O |
	// +----------+--+-+--+
	// |FQ        |ARP |
	// |  O  O  O |  O |
	// |  O  O  O |  O |
	// +----------+----+

	if (ImGui::BeginChild("EG", ImVec2(200, 90), true)) {
		ImGui::TextDisabled("EG");

		value_changed |= KnobFloat("Attack", &sfxr_params->p_env_attack, 0.f, 1.f, 400.f);
		ImGui::SameLine();
		value_changed |= KnobFloat("Sustain", &sfxr_params->p_env_sustain, 0.f, 1.f, 400.f);
		ImGui::SameLine();
		value_changed |= KnobFloat("Punch", &sfxr_params->p_env_punch, 0.f, 1.f, 400.f);
		ImGui::SameLine();
		value_changed |= KnobFloat("Decay", &sfxr_params->p_env_decay, 0.f, 1.f, 400.f);
	}
	ImGui::EndChild(); // EG

	ImGui::SameLine();

	if (ImGui::BeginChild("Vol", ImVec2(58, 90), true)) {
		ImGui::TextDisabled("Vol");

		value_changed |= KnobFloat("Volume", &sfxr_params->sound_vol, 0.f, 1.f, 400.f);
	}
	ImGui::EndChild(); // Vol

	// next line

	if (ImGui::BeginChild("Frequency", ImVec2(58, 152), true)) {
		ImGui::TextDisabled("Freq");
		value_changed |= KnobFloat("Base", &sfxr_params->p_base_freq, 0.f, 1.f, 400.f);
		value_changed |= KnobFloat("Limit", &sfxr_params->p_freq_limit, 0.f, 1.f, 400.f);
	}
	ImGui::EndChild();

	ImGui::SameLine();

	if (ImGui::BeginChild("Slide", ImVec2(58, 152), true)) {
		ImGui::TextDisabled("Slide");
		value_changed |= KnobFloat("Slide", &sfxr_params->p_freq_ramp, -1.f, 1.f, 400.f);
		value_changed |= KnobFloat("Delta", &sfxr_params->p_freq_dramp, -1.f, 1.f, 400.f);
	}
	ImGui::EndChild();

	ImGui::SameLine();

	if (ImGui::BeginChild("Vibrato", ImVec2(58, 152), true)) {
		ImGui::TextDisabled("Vib");
		value_changed |= KnobFloat("Depth", &sfxr_params->p_vib_strength, 0.f, 1.f, 400.f);
		value_changed |= KnobFloat("Speed", &sfxr_params->p_vib_speed, 0.f, 1.f, 400.f);
	}
	ImGui::EndChild();

	ImGui::SameLine();

	if (ImGui::BeginChild("Arp", ImVec2(58, 152), true)) {
		ImGui::TextDisabled("Arp");
		value_changed |= KnobFloat("Amount", &sfxr_params->p_arp_mod, -1.f, 1.f, 400.f);
		value_changed |= KnobFloat("Speed", &sfxr_params->p_arp_speed, 0.f, 1.f, 400.f);
	}
	ImGui::EndChild(); // Arp

	// next line

	if (ImGui::BeginChild("Repeat", ImVec2(58, 90), true)) {
		ImGui::TextDisabled("Repeat");
		value_changed |= KnobFloat("Speed", &sfxr_params->p_repeat_speed, 0.f, 1.f, 400.f);
	}
	ImGui::EndChild(); // Repeat

	ImGui::SameLine();

	if (ImGui::BeginChild("Phaser", ImVec2(105, 90), true)) {
		ImGui::TextDisabled("Phaser");
		value_changed |= KnobFloat("Offset", &sfxr_params->p_pha_offset, -1.f, 1.f, 400.f);
		ImGui::SameLine();
		value_changed |= KnobFloat("Sweep", &sfxr_params->p_pha_ramp, -1.f, 1.f, 400.f);
	}
	ImGui::EndChild(); // Phaser

	// next line

	if (ImGui::BeginChild("LP Filter", ImVec2(152, 90), true)) {
		ImGui::TextDisabled("LP Filter");
		value_changed |= KnobFloat("Cutoff", &sfxr_params->p_lpf_freq, 0.f, 1.f, 400.f);
		ImGui::SameLine();
		value_changed |= KnobFloat("Sweep", &sfxr_params->p_lpf_ramp, -1.f, 1.f, 400.f);
		ImGui::SameLine();
		value_changed |= KnobFloat("Peak", &sfxr_params->p_lpf_resonance, 0.f, 1.f, 400.f);
	}
	ImGui::EndChild(); // LP Filter

	ImGui::SameLine();

	if (ImGui::BeginChild("HP Filter", ImVec2(105, 90), true)) {
		ImGui::TextDisabled("HP Filter");
		value_changed |= KnobFloat("Cutoff", &sfxr_params->p_hpf_freq, 0.f, 1.f, 400.f);
		ImGui::SameLine();
		value_changed |= KnobFloat("Sweep", &sfxr_params->p_hpf_ramp, -1.f, 1.f, 400.f);
	}
	ImGui::EndChild(); // HP Filter

	ImGui::PopID();
	return value_changed;
}

} // MM::ImGuiWidgets

