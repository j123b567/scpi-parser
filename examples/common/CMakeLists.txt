cmake_minimum_required(VERSION 3.28)

project(scpi-common-example LANGUAGES C)

add_library(scpi-common-example STATIC scpi-def.c)

target_include_directories(scpi-common-example PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(scpi-common-example PRIVATE scpi-static)

install(TARGETS scpi-common-example
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(FILES scpi-def.h DESTINATION include)