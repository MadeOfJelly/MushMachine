#pragma once

#ifdef TRACY_ENABLE

#include <spdlog/sinks/base_sink.h>
#include <tracy/Tracy.hpp>

namespace spdlog::sinks {

static uint32_t level_to_color(spdlog::level::level_enum level) {
	switch (level) {
		case level::level_enum::trace:		return 0xFFFFFF;
		case level::level_enum::debug:		return 0x1111FF;
		case level::level_enum::info:		return 0x11FF11;
		case level::level_enum::warn:		return 0xFFFF11;
		case level::level_enum::err:		return 0xFF1111;
		case level::level_enum::critical:	return 0xFF0000;
		default:							return 0xffffff;
	}
}

template<typename Mutex>
class tracy_sink : public ::spdlog::sinks::base_sink <Mutex> {
	public:
		tracy_sink(void) {
			this->set_pattern("[%n] [%l] %v");
		}
	protected:
		void sink_it_(const spdlog::details::log_msg& msg) override {

			// log_msg is a struct containing the log entry info like level, timestamp, thread id etc.
			// msg.raw contains pre formatted log

			// If needed (very likely but not mandatory), the sink formats the message before sending it to its final destination:
			spdlog::memory_buf_t formatted;
			base_sink<Mutex>::formatter_->format(msg, formatted);
			auto string = fmt::to_string(formatted);
			TracyMessageC(string.data(), string.size(), level_to_color(msg.level));
		}

		void flush_() override {
		}
};

} // spdlog::sinks

#include <spdlog/details/null_mutex.h>
#include <mutex>

namespace spdlog::sinks {
	using tracy_sink_mt = tracy_sink<std::mutex>;
	using tracy_sink_st = tracy_sink<spdlog::details::null_mutex>;
} // spdlog::sinks

#endif // TRACY_ENABLE

