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

scpi_result_t DMM_MeasureVoltageDcQ(scpi_t * context) {
    scpi_number_t param1, param2;
    char bf[15];
    fprintf(stderr, "meas:volt:dc\r\n"); // debug command name   

    // read first parameter if present
    if (SCPI_ParamNumber(context, &param1, false)) {
        SCPI_NumberToStr(context, &param1, bf, 15);
        fprintf(stderr, "\tP1=%s\r\n", bf);
    }

    // read second paraeter if present
    if (SCPI_ParamNumber(context, &param2, false)) {
        SCPI_NumberToStr(context, &param2, bf, 15);
        fprintf(stderr, "\tP2=%s\r\n", bf);
    }

    SCPI_ResultDouble(context, 0);
    
    return SCPI_RES_OK;
}

static const scpi_command_t scpi_commands[] = {
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
    {.pattern = "SYSTem:ERRor:COUNt?", .callback = SCPI_SystemErrorCountQ,},
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

static size_t SCPI_Write(scpi_t * context, const char * data, size_t len) {
    (void) context;
    return fwrite(data, 1, len, stdout);
}

static int SCPI_Error(scpi_t * context, int_fast16_t err) {
    (void) context;

    fprintf(stderr, "**ERROR: %d, \"%s\"\r\n", (int32_t) err, SCPI_ErrorTranslate(err));
    return 0;
}

static scpi_result_t SCPI_Srq(scpi_t * context) {
    scpi_reg_val_t stb = SCPI_RegGet(context, SCPI_REG_STB);
    fprintf(stderr, "**SRQ: 0x%X (%d)\r\n", stb, stb);
    return SCPI_RES_OK;
}


static scpi_interface_t scpi_interface = {
    .write = SCPI_Write,
    .error = SCPI_Error,
    .reset = NULL,
    .test = NULL,
    .srq = SCPI_Srq,
};

#define SCPI_INPUT_BUFFER_LENGTH 256
static char scpi_input_buffer[SCPI_INPUT_BUFFER_LENGTH];

static scpi_reg_val_t scpi_regs[SCPI_REG_COUNT];


scpi_t scpi_context = {
    .cmdlist = scpi_commands,
    .buffer = {
        .length = SCPI_INPUT_BUFFER_LENGTH,
        .data = scpi_input_buffer,
    },
    .interface = &scpi_interface,
    .registers = scpi_regs,
    .units = scpi_units_def,
    .special_numbers = scpi_special_numbers_def,
};

/*
 * 
 */
int main(int argc, char** argv) {
    (void) argc;
    (void) argv;
    int result;

    SCPI_Init(&scpi_context);

#define TEST_SCPI_INPUT(cmd)    result = SCPI_Input(&scpi_context, cmd, strlen(cmd))

    TEST_SCPI_INPUT("*CLS\r\n");
    TEST_SCPI_INPUT("*RST\r\n");
    TEST_SCPI_INPUT("MEAS:volt:DC? 12,50;*OPC\r\n");
    TEST_SCPI_INPUT("*IDN?\r\n");
    TEST_SCPI_INPUT("SYST:VERS?");
    TEST_SCPI_INPUT("\r\n*ID");
    TEST_SCPI_INPUT("N?");
    TEST_SCPI_INPUT(""); // emulate command timeout

    TEST_SCPI_INPUT("*ESE\r\n"); // cause error -109, missing parameter
    TEST_SCPI_INPUT("*ESE 0x20\r\n");

    TEST_SCPI_INPUT("*SRE 0xFF\r\n");
    
    TEST_SCPI_INPUT("IDN?\r\n"); // cause error -113, undefined header

    TEST_SCPI_INPUT("SYST:ERR?\r\n");
    TEST_SCPI_INPUT("SYST:ERR?\r\n");
    TEST_SCPI_INPUT("*STB?\r\n");
    TEST_SCPI_INPUT("*ESR?\r\n");
    TEST_SCPI_INPUT("*STB?\r\n");

    TEST_SCPI_INPUT("meas:volt:dc? 0.01 V, Default\r\n");
    TEST_SCPI_INPUT("meas:volt:dc?\r\n");
    TEST_SCPI_INPUT("meas:volt:dc? def, 0.00001\r\n");
    TEST_SCPI_INPUT("meas:volt:dc? 0.00001\r\n");


    //printf("%.*s %s\r\n",  3, "asdadasdasdasdas", "b");
    // interactive demo
    //char smbuffer[10];
    //while (1) {
    //     fgets(smbuffer, 10, stdin);
    //     SCPI_Input(&scpi_context, smbuffer, strlen(smbuffer));
    //}


    return (EXIT_SUCCESS);
}

