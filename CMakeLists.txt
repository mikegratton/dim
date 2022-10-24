cmake_minimum_required(VERSION 3.0)
project (dimlib VERSION 1.0.0.0)

set(source 
    dim/io_stream.cpp
    dim/si/io.cpp
    dim/si/io_stream.cpp
    dim/si/quantity.lex.cpp
    dim/si/quantity.tab.cpp
    dim/si/parse_unit.cpp
)

if (BUILD_SHARED_LIBS)
    add_library(DimLib SHARED ${source} )
else()
    add_library(DimLib STATIC ${source} )
endif()

target_include_directories(DimLib
   INTERFACE
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
      $<INSTALL_INTERFACE:include>
   PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}
   )

set_target_properties(DimLib PROPERTIES CXX_STANDARD 17)

add_subdirectory(test)

######################################################
# Installation instructions
######################################################
install(TARGETS DimLib
    EXPORT DimLibTargets
    ARCHIVE 
        DESTINATION lib
        COMPONENT Development EXCLUDE_FROM_ALL
    LIBRARY DESTINATION lib
    PUBLIC_HEADER 
        DESTINATION include
        COMPONENT Development EXCLUDE_FROM_ALL
    )
 
install(EXPORT DimLibTargets 
    NAMESPACE met::
    DESTINATION lib/cmake
    COMPONENT Development EXCLUDE_FROM_ALL
    )
