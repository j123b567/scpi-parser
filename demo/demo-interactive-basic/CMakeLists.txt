cmake_minimum_required(VERSION 3.28)

project(demo-interactive-basic LANGUAGES C)

# Create the interactive demo executable
add_executable(${PROJECT_NAME} 
    main.c
    scpi-def.h
    scpi-def.c
)

# Link against the SCPI Parser package from Conan
target_link_libraries(${PROJECT_NAME} PRIVATE
    scpi_parser::scpi_parser
    )

# Set output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# Copy binaries post-build
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different ${CMAKE_BINARY_DIR}/bin $<TARGET_FILE_DIR:${PROJECT_NAME}> 
  COMMAND_EXPAND_LISTS
)

# Installation
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    COMPONENT demos
) 