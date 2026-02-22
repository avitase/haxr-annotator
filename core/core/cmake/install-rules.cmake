if(PROJECT_IS_TOP_LEVEL)
  set(
      CMAKE_INSTALL_INCLUDEDIR "include/core-${PROJECT_VERSION}"
      CACHE STRING ""
  )
  set_property(CACHE CMAKE_INSTALL_INCLUDEDIR PROPERTY TYPE PATH)
endif()

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

# find_package(<package>) call for consumers to find this project
set(package core)

install(
    DIRECTORY
    include/
    "${PROJECT_BINARY_DIR}/export/"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    COMPONENT core_Development
)

install(
    TARGETS core_core
    EXPORT coreTargets
    RUNTIME #
    COMPONENT core_Runtime
    LIBRARY #
    COMPONENT core_Runtime
    NAMELINK_COMPONENT core_Development
    ARCHIVE #
    COMPONENT core_Development
    INCLUDES #
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

write_basic_package_version_file(
    "${package}ConfigVersion.cmake"
    COMPATIBILITY SameMajorVersion
)

# Allow package maintainers to freely override the path for the configs
set(
    core_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/${package}"
    CACHE STRING "CMake package config location relative to the install prefix"
)
set_property(CACHE core_INSTALL_CMAKEDIR PROPERTY TYPE PATH)
mark_as_advanced(core_INSTALL_CMAKEDIR)

install(
    FILES cmake/install-config.cmake
    DESTINATION "${core_INSTALL_CMAKEDIR}"
    RENAME "${package}Config.cmake"
    COMPONENT core_Development
)

install(
    FILES "${PROJECT_BINARY_DIR}/${package}ConfigVersion.cmake"
    DESTINATION "${core_INSTALL_CMAKEDIR}"
    COMPONENT core_Development
)

install(
    EXPORT coreTargets
    NAMESPACE core::
    DESTINATION "${core_INSTALL_CMAKEDIR}"
    COMPONENT core_Development
)

if(PROJECT_IS_TOP_LEVEL)
  include(CPack)
endif()
