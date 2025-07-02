cmake_minimum_required(VERSION 3.28)

project(scpi-parser VERSION 2.1.0 LANGUAGES C)

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
endif()
set( CMAKE_MAP_IMPORTED_CONFIG_DEBUG RELEASE )
set( CMAKE_MAP_IMPORTED_CONFIG_RELWITHDEBINFO RELEASE )

# Add subprojects
add_subdirectory(libscpi)


if( IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/examples )
    add_subdirectory(examples)
endif()



# Enable testing
enable_testing()