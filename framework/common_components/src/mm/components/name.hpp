#pragma once

#include <string>

namespace MM::Components {
	struct Name {
		static const size_t max_str_len {64u};
		std::string str;
	};
}

