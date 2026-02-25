#pragma once

#include <cstddef>
#include <cstdint>

#include "core/core_export.hpp"

extern "C"
{
CORE_EXPORT [[nodiscard]] int32_t haxr_tag_hdf5_image(const uint8_t* input_data,
                                                      size_t input_size);
CORE_EXPORT [[nodiscard]] uint8_t* haxr_get_output_ptr();
CORE_EXPORT [[nodiscard]] size_t haxr_get_output_size();
CORE_EXPORT void haxr_clear_output();
}  // extern "C"
