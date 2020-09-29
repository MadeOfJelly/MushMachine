#pragma once

#define SPDLOG_ACTIVE_LEVEL 0

#include <memory>
#include <spdlog/spdlog.h>

// thos are helpers so you can do "#define LOG_ERROR(...) __LOG_ERROR("Engine", ...)
#define __LOG_CRIT(s, ...)		SPDLOG_LOGGER_CRITICAL(	spdlog::get(s), __VA_ARGS__)
#define __LOG_ERROR(s, ...)		SPDLOG_LOGGER_ERROR(	spdlog::get(s), __VA_ARGS__)
#define __LOG_WARN(s, ...)		SPDLOG_LOGGER_WARN(		spdlog::get(s), __VA_ARGS__)
#define __LOG_INFO(s, ...)		SPDLOG_LOGGER_INFO(		spdlog::get(s), __VA_ARGS__)
#define __LOG_DEBUG(s, ...)		SPDLOG_LOGGER_DEBUG(	spdlog::get(s), __VA_ARGS__)
#define __LOG_TRACE(s, ...)		SPDLOG_LOGGER_TRACE(	spdlog::get(s), __VA_ARGS__)

// copy paste me to start
//#define LOG_CRIT(...)		__LOG_CRIT(	"Engine", __VA_ARGS__)
//#define LOG_ERROR(...)		__LOG_ERROR("Engine", __VA_ARGS__)
//#define LOG_WARN(...)		__LOG_WARN(	"Engine", __VA_ARGS__)
//#define LOG_INFO(...)		__LOG_INFO(	"Engine", __VA_ARGS__)
//#define LOG_DEBUG(...)		__LOG_DEBUG("Engine", __VA_ARGS__)
//#define LOG_TRACE(...)		__LOG_TRACE("Engine", __VA_ARGS__)

// copy paste me to end if in header
//#undef LOG_CRIT
//#undef LOG_ERROR
//#undef LOG_WARN
//#undef LOG_INFO
//#undef LOG_DEBUG
//#undef LOG_TRACE


// DONT USE! this exists only for legacy reasons!
#define LOG(s, x) SPDLOG_WARN("[!][{}] {}", s, x)
//#define LOG(s, x) MM::Logger::log(MM::Logger::file_name(__FILE__), __LINE__, s, x)

namespace MM {

	struct Logger {
		static bool initialized;

		//static std::shared_ptr<spdlog::sinks::sink> _sink_console;
		static spdlog::sink_ptr _sink_console;
		static spdlog::sink_ptr _sink_file;

#ifdef TRACY_ENABLE
		static spdlog::sink_ptr _sink_tracy;
#endif

		static void init(void);

		static void initSectionLogger(const char* section, bool log_to_stdio = true);
	};

} // namespace MM

