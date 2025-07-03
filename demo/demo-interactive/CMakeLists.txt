cmake_minimum_required(VERSION 3.28)

project(demo-interactive-scpi LANGUAGES C)

# Create the interactive demo executable
add_executable(demo-interactive-scpi 
    main.c
    ../common/scpi-def.h
)

# Link against the SCPI Parser package from Conan
target_link_libraries(demo-interactive-scpi PRIVATE
    scpi_parser::scpi_parser
    scpi-common-example)

# Set output directory
set_target_properties(demo-interactive-scpi PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# Copy binaries post-build
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different ${CMAKE_BINARY_DIR}/bin $<TARGET_FILE_DIR:${PROJECT_NAME}> 
  COMMAND_EXPAND_LISTS
)

# Installation
install(TARGETS demo-interactive-scpi
    RUNTIME DESTINATION bin
    COMPONENT demos
) 