cmake_minimum_required(VERSION 3.16)
project (dimlib VERSION 2.1.0.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Sanitizer builds
option(DIM_SANITIZER_BUILD "Build with asan and ubsan" OFF)
if (DIM_SANITIZER_BUILD)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize=leak -fsanitize=undefined -ggdb -fno-omit-frame-pointer")
endif()

# Compatibility builds
set(DIM_CXX_VERSION "Default" CACHE STRING "Build with a specific C++ version")
set_property(CACHE DIM_CXX_VERSION PROPERTY STRINGS "Default" "11" "14" "17" "20")

add_subdirectory(src)

option(DIM_BUILD_TEST "Build dim unit test and examples" OFF)
if (DIM_BUILD_TEST)
    add_subdirectory(test)
    add_subdirectory(example)
endif()
