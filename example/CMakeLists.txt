add_executable(quickstart quickstart.cpp)
target_link_libraries(quickstart PUBLIC dim)

add_executable(advanced advanced.cpp)
target_link_libraries(advanced PUBLIC dim)

set_target_properties(quickstart PROPERTIES CXX_STANDARD 20)
set_target_properties(advanced PROPERTIES CXX_STANDARD 20)