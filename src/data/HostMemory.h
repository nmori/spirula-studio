#pragma once

#include <cstdint>

namespace spirula {

// Physical memory free right now, or 0 when the platform will not say.
uint64_t available_ram_bytes();

}  // namespace spirula
