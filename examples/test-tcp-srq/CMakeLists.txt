cmake_minimum_required(VERSION 3.28)

project(scpi-test-tcp-srq LANGUAGES C)

# This example uses Unix-specific socket features, skip on Windows for now
if(NOT WIN32)
    add_executable(scpi-test-tcp-srq main.c)

    target_include_directories(scpi-test-tcp-srq PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../../libscpi/inc
    )
    target_link_libraries(scpi-test-tcp-srq PRIVATE scpi scpi-common-example)
    install(TARGETS scpi-test-tcp-srq
        RUNTIME DESTINATION bin
    )
endif()