#include "./sound_loader_sfxr.hpp"

#include <nlohmann/json.hpp>

#include "./soloud_json.hpp"

#include <mm/services/filesystem.hpp>


namespace MM {

std::shared_ptr<::SoLoud::Sfxr> SoundLoaderSfxrPreset::load(const ::SoLoud::Sfxr::SFXR_PRESETS preset, const int seed) const {
	auto sfxr = std::make_shared<::SoLoud::Sfxr>();
	sfxr->loadPreset(preset, seed);
	return sfxr;
}

std::shared_ptr<::SoLoud::Sfxr> SoundLoaderSfxrJson::load(const nlohmann::json& j) const {
	auto sfxr = std::make_shared<::SoLoud::Sfxr>();
	*sfxr = j;
	return sfxr;
}

std::shared_ptr<::SoLoud::Sfxr> SoundLoaderSfxrFile::load(const std::string& path, MM::Engine& engine) const {
	auto& fs = engine.getService<MM::Services::FilesystemService>();

	if (!fs.isFile(path.c_str())) {
		// TODO: log error
		return nullptr;
	}

	auto j = fs.readJson(path.c_str());

	return SoundLoaderSfxrJson{}.load(j);
}

} // MM

