cmake_minimum_required(VERSION 3.14)

include(cmake/prelude.cmake)

project(
    core
    VERSION 0.1.0
    DESCRIPTION "haxr-annotator core"
    HOMEPAGE_URL "https://github.com/avitase/haxr-annotator"
    LANGUAGES CXX
)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

# ---- Declare library ----

add_library(
    core_core
    source/core.cpp
)
add_library(core::core ALIAS core_core)

include(GenerateExportHeader)
generate_export_header(
    core_core
    BASE_NAME core
    EXPORT_FILE_NAME export/core/core_export.hpp
    CUSTOM_CONTENT_FROM_VARIABLE pragma_suppress_c4251
)

if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(core_core PUBLIC CORE_STATIC_DEFINE)
endif()

set_target_properties(
    core_core PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN YES
    VERSION "${PROJECT_VERSION}"
    SOVERSION "${PROJECT_VERSION_MAJOR}"
    EXPORT_NAME core
    OUTPUT_NAME core
)

target_include_directories(
    core_core ${warning_guard}
    PUBLIC
    "\$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
)

target_include_directories(
    core_core SYSTEM
    PUBLIC
    "\$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/export>"
)

target_compile_features(core_core PUBLIC cxx_std_23)

find_package(ZLIB 1.3.2 EXACT REQUIRED)
find_package(HDF5 2.0.0 EXACT REQUIRED COMPONENTS C)
# find_package(HDF5 1.14.6 EXACT REQUIRED COMPONENTS C)

target_link_libraries(core_core PRIVATE hdf5::hdf5 ZLIB::ZLIB)

# ---- Optional WASM wrapper (Emscripten only) ----

option(CORE_BUILD_WASM "Build Emscripten JS/WASM wrapper" OFF)

if(CORE_BUILD_WASM)
  option(CORE_WASM_OUTPUT_DIR "Output directory for core.js/core.wasm" "")

  add_executable(core_wasm source/wasm_entry.cpp)
  target_link_libraries(core_wasm PRIVATE core::core)
  set_target_properties(core_wasm PROPERTIES OUTPUT_NAME core SUFFIX ".js")

  if(CORE_WASM_OUTPUT_DIR)
    set_target_properties(core_wasm PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY "${CORE_WASM_OUTPUT_DIR}"
    )
  endif()

  target_link_options(
      core_wasm PRIVATE
      "-sEXPORTED_FUNCTIONS=['_haxr_tag_hdf5_image','_haxr_get_output_ptr','_haxr_get_output_size','_haxr_clear_output','_malloc','_free']"
      "-sEXPORTED_RUNTIME_METHODS=['HEAPU8','HEAPU32']"
      "-sENVIRONMENT=web"
      "-sMODULARIZE=1"
      "-sEXPORT_NAME=Module"
      "-sALLOW_MEMORY_GROWTH=1"
  )

  if(CORE_BUILD_WASM)
    target_compile_options(core_wasm PRIVATE -Oz -g0 -flto)
    target_link_options(core_wasm PRIVATE -Oz -g0 -flto -sASSERTIONS=0)
  endif()
endif()

# ---- Developer mode ----

if(NOT core_DEVELOPER_MODE)
  return()
elseif(NOT PROJECT_IS_TOP_LEVEL)
  message(
      AUTHOR_WARNING
      "Developer mode is intended for developers of core"
  )
endif()

include(cmake/dev-mode.cmake)
