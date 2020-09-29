#include "./logger.hpp"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "./tracy_sink.hpp"
#include "spdlog/spdlog.h"

namespace MM {
	bool Logger::initialized = false;

	spdlog::sink_ptr Logger::_sink_console = nullptr;
	spdlog::sink_ptr Logger::_sink_file = nullptr;

#ifdef TRACY_ENABLE
	spdlog::sink_ptr Logger::_sink_tracy = nullptr;
#endif


	void Logger::init(void) {
		_sink_console = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		_sink_console->set_level(spdlog::level::info);
		//console_sink->set_pattern("[multi_sink_example] [%^%l%$] %v");

		_sink_file = std::make_shared<spdlog::sinks::basic_file_sink_mt>("mm_log.txt", true);
		_sink_file->set_level(spdlog::level::trace);

#ifdef TRACY_ENABLE
		_sink_tracy = std::make_shared<spdlog::sinks::tracy_sink_mt>();
		_sink_tracy->set_level(spdlog::level::trace);
#endif

		spdlog::set_default_logger(
			std::make_shared<spdlog::logger>("MushMachine", spdlog::sinks_init_list(
				{_sink_console, _sink_file
#ifdef TRACY_ENABLE
				, _sink_tracy
#endif
				}
			))
		);

		spdlog::default_logger_raw()->set_level(spdlog::level::trace);

		initialized = true;
	}

	void Logger::initSectionLogger(const char* section, bool log_to_stdio) {
		if (spdlog::get(section)) { // already present
			SPDLOG_WARN("tried to reinit '{}' section logger!", section);
			return;
		}

		if (log_to_stdio) {
			auto section_logger = std::make_shared<spdlog::logger>(
					section, spdlog::sinks_init_list({
						_sink_console,
						_sink_file
#ifdef TRACY_ENABLE
						, _sink_tracy
#endif
					})
			);

			section_logger->set_level(spdlog::level::trace);

			spdlog::register_logger(section_logger);
		} else { // TODO: make better
			auto section_logger = std::make_shared<spdlog::logger>(
					section, spdlog::sinks_init_list({
						_sink_file
#ifdef TRACY_ENABLE
						, _sink_tracy
#endif
					})
			);

			section_logger->set_level(spdlog::level::trace);

			spdlog::register_logger(section_logger);
		}
	}

}

