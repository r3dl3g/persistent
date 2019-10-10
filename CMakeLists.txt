cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project("persistent" CXX)

# see: http://stackoverflow.com/questions/23684789/cmake-build-multiple-executables-in-one-project-with-static-library

include (GenerateExportHeader)

set(PERSISTENT_BUILD_STATIC_MODULE_LIB ON CACHE BOOL "On to build a static library for each submodule, Off for shared library. default On")

if(NOT TARGET persistent-obj)

    set (persistent_include
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${CMAKE_CURRENT_BINARY_DIR}
        ${PROJECT_BINARY_DIR}
    )

    if (APPLE)
      set (persistent_include ${persistent_include} /usr/local/include)
    endif (APPLE)

    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set (persistent_sys_libraries stdc++fs pthread)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set (persistent_sys_libraries stdc++fs pthread)
    endif()

    if (PERSISTENT_BUILD_STATIC_MODULE_LIB)
        set (persistent_libraries persistent-static)
        set (persistent_defines "-DPERSISTENT_BUILT_AS_STATIC_LIB")
    else()
        set (persistent_libraries persistent)
    endif()

    set (persistent_sys_libraries ${persistent_sys_libraries} PARENT_SCOPE)
    set (persistent_include ${persistent_include} PARENT_SCOPE)
    set (persistent_libraries ${persistent_libraries} PARENT_SCOPE)
    set (persistent_defines ${persistent_defines} PARENT_SCOPE)

    message(STATUS "persistent_defines: ${persistent_defines}")
    message(STATUS "persistent_libraries: ${persistent_libraries}")
    message(STATUS "persistent_sys_libraries: ${persistent_sys_libraries}")

    set (PERSISTENT_VERSION_MAJOR 1)
    set (PERSISTENT_VERSION_MINOR 0)
    set (PERSISTENT_VERSION_PATCH 0)
    set (PERSISTENT_VERSION "${PERSISTENT_VERSION_MAJOR}.${PERSISTENT_VERSION_MINOR}.${PERSISTENT_VERSION_PATCH}")

    include_directories(${persistent_include})

    # header file to pass version settings to the source code
    configure_file (
        "${PROJECT_SOURCE_DIR}/version.h.in"
        "${PROJECT_BINARY_DIR}/persistent_version.h"
    )

    file(GLOB SOURCE_FILES "*.h" "*.cpp" "*.inl")

    if (WIN32)
      add_definitions(-DWIN32_LEAN_AND_MEAN)
    endif (WIN32)

    add_definitions(-DEXPERIMENTAL -Dpersistent_obj_EXPORTS ${persistent_defines})

    add_library(persistent-obj OBJECT ${SOURCE_FILES})

    set_target_properties(persistent-obj PROPERTIES POSITION_INDEPENDENT_CODE ON)

    if(PERSISTENT_BUILD_STATIC_MODULE_LIB)
        add_library(persistent-static STATIC $<TARGET_OBJECTS:persistent-obj>)
    else()
        add_library(persistent SHARED $<TARGET_OBJECTS:persistent-obj>)

        target_link_libraries(persistent PRIVATE ${persistent_sys_libraries})

        set_target_properties(persistent PROPERTIES
                                     POSITION_INDEPENDENT_CODE ON
                                     VERSION ${PERSISTENT_VERSION}
                                     SOVERSION ${PERSISTENT_VERSION_MAJOR})
    endif()

    GENERATE_EXPORT_HEADER(persistent-obj
        BASE_NAME PERSISTENT
        EXPORT_MACRO_NAME PERSISTENT_EXPORT
        EXPORT_FILE_NAME persistent-export.h
        STATIC_DEFINE PERSISTENT_BUILT_AS_STATIC_LIB
    )

endif()
