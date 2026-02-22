cmake_minimum_required(VERSION 3.14)

project(coreTests LANGUAGES CXX)

include(../cmake/project-is-top-level.cmake)
include(../cmake/folders.cmake)

# ---- Dependencies ----

if(PROJECT_IS_TOP_LEVEL)
  find_package(core REQUIRED)
  enable_testing()
endif()

# ---- Tests ----

add_executable(core_test source/core_test.cpp)
target_link_libraries(core_test PRIVATE core::core)
target_compile_features(core_test PRIVATE cxx_std_20)

add_test(NAME core_test COMMAND core_test)

# ---- End-of-file commands ----

add_folders(Test)
