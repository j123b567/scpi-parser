cmake_minimum_required(VERSION 3.28)

project(scpi-test-parser LANGUAGES C)

add_executable(scpi-test-parser main.c)

target_link_libraries(scpi-test-parser PRIVATE scpi-static scpi-common-example)

install(TARGETS scpi-test-parser
    RUNTIME DESTINATION bin
)