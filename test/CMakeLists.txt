add_executable(dimTest 
	main.cpp
	literal.cpp
	si.cpp
	si_io.cpp
	)
target_link_libraries(dimTest PUBLIC DimLib)

set_target_properties(dimTest PROPERTIES CXX_STANDARD 17)
