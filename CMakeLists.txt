cmake_minimum_required(VERSION 3.19)

file(READ "derivation.nix" derivation_ver)
string(REGEX MATCH "version = \"([0-9\.]+)\"" _ ${derivation_ver})
set(project_version ${CMAKE_MATCH_1})

message("Building persistent version: ${project_version}")

project("persistent" LANGUAGES CXX VERSION ${project_version})

include (GenerateExportHeader)
include (CMakePackageConfigHelpers)
include (GNUInstallDirs)

if(POLICY CMP0077)
  cmake_policy(SET CMP0077 NEW)
endif()

if(NOT DEFINED CMAKE_CXX_VISIBILITY_PRESET AND
    NOT DEFINED CMAKE_VISIBILITY_INLINES_HIDDEN)
  set(CMAKE_CXX_VISIBILITY_PRESET hidden)
  set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)
endif()

if(NOT TARGET persistent)

  set (PERSISTENT_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
  set (PERSISTENT_VERSION_MINOR ${PROJECT_VERSION_MINOR})
  set (PERSISTENT_VERSION_PATCH ${PROJECT_VERSION_PATCH})
  set (PERSISTENT_VERSION "${PROJECT_VERSION}")

  option(PERSISTENT_CONFIG_INSTALL "On to make an installable standalone build, Off to build as part of a project. Default Off" OFF)
  set(PERSISTENT_CXX_STANDARD "${CMAKE_CXX_STANDARD}" CACHE STRING "C++ standard to overwrite default cmake standard")

  set (PERSISTENT_LIBRARIES persistent::persistent)

  if (PERSISTENT_CXX_STANDARD STREQUAL "")
    set (PERSISTENT_CXX_STANDARD 17)
  endif ()

  set (PERSISTENT_INCLUDE_DIRS
      ${CMAKE_CURRENT_SOURCE_DIR}/..
      ${CMAKE_CURRENT_BINARY_DIR}
      ${PROJECT_BINARY_DIR}
  )

  if (APPLE)
    set (PERSISTENT_INCLUDE_DIRS ${PERSISTENT_INCLUDE_DIRS} /usr/local/include)
  endif (APPLE)

  get_directory_property(hasParent PARENT_DIRECTORY)
  if (hasParent)
    set (PERSISTENT_INCLUDE_DIRS ${PERSISTENT_INCLUDE_DIRS} PARENT_SCOPE)
    set (PERSISTENT_CXX_STANDARD ${PERSISTENT_CXX_STANDARD} PARENT_SCOPE)
    set (PERSISTENT_LIBRARIES ${PERSISTENT_LIBRARIES} PARENT_SCOPE)
  endif()

  include_directories(${PERSISTENT_INCLUDE_DIRS})

  # header file to pass version settings to the source code
  configure_file (
      "${PROJECT_SOURCE_DIR}/version.h.in"
      "${PROJECT_BINARY_DIR}/persistent/persistent_version.h"
  )

  set(INCLUDE_FILES
    persistent.h
    persistent_ini.h
    persistent_io.h
    persistent_ios.h
    persistent_json.h
    persistent_ptree.h
    persistent_xml.h
  )

  add_library(persistent INTERFACE)
  add_library(persistent::persistent ALIAS persistent)

if (NOT ${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
  set_target_properties(persistent PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    FOLDER libraries
    VERSION ${PERSISTENT_VERSION}
    SOVERSION ${PERSISTENT_VERSION_MAJOR}
    CXX_STANDARD ${PERSISTENT_CXX_STANDARD}
  )
endif()

  target_include_directories(persistent INTERFACE
      "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
      "$<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}>"
  )

  # target_sources(persistent INTERFACE "$<BUILD_INTERFACE:${INCLUDE_FILES}>")

  if (PERSISTENT_CONFIG_INSTALL)
    set (PERSISTENT_INSTALL_INCLUDEDIR ${CMAKE_INSTALL_INCLUDEDIR}/persistent)
    set (PERSISTENT_INSTALL_LIBDIR ${CMAKE_INSTALL_LIBDIR}/cmake/persistent)

    target_include_directories(persistent
        INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${PERSISTENT_INSTALL_INCLUDEDIR}>
    )

    write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/persistentConfigVersion.cmake
        VERSION ${PERSISTENT_VERSION}
        COMPATIBILITY SameMajorVersion
    )

    configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
        "${CMAKE_CURRENT_BINARY_DIR}/persistentConfig.cmake"
        INSTALL_DESTINATION ${PERSISTENT_INSTALL_LIBDIR}
        PATH_VARS CMAKE_INSTALL_INCLUDEDIR
    )

    install(TARGETS persistent
        EXPORT persistentTargets
        INCLUDES DESTINATION ${PERSISTENT_INSTALL_INCLUDEDIR}
    )

    install(FILES ${INCLUDE_FILES}
        ${PROJECT_BINARY_DIR}/persistent/persistent_version.h
        DESTINATION ${PERSISTENT_INSTALL_INCLUDEDIR}
    )

    install(EXPORT persistentTargets
        FILE persistentTargets.cmake
        NAMESPACE persistent::
        DESTINATION ${PERSISTENT_INSTALL_LIBDIR}
    )

    export(TARGETS persistent FILE persistentTargets.cmake)

    install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/persistentConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/persistentConfigVersion.cmake"
        DESTINATION ${PERSISTENT_INSTALL_LIBDIR}
    )

  endif()

endif()
