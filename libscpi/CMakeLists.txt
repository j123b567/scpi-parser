cmake_minimum_required(VERSION 3.28)

# Set CMP0091 policy to NEW
cmake_policy(SET CMP0091 NEW)

# Set MSVC runtime to /MD
# if(MSVC)
#     set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
# endif()
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
endif()


set( CMAKE_MAP_IMPORTED_CONFIG_DEBUG RELEASE )
set( CMAKE_MAP_IMPORTED_CONFIG_RELWITHDEBINFO RELEASE )

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

    exports.def
)




# Create library based on BUILD_SHARED_LIBS (set by conan based on shared option)
if(BUILD_SHARED_LIBS)
    message(STATUS "Building shared library")
    # Create shared library
    add_library(scpi SHARED ${SOURCES})
    set_target_properties(scpi PROPERTIES
        OUTPUT_NAME scpi
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
        PUBLIC_HEADER "inc/scpi/scpi.h;inc/scpi/constants.h;inc/scpi/error.h;inc/scpi/ieee488.h;inc/scpi/minimal.h;inc/scpi/parser.h;inc/scpi/types.h;inc/scpi/units.h;inc/scpi/expression.h"
    )
    
    # Windows-specific configuration for shared libraries
    if(WIN32)

        # if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/exports.def)
        #     list(APPEND SOURCES exports.def)
        # endif()

        # Enable symbol exports for Windows DLL
        set_target_properties(scpi PROPERTIES
            WINDOWS_EXPORT_ALL_SYMBOLS ON
            # ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
            # LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
            # RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
        )
        # Ensure import library is generated
        set_target_properties(scpi PROPERTIES
            IMPORT_SUFFIX ".lib"
            IMPORT_PREFIX ""
        )
    else()
        target_link_libraries(scpi PRIVATE m)
        target_compile_options(scpi PRIVATE -fPIC)
    endif()
    
    target_include_directories(scpi PUBLIC inc)
    
else()
    message(STATUS "Building static library")
    # Create static library
    add_library(scpi STATIC ${SOURCES})
    set_target_properties(scpi PROPERTIES
        OUTPUT_NAME scpi
        PUBLIC_HEADER "inc/scpi/scpi.h;inc/scpi/constants.h;inc/scpi/error.h;inc/scpi/ieee488.h;inc/scpi/minimal.h;inc/scpi/parser.h;inc/scpi/types.h;inc/scpi/units.h;inc/scpi/expression.h"
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    )
    target_include_directories(scpi PUBLIC inc)
    if(NOT WIN32)
        target_link_libraries(scpi PRIVATE m)
    endif()
    
    # Create alias for backwards compatibility
    add_library(scpi-static ALIAS scpi)
    add_library(scpi-shared ALIAS scpi)
endif()


if( (WIN32 OR NOT BUILD_SHARED_LIBS) AND IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/test )



# Tests
enable_testing()

set(TEST_SOURCES
    test/test_fifo.c
    test/test_scpi_utils.c
    test/test_lexer_parser.c
    test/test_parser.c
)

# Add CUnit include directories to all test targets
find_package(conan_cunit REQUIRED)
target_include_directories(scpi PUBLIC ${conan_cunit_INCLUDE_DIRS})

foreach(test_source ${TEST_SOURCES})

    get_filename_component(test_name ${test_source} NAME_WE)
    add_executable(${test_name} ${test_source})
    target_link_libraries(${test_name} PRIVATE scpi conan_cunit::conan_cunit)  # 
    target_include_directories(${test_name} PUBLIC 
        ${conan_cunit_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}/inc
    )
    add_test(NAME ${test_name} COMMAND ${test_name})
    
    
endforeach()

# Installation
include(GNUInstallDirs)
install(TARGETS scpi
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/scpi
)



endif()
