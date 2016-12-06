cmake_minimum_required(VERSION 2.8.10)
project(arg-parse C CXX)
set(PROJECT_VERSION 1)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

set(EXTRA_FLAGS "-Wall -pedantic -std=c++11 -g")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EXTRA_FLAGS}")

set(BINARY_OUTPUT_DIR ${PROJECT_BINARY_DIR}/bin)
set(INCLUDE_OUTPUT_DIR ${PROJECT_BINARY_DIR}/include)

file(MAKE_DIRECTORY ${BINARY_OUTPUT_DIR})
file(MAKE_DIRECTORY ${INCLUDE_OUTPUT_DIR})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BINARY_OUTPUT_DIR})

add_subdirectory(src)
#add_subdirectory(tests)
add_subdirectory(demos)
