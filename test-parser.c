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
#include "scpi/scpi_ieee488.h"
#include "scpi/scpi_error.h"
#include "scpi/scpi_constants.h"
#include "scpi/scpi_minimal.h"

int DMM_MeasureVoltageDcQ(scpi_context_t * context) {
    double param1, param2;
    fprintf(stderr, "meas:volt:dc "); // debug command name
    
    // read first parameter
    if (SCPI_ParamDouble(context, &param1, false)) {
        fprintf(stderr, "P1=%lf ", param1);
    }
    
    // read second paraeter
    if (SCPI_ParamDouble(context, &param2, false)) {
        fprintf(stderr, "P2=%lf ", param2);
    }

    fprintf(stderr, "\r\n");
    SCPI_ResultDouble(context, 0);
    return 0;
}

scpi_command_t scpi_commands[] = {
    /* IEEE Mandated Commands (SCPI std V1999.0 4.1.1) */
    { .pattern = "*CLS", .callback = SCPI_CoreCls,},
    { .pattern = "*ESE", .callback = SCPI_CoreEse,},
    { .pattern = "*ESE?", .callback = SCPI_CoreEseQ,},
    { .pattern = "*ESR?", .callback = SCPI_CoreEsrQ,},
    { .pattern = "*IDN?", .callback = SCPI_CoreIdnQ,},
    { .pattern = "*OPC", .callback = SCPI_CoreOpc,},
    { .pattern = "*OPC?", .callback = SCPI_CoreOpcQ,},
    { .pattern = "*RST", .callback = SCPI_CoreRst,},
    { .pattern = "*SRE", .callback = SCPI_CoreSre,},
    { .pattern = "*SRE?", .callback = SCPI_CoreSreQ,},
    { .pattern = "*STB?", .callback = SCPI_CoreStbQ,},
    { .pattern = "*TST?", .callback = SCPI_CoreTstQ,},
    { .pattern = "*WAI", .callback = SCPI_CoreWai,},

    /* Required SCPI commands (SCPI std V1999.0 4.2.1) */
    {.pattern = "SYSTem:ERRor?", .callback = SCPI_SystemErrorNextQ,},
    {.pattern = "SYSTem:ERRor:NEXT?", .callback = SCPI_SystemErrorNextQ,},
    {.pattern = "SYSTem:VERSion?", .callback = SCPI_SystemVersionQ,},

    //{.pattern = "STATus:OPERation?", .callback = scpi_stub_callback,},
    //{.pattern = "STATus:OPERation:EVENt?", .callback = scpi_stub_callback,},
    //{.pattern = "STATus:OPERation:CONDition?", .callback = scpi_stub_callback,},
    //{.pattern = "STATus:OPERation:ENABle", .callback = scpi_stub_callback,},
    //{.pattern = "STATus:OPERation:ENABle?", .callback = scpi_stub_callback,},

    {.pattern = "STATus:QUEStionable?", .callback = SCPI_StatusQuestionableEventQ,},
    {.pattern = "STATus:QUEStionable:EVENt?", .callback = SCPI_StatusQuestionableEventQ,},
    //{.pattern = "STATus:QUEStionable:CONDition?", .callback = scpi_stub_callback,},
    {.pattern = "STATus:QUEStionable:ENABle", .callback = SCPI_StatusQuestionableEnable,},
    {.pattern = "STATus:QUEStionable:ENABle?", .callback = SCPI_StatusQuestionableEnableQ,},

    {.pattern = "STATus:PRESet", .callback = SCPI_StatusPreset,},

    /* DMM */
    {.pattern = "MEASure:VOLTage:DC?", .callback = DMM_MeasureVoltageDcQ,},
    {.pattern = "MEASure:VOLTage:DC:RATio?", .callback = SCPI_StubQ,},
    {.pattern = "MEASure:VOLTage:AC?", .callback = SCPI_StubQ,},
    {.pattern = "MEASure:CURRent:DC?", .callback = SCPI_StubQ,},
    {.pattern = "MEASure:CURRent:AC?", .callback = SCPI_StubQ,},
    {.pattern = "MEASure:RESistance?", .callback = SCPI_StubQ,},
    {.pattern = "MEASure:FRESistance?", .callback = SCPI_StubQ,},
    {.pattern = "MEASure:FREQuency?", .callback = SCPI_StubQ,},
    {.pattern = "MEASure:PERiod?", .callback = SCPI_StubQ,},

    SCPI_CMD_LIST_END
};

size_t SCPI_Write(scpi_context_t * context, const char * data, size_t len) {
    (void) context;
    return fwrite(data, 1, len, stdout);
}

int SCPI_Error(scpi_context_t * context, int_fast16_t err) {
    (void) context;
    
    fprintf(stderr, "**ERROR: %d, \"%s\"\r\n", (int32_t) err, SCPI_ErrorTranslate(err));
    return 0;
}

scpi_interface_t scpi_interface = {
    .write = SCPI_Write,
    .error = SCPI_Error,
    .reset = NULL,
    .test = NULL,
};

#define SCPI_BUFFER_LENGTH 256
char buffer[SCPI_BUFFER_LENGTH];

scpi_buffer_t scpi_buffer = {
    .length = SCPI_BUFFER_LENGTH,
    .data = buffer,
    //    .data = (char[SCPI_BUFFER_LENGTH]){},
};

scpi_context_t scpi_context;

/*
 * 
 */
#include "scpi/scpi_utils.h"
int main(int argc, char** argv) {
    (void) argc;
    (void) argv;
    int result;
   
    //printf("%.*s %s\r\n",  3, "asdadasdasdasdas", "b");
    
    SCPI_Init(&scpi_context, scpi_commands, &scpi_buffer, &scpi_interface);

    // // interactive demo
    //  char smbuffer[10];
    //  while(1) {
    //          fgets(smbuffer, 10, stdin);
    //          SCPI_Input(&scpi_context, smbuffer, strlen(smbuffer));      
    //  }
       
    result = SCPI_Input(&scpi_context, "*CLS\r\n", 6);
    result = SCPI_Input(&scpi_context, "*RST\r\n", 6);
    result = SCPI_Input(&scpi_context, "MEAS:volt:DC? 12,50;*OPC\r\n", 26);
    result = SCPI_Input(&scpi_context, "*IDN?\r\n", 7);
    result = SCPI_Input(&scpi_context, "SYST:VERS?", 10);
    result = SCPI_Input(&scpi_context, "\r\n*ID", 5);
    result = SCPI_Input(&scpi_context, "N?", 2);
    result = SCPI_Input(&scpi_context, NULL, 0); // emulate command timeout

    result = SCPI_Input(&scpi_context, "*ESE\r\n", 6);
    result = SCPI_Input(&scpi_context, "*ESE 0x20\r\n", 11);

    result = SCPI_Input(&scpi_context, "IDN?\r\n", 6); // cause error -113, undefined header

    result = SCPI_Input(&scpi_context, "SYST:ERR?\r\n", 11);
    result = SCPI_Input(&scpi_context, "SYST:ERR?\r\n", 11);
    result = SCPI_Input(&scpi_context, "*STB?\r\n", 6);
    result = SCPI_Input(&scpi_context, "*ESR?\r\n", 6);
    result = SCPI_Input(&scpi_context, "*STB?\r\n", 6);
    return (EXIT_SUCCESS);
}

