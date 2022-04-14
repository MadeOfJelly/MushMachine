#include "./lite_particles2d_type_loader.hpp"

#include <mm/services/filesystem.hpp>

#include <mm/logger.hpp>

namespace MM::OpenGL {

std::shared_ptr<LiteParticles2DType> LiteParticles2DTypeLoaderJson::load(const nlohmann::json& j) const {
	auto new_type = std::make_shared<LiteParticles2DType>();

	try {
		*new_type = j;
	} catch (...) {
		SPDLOG_ERROR("failed parsing particle type json:\n{}", j.dump(2));
		return nullptr;
	}

	return new_type;
}

std::shared_ptr<LiteParticles2DType> LiteParticles2DTypeLoaderFile::load(MM::Engine& engine, const std::string& path) const {
	auto* fs = engine.tryService<MM::Services::FilesystemService>();
	if (fs == nullptr) {
		// TODO: error
		return nullptr;
	}

	auto new_particle = LiteParticles2DTypeLoaderJson{}.load(fs->readJson(path.c_str()));

	if (!new_particle) {
		SPDLOG_ERROR("particle type file: '{}'", path);
	}

	return new_particle;
}

} // MM::OpenGL

