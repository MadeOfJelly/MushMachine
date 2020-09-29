#pragma once

#include <memory>
#include <string>

#include <soloud_wav.h>

namespace MM {

	class Engine;

	struct SoundLoaderWavFile {
		std::shared_ptr<::SoLoud::Wav> load(const std::string& path, Engine& engine) const;
	};

} // MM

