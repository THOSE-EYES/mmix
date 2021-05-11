#pragma once

// Include C++ STL headers
#include <cstdint>
#include <string>

namespace mmix {
	/**
	 * The strucure holds info about program blocks
	 */
	struct Block {
		std::string label;
		uint64_t 	origin;
		uint64_t 	start;
		uint64_t 	end;
	};
}