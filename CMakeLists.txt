cmake_minimum_required(VERSION 3.28)

project(scpi-parser VERSION 2.1.0 LANGUAGES C)


# Add subprojects
add_subdirectory(libscpi)

# Enable testing
enable_testing()