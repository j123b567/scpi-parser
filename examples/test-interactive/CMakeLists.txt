cmake_minimum_required(VERSION 3.28)

project(scpi-test-interactive LANGUAGES C)

add_executable(scpi-test-interactive main.c)

target_link_libraries(scpi-test-interactive PRIVATE scpi-static scpi-common-example)

install(TARGETS scpi-test-interactive
    RUNTIME DESTINATION bin
)