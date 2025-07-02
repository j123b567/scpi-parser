/*
 * Interactive SCPI Demo
 * 
 * This demo shows how to use the SCPI Parser library to create
 * an interactive command-line instrument interface.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <scpi/scpi.h>
#include "scpi-commands.h"

#define SCPI_INPUT_BUFFER_LENGTH 256
#define SCPI_ERROR_QUEUE_SIZE 17

/* SCPI buffer and error queue */
static char scpi_input_buffer[SCPI_INPUT_BUFFER_LENGTH];
static scpi_error_t scpi_error_queue_data[SCPI_ERROR_QUEUE_SIZE];

/* SCPI Interface Implementation */
static size_t SCPI_Write(scpi_t * context, const char * data, size_t len) {
    (void) context;
    return fwrite(data, 1, len, stdout);
}

static scpi_result_t SCPI_Flush(scpi_t * context) {
    (void) context;
    return fflush(stdout) == 0 ? SCPI_RES_OK : SCPI_RES_ERR;
}

static int SCPI_Error(scpi_t * context, int_fast16_t err) {
    (void) context;
    fprintf(stderr, "**ERROR: %d, \"%s\"\r\n", (int16_t) err, SCPI_ErrorTranslate(err));
    return 0;
}

static scpi_result_t SCPI_Control(scpi_t * context, scpi_ctrl_name_t ctrl, scpi_reg_val_t val) {
    (void) context;
    if (SCPI_CTRL_SRQ == ctrl) {
        fprintf(stderr, "**SRQ: 0x%X (%d)\r\n", val, val);
    } else {
        fprintf(stderr, "**CTRL %02x: 0x%X (%d)\r\n", ctrl, val, val);
    }
    return SCPI_RES_OK;
}

static scpi_result_t SCPI_Reset(scpi_t * context) {
    (void) context;
    printf("**Reset\r\n");
    return SCPI_RES_OK;
}

/* SCPI Interface structure */
static const scpi_interface_t scpi_interface = {
    .error = SCPI_Error,
    .write = SCPI_Write,
    .control = SCPI_Control,
    .flush = SCPI_Flush,
    .reset = SCPI_Reset,
};

/* SCPI Context */
static scpi_t scpi_context;

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
             NULL, /* No units */
             "DEMO", "SCPI_PARSER", "Interactive Demo", "v1.0",
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