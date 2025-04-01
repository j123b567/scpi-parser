cmake_minimum_required(VERSION 3.28)

project(scpi-test-vxi11 LANGUAGES C)

add_executable(scpi-test-vxi11 main.c vxi11_xdr.c)

target_link_libraries(scpi-test-vxi11 PRIVATE scpi-static)

install(TARGETS scpi-test-vxi11
    RUNTIME DESTINATION bin
)