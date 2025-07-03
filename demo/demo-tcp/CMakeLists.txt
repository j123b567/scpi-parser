project(demo-tcp-scpi LANGUAGES C)

# Create the TCP demo executable  
add_executable(demo-tcp-scpi 
    main.c
    scpi-commands.c
)

# Link against the SCPI Parser package from Conan
target_link_libraries(demo-tcp-scpi PRIVATE scpi_parser::scpi_parser)

# Platform-specific socket libraries
if(WIN32)
    target_link_libraries(demo-tcp-scpi PRIVATE ws2_32)
endif()

# Set output directory
set_target_properties(demo-tcp-scpi PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)



add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different ${CMAKE_BINARY_DIR}/bin $<TARGET_FILE_DIR:${PROJECT_NAME}> 
  COMMAND_EXPAND_LISTS
)

# Installation
install(TARGETS demo-tcp-scpi
    RUNTIME DESTINATION bin
    COMPONENT demos
) 