#pragma once

#include <memory>
#include <string>

#include <soloud_sfxr.h>

#include <nlohmann/json_fwd.hpp>

namespace MM {

	// fwd
	class Engine;

	struct SoundLoaderSfxrPreset {
		std::shared_ptr<::SoLoud::Sfxr> load(const ::SoLoud::Sfxr::SFXR_PRESETS preset, const int seed) const;
	};

	struct SoundLoaderSfxrJson {
		std::shared_ptr<::SoLoud::Sfxr> load(const nlohmann::json& j) const;
	};

	struct SoundLoaderSfxrFile {
		std::shared_ptr<::SoLoud::Sfxr> load(const std::string& path, MM::Engine& engine) const;
	};

} // MM

