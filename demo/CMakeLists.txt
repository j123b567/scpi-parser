cmake_minimum_required(VERSION 3.28)

project(scpi_parser_demo VERSION 1.0.0 LANGUAGES C CXX)

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Configure MSVC runtime for Windows
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
    set( CMAKE_MAP_IMPORTED_CONFIG_DEBUG RELEASE )
    set( CMAKE_MAP_IMPORTED_CONFIG_RELWITHDEBINFO RELEASE )
endif()

# Find the SCPI Parser package from Conan
find_package(scpi_parser REQUIRED)

# Display found package information
if(scpi_parser_FOUND)
    message(STATUS "Found SCPI Parser package: ${scpi_parser_VERSION}")
    get_target_property(SCPI_INCLUDE_DIRS scpi_parser::scpi_parser INTERFACE_INCLUDE_DIRECTORIES)
    message(STATUS "SCPI Include directories: ${SCPI_INCLUDE_DIRS}")
endif()

add_subdirectory(common)
add_subdirectory(demo-interactive)
add_subdirectory(demo-tcp)
add_subdirectory(demo-interactive-basic)

# Enable testing
enable_testing()

# Add tests for the demo applications
add_test(NAME demo_interactive_help 
         COMMAND demo-interactive-scpi --help
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/demo-interactive)
         
add_test(NAME demo_tcp_server_startup
         COMMAND demo-tcp-scpi --test-mode
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/demo-tcp) 