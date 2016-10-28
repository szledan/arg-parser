file(GLOB SOURCES *.cpp)

add_library(arg-parse SHARED ${SOURCES})

file(COPY arg-parse.h DESTINATION ${INCLUDE_OUTPUT_DIR})

set(COMMON_INCLUDE_DIRS
    .
)

include_directories(${CMAKE_BINARY_DIR}/include/)

add_executable(main EXCLUDE_FROM_ALL ${SOURCES})

target_link_libraries(main arg-parse)

