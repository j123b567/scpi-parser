cmake_minimum_required(VERSION 3.28)

project(scpi-parser VERSION 2.1.0 LANGUAGES C)

set( CMAKE_MAP_IMPORTED_CONFIG_DEBUG RELEASE )
set( CMAKE_MAP_IMPORTED_CONFIG_RELWITHDEBINFO RELEASE )

# Add subprojects
add_subdirectory(libscpi)

# Enable testing
enable_testing()