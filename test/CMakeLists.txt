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


if (DIM_FORCE_CXX_11)
    set_target_properties(dimTest PROPERTIES CXX_STANDARD 11)
else()
    set_target_properties(dimTest PROPERTIES CXX_STANDARD 20)
endif()

add_executable(dimCompileErrors
    main.cpp
    compile_errors.cpp
)

target_link_libraries(dimCompileErrors PUBLIC dim)

set_target_properties(dimCompileErrors PROPERTIES
    CXX_STANDARD 17
    EXCLUDE_FROM_ALL ON
)
