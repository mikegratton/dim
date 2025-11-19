cmake_minimum_required(VERSION 3.16)
project (dimlib VERSION 1.3.0.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Sanitizer builds
set(DIM_SANITIZER_BUILD "none" CACHE STRING "Build with Google sanitizers")
set_property(CACHE DIM_SANITIZER_BUILD PROPERTY STRINGS "none" "address" "thread")
if ("${DIM_SANITIZER_BUILD}" STREQUAL "none")
    message(STATUS "Sanitizers disabled")
elseif ("${DIM_SANITIZER_BUILD}" STREQUAL "address")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize=leak -fsanitize=undefined -ggdb -fno-omit-frame-pointer")
    message(STATUS "Building with asan and ubsan")
elseif ("${DIM_SANITIZER_BUILD}" STREQUAL "thread")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread -ggdb -fno-omit-frame-pointer")
    message(STATUS "Building with tsan")
else()
    message(STATUS "DIM_SANITIZER_BUILD \"${DIM_SANITIZER_BUILD}\" is not known. Sanitizers have been disabled")
endif()


add_subdirectory(src)

option(DIM_BUILD_TEST "Build dim unit test and examples" OFF)

if (DIM_BUILD_TEST)
    add_subdirectory(test)
    add_subdirectory(example)
endif()
