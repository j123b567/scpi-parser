cmake_minimum_required(VERSION 3.28)

project(scpi-test-tcp LANGUAGES C)

# Use Windows-compatible source file on Windows, Unix version otherwise
if(WIN32)
    add_executable(scpi-test-tcp main-win32.c)
    target_link_libraries(scpi-test-tcp PRIVATE scpi-static scpi-common-example ws2_32)
else()
    add_executable(scpi-test-tcp main.c)
    target_link_libraries(scpi-test-tcp PRIVATE scpi-static scpi-common-example)
endif()

install(TARGETS scpi-test-tcp
    RUNTIME DESTINATION bin
)