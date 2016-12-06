add_custom_target(demos)

include_directories(${PROJECT_BINARY_DIR}/include/)

add_subdirectory(parser-demo EXCLUDE_FROM_ALL)
