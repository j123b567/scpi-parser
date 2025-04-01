cmake_minimum_required(VERSION 3.28)

project(scpi-common-cxx-example LANGUAGES CXX)

add_library(scpi-common-cxx-example STATIC scpi-def.cpp)

target_include_directories(scpi-common-cxx-example PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(scpi-common-cxx-example PRIVATE scpi-static)

install(TARGETS scpi-common-cxx-example
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(FILES scpi-def.h DESTINATION include)