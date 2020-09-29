#pragma once

#include <nlohmann/json_fwd.hpp>

#include <soloud_sfxr.h>

namespace SoLoud {

	//SoLoud::Sfxr
	void to_json(nlohmann::json& j, const Sfxr& sfxr);
	void from_json(const nlohmann::json& j, Sfxr& sfxr);

} // SoLoud

