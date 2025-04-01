cmake_minimum_required(VERSION 3.28)

project(scpi-test-tcp-srq LANGUAGES C)

add_executable(scpi-test-tcp-srq main.c)

target_link_libraries(scpi-test-tcp-srq PRIVATE scpi-static)

install(TARGETS scpi-test-tcp-srq
    RUNTIME DESTINATION bin
)