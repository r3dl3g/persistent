cmake_minimum_required(VERSION 3.1)

project("persistent")

if(NOT TARGET persistent)

    set (persistent_include
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${CMAKE_CURRENT_BINARY_DIR}
        ${PROJECT_BINARY_DIR}
    )

    if (APPLE)
      set (persistent_include ${persistent_include} /usr/local/include)
    endif (APPLE)

    set (persistent_include ${persistent_include} PARENT_SCOPE)

    set (PERSISTENT_VERSION_MAJOR 1)
    set (PERSISTENT_VERSION_MINOR 0)
    set (PERSISTENT_VERSION_PATCH 1)
    set (PERSISTENT_VERSION "${PERSISTENT_VERSION_MAJOR}.${PERSISTENT_VERSION_MINOR}.${PERSISTENT_VERSION_PATCH}")

    set (INCLUDE_INSTALL_DIR include/guipp/persistent)

    include_directories(${persistent_include})

    # header file to pass version settings to the source code
    configure_file (
        "${PROJECT_SOURCE_DIR}/version.h.in"
        "${PROJECT_BINARY_DIR}/persistent_version.h"
    )

    file(GLOB INCLUDE_FILES "*.h" "*.inl")

    add_library(persistent INTERFACE)
    target_include_directories(persistent INTERFACE
        $<BUILD_INTERFACE:${INCLUDE_FILES}>
        $<INSTALL_INTERFACE:${INCLUDE_FILES}>
    )

    # target_sources(persistent INTERFACE "$<BUILD_INTERFACE:${INCLUDE_FILES}>")

    install(FILES ${INCLUDE_FILES}
                  ${PROJECT_BINARY_DIR}/persistent_version.h
            DESTINATION ${INCLUDE_INSTALL_DIR})

endif()
