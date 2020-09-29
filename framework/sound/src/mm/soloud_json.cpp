#include "./soloud_json.hpp"
#include "nlohmann/json_fwd.hpp"

#include <nlohmann/json.hpp>

namespace SoLoud {

void to_json(nlohmann::json& j, const Sfxr& sfxr) {
	j = nlohmann::json::object();

#define X(x) j[#x] = sfxr.mParams.x
	X(wave_type);

	X(p_base_freq);
	X(p_freq_limit);
	X(p_freq_ramp);
	X(p_freq_dramp);
	X(p_duty);
	X(p_duty_ramp);

	X(p_vib_strength);
	X(p_vib_speed);
	X(p_vib_delay);

	X(p_env_attack);
	X(p_env_sustain);
	X(p_env_decay);
	X(p_env_punch);

	X(filter_on);
	X(p_lpf_resonance);
	X(p_lpf_freq);
	X(p_lpf_ramp);
	X(p_hpf_freq);
	X(p_hpf_ramp);

	X(p_pha_offset);
	X(p_pha_ramp);

	X(p_repeat_speed);

	X(p_arp_speed);
	X(p_arp_mod);

	X(master_vol);

	X(sound_vol);
#undef X
}

void from_json(const nlohmann::json& j, Sfxr& sfxr) {
	if (j.empty()) {
		sfxr.resetParams();
	}

	if (j.contains("preset")) {
		sfxr.loadPreset(j["preset"].get<int>(), j.value("preset_seed", 0));
	}

#define X(x) if (j.contains(#x)) sfxr.mParams.x = j[#x]
	X(wave_type);

	X(p_base_freq);
	X(p_freq_limit);
	X(p_freq_ramp);
	X(p_freq_dramp);
	X(p_duty);
	X(p_duty_ramp);

	X(p_vib_strength);
	X(p_vib_speed);
	X(p_vib_delay);

	X(p_env_attack);
	X(p_env_sustain);
	X(p_env_decay);
	X(p_env_punch);

	X(filter_on);
	X(p_lpf_resonance);
	X(p_lpf_freq);
	X(p_lpf_ramp);
	X(p_hpf_freq);
	X(p_hpf_ramp);

	X(p_pha_offset);
	X(p_pha_ramp);

	X(p_repeat_speed);

	X(p_arp_speed);
	X(p_arp_mod);

	X(master_vol);

	X(sound_vol);
#undef X

}

} // SoLoud

