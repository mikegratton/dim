
set(source
    dim/io.cpp
    dim/io_stream.cpp
    dim/format_map.cpp
    dim/si/si_io.cpp
    dim/si/si_io_stream.cpp
    dim/si/quantity.tab.cpp
    dim/si/definition.cpp
)

if(BUILD_SHARED_LIBS)
    add_library(dim SHARED ${source})
else()
    add_library(dim STATIC ${source})
endif()

target_include_directories(dim
    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set_target_properties(dim PROPERTIES CXX_STANDARD 11)

install(DIRECTORY dim TYPE INCLUDE
    FILES_MATCHING PATTERN *.hpp)
