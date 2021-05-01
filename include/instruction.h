#pragma once

// Include C++ STL headers
#include <string>
#include <vector>

namespace mmix {
	/**
	 * The main structure in the program. It is used
	 * to store the complete information about the
	 * instruction. It is filled in Parser class
	 */
	struct Instruction {
		std::string 				mnemonic;
		std::string					size;
		std::string					directive;
		std::string 				label;
		std::vector<std::string> 	parameters;
	};
} // mmix