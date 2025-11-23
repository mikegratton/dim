cmake_minimum_required(VERSION 3.16)
project (dimlib VERSION 1.3.0.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Sanitizer builds
option(DIM_SANITIZER_BUILD "Build with asan and ubsan" OFF)
if (DIM_SANITIZER_BUILD)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize=leak -fsanitize=undefined -ggdb -fno-omit-frame-pointer")
endif()


option(DIM_FORCE_CXX_11 "Force c++11 mode (for compatibility testing)" OFF)
mark_as_advanced(DIM_FORCE_CXX_11)

add_subdirectory(src)

option(DIM_BUILD_TEST "Build dim unit test and examples" OFF)

if (DIM_BUILD_TEST)
    add_subdirectory(test)
    add_subdirectory(example)
endif()
