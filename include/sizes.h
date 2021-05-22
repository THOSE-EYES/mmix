#pragma once

// Include C++ STL headers
#include <map>
#include <string>
namespace mmix {
	namespace compiler {
		static const std::map<std::string, uint32_t> sizes {
			{"BYTE", 1},
			{"WYDE", 2},
			{"TETRA", 3},
			{"OCTA", 4},
		};
	} // compiler
} // mmix