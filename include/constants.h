#pragma once

// Include C library headers
#include <cstdint>

namespace mmix {
	namespace constants {
		static const uint64_t text_segment = 0;
		static const uint64_t data_segment = 2305843009213693952;
		static const uint64_t pool_segment = 4611686018427387904;
		static const uint64_t stack_segment = 6917529027641081856;
	} // constants
} // mmix