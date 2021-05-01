#pragma once

//
#include <string>
#include <cstdint>

/**
 * 
 */
namespace mmix {
	/**
	 * 
	 */
	struct Block {
		std::string label;
		uint64_t 	origin;
		uint64_t 	start;
		uint64_t 	end;
	};
}