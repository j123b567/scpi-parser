/*-
 * BSD 2-Clause License
 *
 * Copyright (c) 2012-2018, Jan Breuer
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file   main.c
 * @date   Thu Nov 15 10:58:45 UTC 2012
 *
 * @brief  Interactive SCPI Demo
 *
 * This demo shows how to use the SCPI Parser library to create
 * an interactive command-line instrument interface.
 * 
 * This version includes enhanced features like help system and
 * command-line processing. For a simpler version, see 
 * examples/test-interactive/main.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <scpi/scpi.h>
#include "scpi-def.h"

#define SCPI_INPUT_BUFFER_LENGTH 256
#define SCPI_ERROR_QUEUE_SIZE 17

/* SCPI interface functions - these are referenced from the examples pattern but implemented per-demo */
size_t SCPI_Write(scpi_t * context, const char * data, size_t len) {
    (void) context;
    return fwrite(data, 1, len, stdout);
}

scpi_result_t SCPI_Flush(scpi_t * context) {
    (void) context;
    return SCPI_RES_OK;
}

int SCPI_Error(scpi_t * context, int_fast16_t err) {
    (void) context;
    fprintf(stderr, "**ERROR: %d, \"%s\"\r\n", (int16_t) err, SCPI_ErrorTranslate(err));
    return 0;
}

scpi_result_t SCPI_Control(scpi_t * context, scpi_ctrl_name_t ctrl, scpi_reg_val_t val) {
    (void) context;
    if (SCPI_CTRL_SRQ == ctrl) {
        fprintf(stderr, "**SRQ: 0x%X (%d)\r\n", val, val);
    } else {
        fprintf(stderr, "**CTRL %02x: 0x%X (%d)\r\n", ctrl, val, val);
    }
    return SCPI_RES_OK;
}

scpi_result_t SCPI_Reset(scpi_t * context) {
    (void) context;
    fprintf(stderr, "**Reset\r\n");
    return SCPI_RES_OK;
}

scpi_result_t SCPI_SystemCommTcpipControlQ(scpi_t * context) {
    (void) context;
    return SCPI_RES_ERR;
}

/**
 * Note: This demo includes enhanced interactive features.
 * For a simpler implementation, see examples/test-interactive/main.c
 * which demonstrates the core functionality with minimal code.
 */

/* Print help information */
static void print_help() {
    printf("\n=== SCPI Parser Interactive Demo ===\n\n");
    printf("This demo simulates a measurement instrument with SCPI interface.\n");
    printf("You can enter SCPI commands interactively.\n\n");
    
    printf("Example Commands:\n");
    printf("  *IDN?                           - Get instrument identification\n");
    printf("  *RST                            - Reset instrument\n");
    printf("  *TST?                           - Self test\n");
    printf("  SOUR:VOLT 5.0                  - Set voltage to 5.0V\n");
    printf("  SOUR:VOLT?                      - Query voltage setting\n");
    printf("  SOUR:CURR 0.1                  - Set current to 0.1A\n");
    printf("  SOUR:CURR?                      - Query current setting\n");
    printf("  SOUR:FREQ 1000                 - Set frequency to 1000Hz\n");
    printf("  SOUR:FREQ?                      - Query frequency setting\n");
    printf("  OUTP ON                         - Enable output\n");
    printf("  OUTP?                           - Query output state\n");
    printf("  MEAS:VOLT?                      - Measure voltage\n");
    printf("  MEAS:CURR?                      - Measure current\n");
    printf("  SYST:ERR?                       - Check for errors\n");
    printf("  AWG:VOLTage?                    - Query voltage setting (custom command)\n");
    printf("  AWG:VOLTage                     - Set voltage to 10.0V (custom command)\n");
    printf("  AWG:Count                       - Set AWG sample count (custom command)\n");
    printf("  AWG:Count?                      - Query AWG sample count (custom command)\n");
    printf("  AWG:Duration                    - Set AWG duration (custom command)\n");
    printf("  AWG:Duration?                    - Query AWG duration (custom command)\n");
    printf("  AWG:Enable                      - Enable AWG (custom command)\n");
    printf("  AWG:Enable?                      - Query AWG enable state (custom command)\n");
    printf("  AWG:WAVEform                    - Set AWG waveform (custom command)\n");
    printf("  AWG:WAVEform?                    - Query AWG waveform (custom command)\n");
    printf("  AWG:NAME                        - Set AWG name (custom command)\n");
    printf("  AWG:NAME?                        - Query AWG name (custom command)\n");
    printf(" AWG:ARB:LOAD                     - Load waveform data (custom command)\n");
    printf("  AWG:FREQ:INST?                  - Query AWG instantaneous frequency (custom command)\n");
    printf("  RUN                             - Start AWG (custom command)\n");
    printf("  help                            - Show this help\n");
    printf("  quit                            - Exit demo\n\n");
    printf("Note: Commands are case-insensitive and support SCPI abbreviations.\n");
    printf("For example: 'SOUR:VOLT?' can be written as 'sour:volt?' or 'so:v?'\n\n");
}

/* Process command line arguments */
static int process_arguments(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            print_help();
            return 1; // Exit after showing help
        }
        else if (strcmp(argv[i], "--version") == 0 || strcmp(argv[i], "-v") == 0) {
            printf("SCPI Parser Interactive Demo v1.0.0\n");
            printf("Built with SCPI Parser library\n");
            return 1; // Exit after showing version
        }
    }
    return 0; // Continue execution
}

/* Main function */
int main(int argc, char* argv[]) {
    char command_buffer[512];
    char *command;
    
    /* Process command line arguments */
    if (process_arguments(argc, argv)) {
        return 0;
    }
    
    /* Initialize SCPI context */
    SCPI_Init(&scpi_context,
             scpi_commands,
             &scpi_interface,
             scpi_units_def,
             SCPI_IDN1, SCPI_IDN2, SCPI_IDN3, SCPI_IDN4,
             scpi_input_buffer, SCPI_INPUT_BUFFER_LENGTH,
             scpi_error_queue_data, SCPI_ERROR_QUEUE_SIZE);
    
    printf("=== SCPI Parser Interactive Demo ===\n");
    printf("Type 'help' for available commands, 'quit' to exit.\n\n");
    
    /* Main command loop */
    while (1) {
        printf("SCPI> ");
        fflush(stdout);
        
        /* Read command from user */
        if (fgets(command_buffer, sizeof(command_buffer), stdin) == NULL) {
            break; /* EOF or error */
        }
        
        /* Remove newline character */
        command_buffer[strcspn(command_buffer, "\r\n")] = '\0';
        command = command_buffer;
        
        /* Skip leading whitespace */
        while (*command == ' ' || *command == '\t') {
            command++;
        }
        
        /* Skip empty commands */
        if (*command == '\0') {
            continue;
        }
        
        /* Handle special commands */
        if (strcmp(command, "quit") == 0 || strcmp(command, "exit") == 0) {
            printf("Goodbye!\n");
            break;
        }
        else if (strcmp(command, "help") == 0) {
            print_help();
            continue;
        }
        else if (strcmp(command, "clear") == 0) {
            #ifdef _WIN32
                system("cls");
            #else
                system("clear");
            #endif
            continue;
        }
        
        /* Process SCPI command */
        SCPI_Input(&scpi_context, command, strlen(command));
        SCPI_Input(&scpi_context, "\r\n", 2);
        
        printf("\n"); /* Add spacing after command output */
    }
    
    return 0;
} 