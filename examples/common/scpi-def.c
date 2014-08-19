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
 * @file   scpi-def.c
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
#include "scpi-def.h"

scpi_result_t DMM_MeasureVoltageDcQ(scpi_t * context) {
    scpi_number_t param1, param2;
    char bf[15];
    fprintf(stderr, "meas:volt:dc\r\n"); // debug command name   

    // read first parameter if present
    if (!SCPI_ParamNumber(context, &param1, false)) {
        // do something, if parameter not present
    }

    // read second paraeter if present
    if (!SCPI_ParamNumber(context, &param2, false)) {
        // do something, if parameter not present
    }

    
    SCPI_NumberToStr(context, &param1, bf, 15);
    fprintf(stderr, "\tP1=%s\r\n", bf);

    
    SCPI_NumberToStr(context, &param2, bf, 15);
    fprintf(stderr, "\tP2=%s\r\n", bf);

    SCPI_ResultDouble(context, 0);
    
    return SCPI_RES_OK;
}


scpi_result_t DMM_MeasureVoltageAcQ(scpi_t * context) {
    scpi_number_t param1, param2;
    char bf[15];
    fprintf(stderr, "meas:volt:ac\r\n"); // debug command name   

    // read first parameter if present
    if (!SCPI_ParamNumber(context, &param1, false)) {
        // do something, if parameter not present
    }

    // read second paraeter if present
    if (!SCPI_ParamNumber(context, &param2, false)) {
        // do something, if parameter not present
    }

    
    SCPI_NumberToStr(context, &param1, bf, 15);
    fprintf(stderr, "\tP1=%s\r\n", bf);

    
    SCPI_NumberToStr(context, &param2, bf, 15);
    fprintf(stderr, "\tP2=%s\r\n", bf);

    SCPI_ResultDouble(context, 0);
    
    return SCPI_RES_OK;
}

scpi_result_t DMM_ConfigureVoltageDc(scpi_t * context) {
    double param1, param2;
    fprintf(stderr, "conf:volt:dc\r\n"); // debug command name   

    // read first parameter if present
    if (!SCPI_ParamDouble(context, &param1, true)) {
        return SCPI_RES_ERR;
    }

    // read second paraeter if present
    if (!SCPI_ParamDouble(context, &param2, false)) {
        // do something, if parameter not present
    }

    fprintf(stderr, "\tP1=%lf\r\n", param1);
    fprintf(stderr, "\tP2=%lf\r\n", param2);

    return SCPI_RES_OK;
}

scpi_result_t TEST_Bool(scpi_t * context) {
    scpi_bool_t param1;
    fprintf(stderr, "TEST:BOOL\r\n"); // debug command name   

    // read first parameter if present
    if (!SCPI_ParamBool(context, &param1, true)) {
        return SCPI_RES_ERR;
    }

    fprintf(stderr, "\tP1=%d\r\n", param1);

    return SCPI_RES_OK;
}

const char * trigger_source[] = {
    "BUS",
    "IMMediate",
    "EXTernal",
    NULL /* termination of option list */
};


scpi_result_t TEST_ChoiceQ(scpi_t * context) {

    int32_t param;
    
    if (!SCPI_ParamChoice(context, trigger_source, &param, true)) {
        return SCPI_RES_ERR;
    }
    
    fprintf(stderr, "\tP1=%s (%d)\r\n", trigger_source[param], param);
    
    SCPI_ResultInt(context, param);

    return SCPI_RES_OK;
}

scpi_result_t TEST_Numbers(scpi_t * context) {

    fprintf(stderr, "RAW CMD %.*s\r\n", (int)context->paramlist.cmd_raw.length, context->paramlist.cmd_raw.data);

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
    {.pattern = "SYSTem:ERRor[:NEXT]?", .callback = SCPI_SystemErrorNextQ,},
    {.pattern = "SYSTem:ERRor:COUNt?", .callback = SCPI_SystemErrorCountQ,},
    {.pattern = "SYSTem:VERSion?", .callback = SCPI_SystemVersionQ,},

    //{.pattern = "STATus:OPERation?", .callback = scpi_stub_callback,},
    //{.pattern = "STATus:OPERation:EVENt?", .callback = scpi_stub_callback,},
    //{.pattern = "STATus:OPERation:CONDition?", .callback = scpi_stub_callback,},
    //{.pattern = "STATus:OPERation:ENABle", .callback = scpi_stub_callback,},
    //{.pattern = "STATus:OPERation:ENABle?", .callback = scpi_stub_callback,},

    {.pattern = "STATus:QUEStionable[:EVENt]?", .callback = SCPI_StatusQuestionableEventQ,},
    //{.pattern = "STATus:QUEStionable:CONDition?", .callback = scpi_stub_callback,},
    {.pattern = "STATus:QUEStionable:ENABle", .callback = SCPI_StatusQuestionableEnable,},
    {.pattern = "STATus:QUEStionable:ENABle?", .callback = SCPI_StatusQuestionableEnableQ,},

    {.pattern = "STATus:PRESet", .callback = SCPI_StatusPreset,},

    /* DMM */
    {.pattern = "MEASure:VOLTage:DC?", .callback = DMM_MeasureVoltageDcQ,},
    {.pattern = "CONFigure:VOLTage:DC", .callback = DMM_ConfigureVoltageDc,},
    {.pattern = "MEASure:VOLTage:DC:RATio?", .callback = SCPI_StubQ,},
    {.pattern = "MEASure:VOLTage:AC?", .callback = DMM_MeasureVoltageAcQ,},
    {.pattern = "MEASure:CURRent:DC?", .callback = SCPI_StubQ,},
    {.pattern = "MEASure:CURRent:AC?", .callback = SCPI_StubQ,},
    {.pattern = "MEASure:RESistance?", .callback = SCPI_StubQ,},
    {.pattern = "MEASure:FRESistance?", .callback = SCPI_StubQ,},
    {.pattern = "MEASure:FREQuency?", .callback = SCPI_StubQ,},
    {.pattern = "MEASure:PERiod?", .callback = SCPI_StubQ,},
    
    {.pattern = "SYSTem:COMMunication:TCPIP:CONTROL?", .callback = SCPI_SystemCommTcpipControlQ,},

    {.pattern = "TEST:BOOL", .callback = TEST_Bool,},
    {.pattern = "TEST:CHOice?", .callback = TEST_ChoiceQ,},
    {.pattern = "TEST#:NUMbers#", .callback = TEST_Numbers,},

    SCPI_CMD_LIST_END
};

static scpi_interface_t scpi_interface = {
    .error = SCPI_Error,
    .write = SCPI_Write,
    .control = SCPI_Control,
    .flush = SCPI_Flush,
    .reset = SCPI_Reset,
    .test = SCPI_Test,
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
    .idn = {"MANUFACTURE", "INSTR2013", NULL, "01-02"},
};
