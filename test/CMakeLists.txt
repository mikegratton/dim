add_executable(dimTest 
    main.cpp
    literal_test.cpp
    si_test.cpp
    si_io_test.cpp
    )
target_link_libraries(dimTest PUBLIC dim)

set_target_properties(dimTest PROPERTIES CXX_STANDARD 17)
