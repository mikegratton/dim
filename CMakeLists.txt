cmake_minimum_required(VERSION 3.0)
project (dimlib VERSION 1.2.0.0)

add_subdirectory(src)

option(DIM_BUILD_TEST "Build dim unit test and examples" OFF)

if (DIM_BUILD_TEST)
    add_subdirectory(test)
    add_subdirectory(example)
endif()

install(TARGETS dim
    EXPORT DimTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    )
 
install(EXPORT DimTargets 
    NAMESPACE dim::
    DESTINATION lib/cmake
    COMPONENT Development EXCLUDE_FROM_ALL
    )
