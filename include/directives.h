#pragma once

// Include C++ STL headers
#include <vector>
#include <string>
namespace mmix {
	namespace preprocessor {
		static const std::vector<std::string> directives {
			"LOC",
			"GREG",
			"IS",
			"USE",
			"BLOCK",
			"ENDBLOCK"
		};
	}
} // mmix