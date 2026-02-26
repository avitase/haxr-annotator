#pragma once

#include <cstddef>
#include <cstdint>

#include "core/core_export.hpp"

extern "C"
{
// Takes ownership of input_data and frees it with free().
// Callers must allocate input_data with malloc-compatible allocation.
CORE_EXPORT [[nodiscard]] int32_t haxr_tag_hdf5_image(uint8_t* input_data,
                                                      size_t input_size);
CORE_EXPORT [[nodiscard]] const uint8_t* haxr_get_output_ptr();
CORE_EXPORT [[nodiscard]] size_t haxr_get_output_size();
}  // extern "C"
