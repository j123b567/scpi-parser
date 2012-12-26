/*-
 * Copyright (c) 2012-2013 Jan Breuer,
 *
 * All Rights Reserved
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
    return SCPI_RES_OK;
}

int SCPI_Error(scpi_t * context, int_fast16_t err) {
    (void) context;

    fprintf(stderr, "**ERROR: %d, \"%s\"\r\n", (int32_t) err, SCPI_ErrorTranslate(err));
    return 0;
}

scpi_result_t SCPI_Control(scpi_t * context, scpi_ctrl_name_t ctrl, scpi_reg_val_t val) {
    if (SCPI_CTRL_SRQ == ctrl) {
        fprintf(stderr, "**SRQ: 0x%X (%d)\r\n", val, val);
    } else {
        fprintf(stderr, "**CTRL %02x: 0x%X (%d)\r\n", ctrl, val, val);
    }
    return SCPI_RES_OK;
}

scpi_result_t SCPI_Test(scpi_t * context) {
    fprintf(stderr, "**Test\r\n");
    return SCPI_RES_OK;
}

scpi_result_t SCPI_Reset(scpi_t * context) {
    fprintf(stderr, "**Reset\r\n");
    return SCPI_RES_OK;
}

scpi_result_t SCPI_SystemCommTcpipControlQ(scpi_t * context) {
    return SCPI_RES_ERR;
}
/*
 * 
 */
int main(int argc, char** argv) {
    (void) argc;
    (void) argv;
    int result;

    SCPI_Init(&scpi_context);

    //printf("%.*s %s\r\n",  3, "asdadasdasdasdas", "b");
    printf("SCPI Interactive demo\r\n");
    char smbuffer[10];
    while (1) {
         fgets(smbuffer, 10, stdin);
         SCPI_Input(&scpi_context, smbuffer, strlen(smbuffer));
    }


    return (EXIT_SUCCESS);
}

