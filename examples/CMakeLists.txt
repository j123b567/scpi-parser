cmake_minimum_required(VERSION 3.28)

# Include all example subdirectories
add_subdirectory(common)
add_subdirectory(common-cxx)
add_subdirectory(test-interactive)
add_subdirectory(test-interactive-cxx)
add_subdirectory(test-parser)
# TCP examples are Unix-specific, skip on Windows
if(NOT WIN32)
    add_subdirectory(test-tcp)
    add_subdirectory(test-tcp-srq)
endif()
# add_subdirectory(test-vxi11)
