cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project("persistent-test" CXX)

set (TESTING_CXX_STANDARD ${PERSISTENT_CXX_STANDARD})
add_subdirectory(${CMAKE_SOURCE_DIR}/libs/testing testing)

# add the binary tree to the search path for include files
# so that we will find Version.h
include_directories(${PROJECT_BINARY_DIR} ${TESTING_INCLUDE_DIRS})

enable_testing()

set(tests
    ios_test
    json_test
    xml_test
    ini_test
    example_test
)

configure_file("test.json" ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)

foreach(test ${tests})
    add_executable(${test} WIN32 ${test}.cpp)
    target_link_libraries(${test} testing ${PERSISTENT_LIBRARIES})
    add_test(${test} ${CMAKE_CURRENT_BINARY_DIR}/${test})
    set_target_properties(${test} PROPERTIES
                          FOLDER tests
                          CXX_STANDARD ${PERSISTENT_CXX_STANDARD})
endforeach(test)
