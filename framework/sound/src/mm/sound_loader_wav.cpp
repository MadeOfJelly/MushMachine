#include "./sound_loader_wav.hpp"

#include "./soloud_filesystem_file_impl.hpp"

#include <mm/services/filesystem.hpp>

namespace MM {

std::shared_ptr<::SoLoud::Wav> SoundLoaderWavFile::load(const std::string& path, Engine& engine) const {
	auto& fs = engine.getService<Services::FilesystemService>();

	if (!fs.isFile(path.c_str()))
		return nullptr;

	auto h = fs.open(path.c_str());

	MM::SoLoud::FilesystemFile sl_f(h, engine);

	auto ptr = std::make_shared<::SoLoud::Wav>();
	auto r = ptr->loadFile(&sl_f);
	if (r != ::SoLoud::SO_NO_ERROR) {
		// log error
		return nullptr;
	}

	return ptr;
}

} // MM

