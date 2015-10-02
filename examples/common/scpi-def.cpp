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
    if (!SCPI_ParamNumber(context, scpi_special_numbers_def, &param1, false)) {
        // do something, if parameter not present
    }

    // read second paraeter if present
    if (!SCPI_ParamNumber(context, scpi_special_numbers_def, &param2, false)) {
        // do something, if parameter not present
    }

    
    SCPI_NumberToStr(context, scpi_special_numbers_def, &param1, bf, 15);
    fprintf(stderr, "\tP1=%s\r\n", bf);

    
    SCPI_NumberToStr(context, scpi_special_numbers_def, &param2, bf, 15);
    fprintf(stderr, "\tP2=%s\r\n", bf);

    SCPI_ResultDouble(context, 0);
    
    return SCPI_RES_OK;
}


scpi_result_t DMM_MeasureVoltageAcQ(scpi_t * context) {
    scpi_number_t param1, param2;
    char bf[15];
    fprintf(stderr, "meas:volt:ac\r\n"); // debug command name

    // read first parameter if present
    if (!SCPI_ParamNumber(context, scpi_special_numbers_def, &param1, false)) {
        // do something, if parameter not present
    }

    // read second paraeter if present
    if (!SCPI_ParamNumber(context, scpi_special_numbers_def, &param2, false)) {
        // do something, if parameter not present
    }


    SCPI_NumberToStr(context, scpi_special_numbers_def, &param1, bf, 15);
    fprintf(stderr, "\tP1=%s\r\n", bf);


    SCPI_NumberToStr(context, scpi_special_numbers_def, &param2, bf, 15);
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

scpi_choice_def_t trigger_source[] = {
    {"BUS", 5},
    {"IMMediate", 6},
    {"EXTernal", 7},
    SCPI_CHOICE_LIST_END /* termination of option list */
};


scpi_result_t TEST_ChoiceQ(scpi_t * context) {

    int32_t param;
    const char * name;

    if (!SCPI_ParamChoice(context, trigger_source, &param, true)) {
        return SCPI_RES_ERR;
    }

    SCPI_ChoiceToName(trigger_source, param, &name);
    fprintf(stderr, "\tP1=%s (%ld)\r\n", name, (long int)param);

    SCPI_ResultInt32(context, param);

    return SCPI_RES_OK;
}

scpi_result_t TEST_Numbers(scpi_t * context) {

    fprintf(stderr, "RAW CMD %.*s\r\n", (int)context->param_list.cmd_raw.length, context->param_list.cmd_raw.data);

    return SCPI_RES_OK;
}

scpi_result_t TEST_Text(scpi_t * context) {
    char buffer[100];
    size_t copy_len;

    if(!SCPI_ParamCopyText(context, buffer, sizeof(buffer), &copy_len, false)) {
        buffer[0] = '\0';
    }

    fprintf(stderr, "TEXT: ***%s***\r\n", buffer);

    return SCPI_RES_OK;
}

scpi_result_t TEST_ArbQ(scpi_t * context) {
    const char * data;
    size_t len;

    SCPI_ParamArbitraryBlock(context, &data, &len, false);

    SCPI_ResultArbitraryBlock(context, data, len);

    return SCPI_RES_OK;
}

/**
 * Reimplement IEEE488.2 *TST?
 *
 * Result should be 0 if everything is ok
 * Result should be 1 if something goes wrong
 *
 * Return SCPI_RES_OK
 */
scpi_result_t My_CoreTstQ(scpi_t * context) {

    SCPI_ResultInt32(context, 0);

    return SCPI_RES_OK;
}

static const scpi_command_t scpi_commands[] = {
    /* {"pattern", callback} *
    
    /* IEEE Mandated Commands (SCPI std V1999.0 4.1.1) */
    {"*CLS", SCPI_CoreCls, 0},
    {"*ESE", SCPI_CoreEse, 0},
    {"*ESE?", SCPI_CoreEseQ, 0},
    {"*ESR?", SCPI_CoreEsrQ, 0},
    {"*IDN?", SCPI_CoreIdnQ, 0},
    {"*OPC", SCPI_CoreOpc, 0},
    {"*OPC?", SCPI_CoreOpcQ, 0},
    {"*RST", SCPI_CoreRst, 0},
    {"*SRE", SCPI_CoreSre, 0},
    {"*SRE?", SCPI_CoreSreQ, 0},
    {"*STB?", SCPI_CoreStbQ, 0},
    {"*TST?", My_CoreTstQ, 0},
    {"*WAI", SCPI_CoreWai, 0},

    /* Required SCPI commands (SCPI std V1999.0 4.2.1) */
    {"SYSTem:ERRor[:NEXT]?", SCPI_SystemErrorNextQ, 0},
    {"SYSTem:ERRor:COUNt?", SCPI_SystemErrorCountQ, 0},
    {"SYSTem:VERSion?", SCPI_SystemVersionQ, 0},

    //{"STATus:OPERation?", scpi_stub_callback, 0},
    //{"STATus:OPERation:EVENt?", scpi_stub_callback, 0},
    //{"STATus:OPERation:CONDition?", scpi_stub_callback, 0},
    //{"STATus:OPERation:ENABle", scpi_stub_callback, 0},
    //{"STATus:OPERation:ENABle?", scpi_stub_callback, 0},

    {"STATus:QUEStionable[:EVENt]?", SCPI_StatusQuestionableEventQ, 0},
    //{"STATus:QUEStionable:CONDition?", scpi_stub_callback, 0},
    {"STATus:QUEStionable:ENABle", SCPI_StatusQuestionableEnable, 0},
    {"STATus:QUEStionable:ENABle?", SCPI_StatusQuestionableEnableQ, 0},

    {"STATus:PRESet", SCPI_StatusPreset, 0},

    /* DMM */
    {"MEASure:VOLTage:DC?", DMM_MeasureVoltageDcQ, 0},
    {"CONFigure:VOLTage:DC", DMM_ConfigureVoltageDc, 0},
    {"MEASure:VOLTage:DC:RATio?", SCPI_StubQ, 0},
    {"MEASure:VOLTage:AC?", DMM_MeasureVoltageAcQ, 0},
    {"MEASure:CURRent:DC?", SCPI_StubQ, 0},
    {"MEASure:CURRent:AC?", SCPI_StubQ, 0},
    {"MEASure:RESistance?", SCPI_StubQ, 0},
    {"MEASure:FRESistance?", SCPI_StubQ, 0},
    {"MEASure:FREQuency?", SCPI_StubQ, 0},
    {"MEASure:PERiod?", SCPI_StubQ, 0},

    {"SYSTem:COMMunication:TCPIP:CONTROL?", SCPI_SystemCommTcpipControlQ, 0},

    {"TEST:BOOL", TEST_Bool, 0},
    {"TEST:CHOice?", TEST_ChoiceQ, 0},
    {"TEST#:NUMbers#", TEST_Numbers, 0},
    {"TEST:TEXT", TEST_Text, 0},
    {"TEST:ARBitrary?", TEST_ArbQ, 0},

    SCPI_CMD_LIST_END
};

static scpi_interface_t scpi_interface = {
    /* error */ SCPI_Error,
    /* write */ SCPI_Write,
    /* control */ SCPI_Control,
    /* flush */ SCPI_Flush,
    /* reset */ SCPI_Reset,
};

#define SCPI_INPUT_BUFFER_LENGTH 256
static char scpi_input_buffer[SCPI_INPUT_BUFFER_LENGTH];

static scpi_reg_val_t scpi_regs[SCPI_REG_COUNT];


scpi_t scpi_context = {
    /* cmdlist */ scpi_commands,
    /* buffer */ { /* length */ SCPI_INPUT_BUFFER_LENGTH, /* position */ 0, /* data */ scpi_input_buffer},
    /* param_list */ { /* cmd */ NULL, /* lex_state */ {NULL, NULL, 0}, /* cmd_raw */ {0, 0, NULL}},
    /* interface */ &scpi_interface,
    /* output_count */ 0,
    /* input_count */ 0,
    /* cmd_error */ FALSE,
    /* error_queue */ NULL,
    /* registers */ scpi_regs,
    /* units */ scpi_units_def,
    /* user_context */ NULL,
    /* parser_state */ { /* programHeader */ {SCPI_TOKEN_UNKNOWN, NULL, 0}, /* programData */ {SCPI_TOKEN_UNKNOWN, NULL, 0}, /* numberOfParameters */ 0, /* termination */ SCPI_MESSAGE_TERMINATION_NONE},
    /* idn */ {"MANUFACTURE", "INSTR2013", NULL, "01-02"},
};

