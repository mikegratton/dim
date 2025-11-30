add_executable(dimTest
    dynamic_test.cpp
    facet_test.cpp
    format_map_test.cpp
    format_test.cpp
    formatter_test.cpp
    iostream_test.cpp
    io_test.cpp    
    literal_test.cpp
    main.cpp
    parse_timing.cpp
    parser_test.cpp    
    si_io_test.cpp
    si_test.cpp
    test_utilities.cpp
)
target_link_libraries(dimTest PUBLIC dim)

target_compile_definitions(dimTest PUBLIC DOCTEST_CONFIG_SUPER_FAST_ASSERTS)

add_executable(dimCompileErrors
    main.cpp
    compile_errors.cpp
)

target_link_libraries(dimCompileErrors PUBLIC dim)

set_target_properties(dimCompileErrors PROPERTIES
    EXCLUDE_FROM_ALL ON
)
