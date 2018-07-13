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
 * @brief  SCPI parser test
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scpi/scpi.h"
#include "../common/scpi-def.h"

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

/*
 *
 */
int main(int argc, char** argv) {
    (void) argc;
    (void) argv;
    int result;

    SCPI_Init(&scpi_context,
            scpi_commands,
            &scpi_interface,
            scpi_units_def,
            SCPI_IDN1, SCPI_IDN2, SCPI_IDN3, SCPI_IDN4,
            scpi_input_buffer, SCPI_INPUT_BUFFER_LENGTH,
            scpi_error_queue_data, SCPI_ERROR_QUEUE_SIZE);

#define TEST_SCPI_INPUT(cmd)    result = SCPI_Input(&scpi_context, cmd, strlen(cmd))

    TEST_SCPI_INPUT("*CLS\r\n");
    TEST_SCPI_INPUT("*RST\r\n");
    TEST_SCPI_INPUT("MEAS:volt:DC? 12,50;*OPC\r\n");
    TEST_SCPI_INPUT("*IDN?\r\n");
    TEST_SCPI_INPUT("SYST:VERS?");
    TEST_SCPI_INPUT("\r\n*ID");
    TEST_SCPI_INPUT("N?");
    TEST_SCPI_INPUT(""); /* emulate command timeout */

    TEST_SCPI_INPUT("*ESE\r\n"); /* cause error -109, missing parameter */
    TEST_SCPI_INPUT("*ESE #H20\r\n");

    TEST_SCPI_INPUT("*SRE #HFF\r\n");

    TEST_SCPI_INPUT("IDN?\r\n"); /* cause error -113, undefined header */

    TEST_SCPI_INPUT("SYST:ERR?\r\n");
    TEST_SCPI_INPUT("SYST:ERR?\r\n");
    TEST_SCPI_INPUT("*STB?\r\n");
    TEST_SCPI_INPUT("*ESR?\r\n");
    TEST_SCPI_INPUT("*STB?\r\n");

    TEST_SCPI_INPUT("meas:volt:dc? 0.01 V, Default\r\n");
    TEST_SCPI_INPUT("meas:volt:dc?\r\n");
    TEST_SCPI_INPUT("meas:volt:dc? def, 0.00001\r\n");
    TEST_SCPI_INPUT("meas:volt:dc? 0.00001\r\n");

    TEST_SCPI_INPUT("test:text 'a'\r\n");
    TEST_SCPI_INPUT("test:text 'a a'\r\n");
    TEST_SCPI_INPUT("test:text 'aa a'\r\n");
    TEST_SCPI_INPUT("test:text 'aaa aaaa'\r\n");
    TEST_SCPI_INPUT("TEST:CHANnellist (@9!2:3!4,5!6)\r\n");
    /* printf("%.*s %s\r\n",  3, "asdadasdasdasdas", "b");
     * interactive demo
     * char smbuffer[10];
     * while (1) {
     *     fgets(smbuffer, 10, stdin);
     *     SCPI_Input(&scpi_context, smbuffer, strlen(smbuffer));
     * }
     */


    return (EXIT_SUCCESS);
}

