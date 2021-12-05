cmake_minimum_required(VERSION 3.1)

project("persistent" CXX)

include (GenerateExportHeader)
include (CMakePackageConfigHelpers)
include (GNUInstallDirs)

if(POLICY CMP0077)
  cmake_policy(SET CMP0077 NEW)
endif()

option(PERSISTENT_CONFIG_INSTALL "On to make an installable standalone build, Off to build as part of a project. Default Off" OFF)
option(PERSISTENT_CXX_STANDARD "C++ standard to overwrite default cmake standard" ${CMAKE_CXX_STANDARD})

if(NOT TARGET persistent)

  set (PERSISTENT_VERSION_MAJOR 1)
  set (PERSISTENT_VERSION_MINOR 0)
  set (PERSISTENT_VERSION_PATCH 2)
  set (PERSISTENT_VERSION "${PERSISTENT_VERSION_MAJOR}.${PERSISTENT_VERSION_MINOR}.${PERSISTENT_VERSION_PATCH}")

  set (PERSISTENT_LIBRARIES persistent::persistent)

  if (PERSISTENT_CXX_STANDARD STREQUAL "OFF")
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
      "${PROJECT_BINARY_DIR}/persistent_version.h"
  )

  set(INCLUDE_FILES
    property.h
    named_property.h
    persistent.h
    basic_struct.h
    persistent_io.h
    persistent_ios.h
    persistent_json.h
    persistent_ptree.h
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
    set (INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_INCLUDEDIR}/persistent)
    set (LIBRARY_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/persistent)

    target_include_directories(persistent
                               PUBLIC
                               "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
                               "$<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}>"
    )

    write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/persistentConfigVersion.cmake
                                     VERSION ${PERSISTENT_VERSION}
                                     COMPATIBILITY SameMajorVersion)

    configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
                                  "${CMAKE_CURRENT_BINARY_DIR}/persistentConfig.cmake"
                                  INSTALL_DESTINATION ${LIBRARY_INSTALL_DIR}
                                  PATH_VARS CMAKE_INSTALL_INCLUDEDIR)

    install(TARGETS persistent
            EXPORT persistentTargets
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
            INCLUDES DESTINATION ${INCLUDE_INSTALL_DIR}
    )

    install(FILES ${INCLUDE_FILES}
            ${PROJECT_BINARY_DIR}/persistent_version.h
            DESTINATION ${INCLUDE_INSTALL_DIR}
    )

    install(EXPORT persistentTargets
            FILE persistentTargets.cmake
            NAMESPACE persistent::
            DESTINATION ${LIBRARY_INSTALL_DIR}
    )

    export(TARGETS persistent FILE persistentTargets.cmake)

    install(FILES
            "${CMAKE_CURRENT_BINARY_DIR}/persistentConfig.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/persistentConfigVersion.cmake"
            DESTINATION ${LIBRARY_INSTALL_DIR})

  endif()

endif()
