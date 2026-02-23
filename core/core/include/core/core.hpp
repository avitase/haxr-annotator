#pragma once

#include <cstdint>

#include "core/core_export.hpp"

extern "C"
{
CORE_EXPORT [[nodiscard]] int32_t haxr_tag_hdf5(const char* input_path,
                                                const char* output_path);
}  // extern "C"
