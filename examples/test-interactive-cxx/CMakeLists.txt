cmake_minimum_required(VERSION 3.28)

project(scpi-test-interactive-cxx LANGUAGES CXX)

add_executable(scpi-test-interactive-cxx main.cpp)

target_link_libraries(scpi-test-interactive-cxx PRIVATE scpi-static)

install(TARGETS scpi-test-interactive-cxx
    RUNTIME DESTINATION bin
)