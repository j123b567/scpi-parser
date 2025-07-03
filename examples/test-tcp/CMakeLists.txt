cmake_minimum_required(VERSION 3.28)

project(scpi-test-tcp LANGUAGES C)

# Build options for different versions
option(USE_CROSS_PLATFORM_TCP "Use cross-platform TCP implementation" OFF)

if(USE_CROSS_PLATFORM_TCP)
    # Cross-platform version (single file for all platforms)
    add_executable(scpi-test-tcp main-cross.c)
    if(WIN32)
        target_link_libraries(scpi-test-tcp PRIVATE scpi scpi-common-example ws2_32)
    else()
        target_link_libraries(scpi-test-tcp PRIVATE scpi scpi-common-example)
    endif()
else()
    # Platform-specific versions
    if(WIN32)
        add_executable(scpi-test-tcp main-win32.c)
        target_link_libraries(scpi-test-tcp PRIVATE scpi scpi-common-example ws2_32)
    else()
        add_executable(scpi-test-tcp main.c)
        target_link_libraries(scpi-test-tcp PRIVATE scpi scpi-common-example)
    endif()
endif()

install(TARGETS scpi-test-tcp
    RUNTIME DESTINATION bin
)