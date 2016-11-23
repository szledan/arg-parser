file(GLOB SOURCES *.cpp)
add_library(arg-parse SHARED ${SOURCES})
file(COPY arg-parse.hpp DESTINATION ${INCLUDE_OUTPUT_DIR})
