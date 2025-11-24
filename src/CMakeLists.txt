
set(source
    dim/io.cpp
    dim/io_stream.cpp
    dim/si/si_io.cpp
    dim/si/si_quantity_facet.cpp
    dim/si/quantity.tab.cpp
    dim/si/definition.cpp
)

add_library(dim ${source})

target_include_directories(dim
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

if (DIM_FORCE_CXX_11)
    set_target_properties(dim PROPERTIES CXX_STANDARD 11)
else()
    set_target_properties(dim PROPERTIES CXX_STANDARD 20)
endif()

# Warnings
target_compile_options(dim PRIVATE -Wall -Wextra)

# Static analysis
set(analysis_source ${source})
list(REMOVE_ITEM analysis_source "dim/si/quantity.tab.cc")
add_custom_target(dimAnalysis
    COMMAND 
        clang-tidy -p ${CMAKE_BINARY_DIR} ${analysis_source}
    VERBATIM
    WORKING_DIRECTORY 
        ${CMAKE_CURRENT_LIST_DIR}
)


#############################################################
# Installation
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(DIM_INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_INCLUDEDIR}" CACHE PATH "Path where headers will be installed")
set(DIM_LIB_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR} CACHE PATH "Path for libraries and cmake files")
set(DIM_CONFIG_INSTALL_DIR ${DIM_LIB_INSTALL_DIR}/cmake/dim)
mark_as_advanced(DIM_INCLUDE_INSTALL_DIR DIM_LIB_INSTALL_DIR)
set(PACKAGE_VERSION ${PROJECT_VERSION})

# Public headers
install(
    DIRECTORY
        dim
    DESTINATION
        ${DIM_INCLUDE_INSTALL_DIR}
    FILES_MATCHING PATTERN
        "*.hpp"    
)

# Targets
install(
    TARGETS
        dim
    EXPORT 
        DimTargets    
    INCLUDES DESTINATION 
        ${CMAKE_INSTALL_INCLUDEDIR}
)

# CMake config/version/target files
configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/cmake/DimConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/DimConfig.cmake
    INSTALL_DESTINATION 
        ${DIM_CONFIG_INSTALL_DIR}
    PATH_VARS 
        DIM_INCLUDE_INSTALL_DIR
        DIM_LIB_INSTALL_DIR
)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/DimConfigVersion.cmake
    VERSION 
        ${PACKAGE_VERSION}
    COMPATIBILITY 
        SameMajorVersion
)
install(
    FILES 
        ${CMAKE_CURRENT_BINARY_DIR}/DimConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/DimConfigVersion.cmake        
    DESTINATION 
        ${DIM_CONFIG_INSTALL_DIR}
)
install(
    EXPORT
        DimTargets
    NAMESPACE
        dim::
    DESTINATION
        ${DIM_CONFIG_INSTALL_DIR}
)
