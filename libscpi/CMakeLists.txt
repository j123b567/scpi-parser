cmake_minimum_required(VERSION 3.28)

# Set CMP0091 policy to NEW
cmake_policy(SET CMP0091 NEW)

project(scpi_parser VERSION 2.1.0 LANGUAGES C)

# Compiler options
# if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
#    add_compile_options(-Wextra -Wmissing-prototypes -Wimplicit)

# Source files
set(SOURCES
    src/error.c
    src/fifo.c
    src/ieee488.c
    src/minimal.c
    src/parser.c
    src/units.c
    src/utils.c
    src/lexer.c
    src/expression.c
)

# Create static library
add_library(scpi-static STATIC ${SOURCES})
set_target_properties(scpi-static PROPERTIES
    OUTPUT_NAME scpi
    PUBLIC_HEADER "inc/scpi/scpi.h;inc/scpi/constants.h;inc/scpi/error.h;inc/scpi/ieee488.h;inc/scpi/minimal.h;inc/scpi/parser.h;inc/scpi/types.h;inc/scpi/units.h;inc/scpi/expression.h"
)
target_include_directories(scpi-static PUBLIC inc)
if(NOT WIN32)
    target_link_libraries(scpi-static PRIVATE m)
endif()

# Create shared library
add_library(scpi-shared SHARED ${SOURCES})
set_target_properties(scpi-shared PROPERTIES
    OUTPUT_NAME scpi
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)
target_include_directories(scpi-shared PUBLIC inc)
if(NOT WIN32)
    target_link_libraries(scpi-shared PRIVATE m)
endif()
target_compile_options(scpi-shared PRIVATE -fPIC)

find_package(CUnit REQUIRED)

# Tests
enable_testing()

set(TEST_SOURCES
    test/test_fifo.c
    test/test_scpi_utils.c
    test/test_lexer_parser.c
    test/test_parser.c
)

target_include_directories(scpi-static PUBLIC CUnit_INCLUDE_DIRS)

foreach(test_source ${TEST_SOURCES})
    get_filename_component(test_name ${test_source} NAME_WE)
    add_executable(${test_name} ${test_source})
    target_link_libraries(${test_name} PRIVATE scpi-static CUnit::cunit)
    target_include_directories(${test_name} PUBLIC CUnit_INCLUDE_DIRS)
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()

# Installation
include(GNUInstallDirs)
install(TARGETS scpi-static scpi-shared
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/scpi
)