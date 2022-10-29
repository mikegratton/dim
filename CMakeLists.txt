cmake_minimum_required(VERSION 3.0)
project (dimlib VERSION 1.0.0.0)

add_subdirectory(src)
add_subdirectory(test)

######################################################
# Installation instructions
######################################################
install(TARGETS dim
    EXPORT DimTargets
    ARCHIVE 
        DESTINATION lib
        COMPONENT Development EXCLUDE_FROM_ALL
    LIBRARY DESTINATION lib
    PUBLIC_HEADER 
        DESTINATION include
        COMPONENT Development EXCLUDE_FROM_ALL
    )
 
install(EXPORT DimTargets 
    NAMESPACE met::
    DESTINATION lib/cmake
    COMPONENT Development EXCLUDE_FROM_ALL
    )
