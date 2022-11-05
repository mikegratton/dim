cmake_minimum_required(VERSION 3.0)
project (dimlib VERSION 1.0.0.0)

add_subdirectory(src)
add_subdirectory(test)

add_subdirectory(example)

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
