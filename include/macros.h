#pragma once

// Include C++ STL headers
#include <vector>
#include <string>

namespace mmix {
	namespace macroprocessor {
		static const std::vector<std::string> macros {
			"INCLUDE",
			"MACRO",
			"USEMACRO",
			"IFDEF",
			"IFNDEF",
			"IF",
			"ELSE",
			"ENDIF",
			"DEFINE",
		};
	} // namespace macroprocessor
} // namespace mmix 