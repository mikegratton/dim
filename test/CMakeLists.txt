add_executable(dimTest
    main.cpp
    io_test.cpp
    literal_test.cpp
    si_test.cpp
    si_io_test.cpp
    parser_test.cpp
    format_map_test.cpp
    parse_timing.cpp
)
target_link_libraries(dimTest PUBLIC dim)

set_target_properties(dimTest PROPERTIES CXX_STANDARD 11)

add_executable(dimCompileErrors
    main.cpp
    compile_errors.cpp
)

target_link_libraries(dimCompileErrors PUBLIC dim)

set_target_properties(dimCompileErrors PROPERTIES
    CXX_STANDARD 17
    EXCLUDE_FROM_ALL ON
)
