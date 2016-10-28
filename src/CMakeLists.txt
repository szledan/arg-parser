file(GLOB SOURCES *.cpp)
add_library(arg-parse SHARED ${SOURCES})
file(COPY arg-parse.h DESTINATION ${INCLUDE_OUTPUT_DIR})
