set(SOURCES
    main.cpp
)

add_executable(parser-demo EXCLUDE_FROM_ALL ${SOURCES})
add_dependencies(demos parser-demo)
