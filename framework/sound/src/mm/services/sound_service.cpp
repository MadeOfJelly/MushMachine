#include "./sound_service.hpp"

#include <string>

#include <mm/logger.hpp>
#define LOG_CRIT(...)		__LOG_CRIT(	"Sound", __VA_ARGS__)
#define LOG_ERROR(...)		__LOG_ERROR("Sound", __VA_ARGS__)
#define LOG_WARN(...)		__LOG_WARN(	"Sound", __VA_ARGS__)
#define LOG_INFO(...)		__LOG_INFO(	"Sound", __VA_ARGS__)
#define LOG_DEBUG(...)		__LOG_DEBUG("Sound", __VA_ARGS__)
#define LOG_TRACE(...)		__LOG_TRACE("Sound", __VA_ARGS__)

namespace MM::Services {

SoundService::SoundService(void) : engine() {
	MM::Logger::initSectionLogger("Sound");
}

SoundService::~SoundService(void) {
}

bool SoundService::enable(Engine&, std::vector<UpdateStrategies::TaskInfo>&) {
	unsigned int flags = SoLoud::Soloud::CLIP_ROUNDOFF;
	auto r = engine.init(flags);
	if (r != 0) {
		LOG_ERROR("SoLoud initialization failed: {}", r);
		return false;
	}

	LOG_INFO("SoLoud v{} backend: {}", engine.getVersion(), engine.getBackendString());

	return true;
}

void SoundService::disable(Engine&) {
	engine.deinit();
}

void SoundService::printErrorString(SoLoud::result errorCode) {
	LOG_ERROR("error string: {}", engine.getErrorString(errorCode));
}

} // MM::Services

