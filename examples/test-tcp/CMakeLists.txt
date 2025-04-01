cmake_minimum_required(VERSION 3.28)

project(scpi-test-tcp LANGUAGES C)

add_executable(scpi-test-tcp main.c)

target_link_libraries(scpi-test-tcp PRIVATE scpi-static)

install(TARGETS scpi-test-tcp
    RUNTIME DESTINATION bin
)