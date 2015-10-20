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
#include "scpi-def.h"

#if USE_64K_PROGMEM_FOR_CMD_LIST
#include <avr/pgmspace.h>
#endif

scpi_result_t DMM_MeasureVoltageDcQ(scpi_t * context) {
    scpi_number_t param1, param2;
    char bf[15];
    SerialPrintf("meas:volt:dc"); // debug command name

    // read first parameter if present
    if (!SCPI_ParamNumber(context, scpi_special_numbers_def, &param1, false)) {
        // do something, if parameter not present
    }

    // read second paraeter if present
    if (!SCPI_ParamNumber(context, scpi_special_numbers_def, &param2, false)) {
        // do something, if parameter not present
    }


    SCPI_NumberToStr(context, scpi_special_numbers_def, &param1, bf, 15);
    SerialPrintf("\tP1=%s", bf);


    SCPI_NumberToStr(context, scpi_special_numbers_def, &param2, bf, 15);
    SerialPrintf("\tP2=%s", bf);

    SCPI_ResultDouble(context, 0);

    return SCPI_RES_OK;
}

scpi_result_t DMM_MeasureVoltageAcQ(scpi_t * context) {
    scpi_number_t param1, param2;
    char bf[15];
    SerialPrintf("meas:volt:ac"); // debug command name

    // read first parameter if present
    if (!SCPI_ParamNumber(context, scpi_special_numbers_def, &param1, false)) {
        // do something, if parameter not present
    }

    // read second paraeter if present
    if (!SCPI_ParamNumber(context, scpi_special_numbers_def, &param2, false)) {
        // do something, if parameter not present
    }


    SCPI_NumberToStr(context, scpi_special_numbers_def, &param1, bf, 15);
    SerialPrintf("\tP1=%s", bf);


    SCPI_NumberToStr(context, scpi_special_numbers_def, &param2, bf, 15);
    SerialPrintf("\tP2=%s", bf);

    SCPI_ResultDouble(context, 0);

    return SCPI_RES_OK;
}

scpi_result_t DMM_ConfigureVoltageDc(scpi_t * context) {
    double param1, param2;
    SerialPrintf("conf:volt:dc"); // debug command name

    // read first parameter if present
    if (!SCPI_ParamDouble(context, &param1, true)) {
        return SCPI_RES_ERR;
    }

    // read second paraeter if present
    if (!SCPI_ParamDouble(context, &param2, false)) {
        // do something, if parameter not present
    }

    SerialPrintf("\tP1=%lf", param1);
    SerialPrintf("\tP2=%lf", param2);

    return SCPI_RES_OK;
}

scpi_result_t TEST_Bool(scpi_t * context) {
    scpi_bool_t param1;
    SerialPrintf("TEST:BOOL"); // debug command name

    // read first parameter if present
    if (!SCPI_ParamBool(context, &param1, true)) {
        return SCPI_RES_ERR;
    }

    SerialPrintf("\tP1=%d", param1);

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
    SerialPrintf("\tP1=%s (%ld)", name, (long int) param);

    SCPI_ResultInt32(context, param);

    return SCPI_RES_OK;
}

scpi_result_t TEST_Numbers(scpi_t * context) {

    SerialPrintf("RAW CMD %.*s", (int) context->param_list.cmd_raw.length, context->param_list.cmd_raw.data);

    return SCPI_RES_OK;
}

scpi_result_t TEST_Text(scpi_t * context) {
    char buffer[100];
    size_t copy_len;

    if (!SCPI_ParamCopyText(context, buffer, sizeof (buffer), &copy_len, false)) {
        buffer[0] = '\0';
    }

    SerialPrintf("TEXT: ***%s***", buffer);

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

scpi_result_t SCPI_MeasureVoltageDCRatio(scpi_t * context) {
    SCPI_ResultInt32(context, 0);
    return SCPI_RES_OK;
}

scpi_result_t SCPI_MeasureCurrentDC(scpi_t * context) {
    SCPI_ResultInt32(context, 0);
    return SCPI_RES_OK;
}

scpi_result_t SCPI_MeasureCurrentAC(scpi_t * context) {
    SCPI_ResultInt32(context, 0);
    return SCPI_RES_OK;
}

scpi_result_t SCPI_MeasureResistance(scpi_t * context) {
    SCPI_ResultInt32(context, 0);
    return SCPI_RES_OK;
}

scpi_result_t SCPI_MeasureFResistance(scpi_t * context) {
    SCPI_ResultInt32(context, 0);
    return SCPI_RES_OK;
}

scpi_result_t SCPI_MeasureFrequency(scpi_t * context) {
    SCPI_ResultInt32(context, 0);
    return SCPI_RES_OK;
}

scpi_result_t SCPI_MeasurePeriod(scpi_t * context) {
    SCPI_ResultInt32(context, 0);
    return SCPI_RES_OK;
}

#define SCPI_COMMANDS \
    SCPI_COMMAND("*CLS", SCPI_CoreCls) \
    SCPI_COMMAND("*ESE", SCPI_CoreEse) \
    SCPI_COMMAND("*ESE?", SCPI_CoreEseQ) \
    SCPI_COMMAND("*ESR?", SCPI_CoreEsrQ) \
    SCPI_COMMAND("*IDN?", SCPI_CoreIdnQ) \
    SCPI_COMMAND("*OPC", SCPI_CoreOpc) \
    SCPI_COMMAND("*OPC?", SCPI_CoreOpcQ) \
    SCPI_COMMAND("*RST", SCPI_CoreRst) \
    SCPI_COMMAND("*SRE", SCPI_CoreSre) \
    SCPI_COMMAND("*SRE?", SCPI_CoreSreQ) \
    SCPI_COMMAND("*STB?", SCPI_CoreStbQ) \
    SCPI_COMMAND("*TST?", My_CoreTstQ) \
    SCPI_COMMAND("*WAI", SCPI_CoreWai) \
    SCPI_COMMAND("SYSTem:ERRor[:NEXT]?", SCPI_SystemErrorNextQ) \
    SCPI_COMMAND("SYSTem:ERRor:COUNt?", SCPI_SystemErrorCountQ) \
    SCPI_COMMAND("SYSTem:VERSion?", SCPI_SystemVersionQ) \
    SCPI_COMMAND("STATus:QUEStionable[:EVENt]?", SCPI_StatusQuestionableEventQ) \
    SCPI_COMMAND("STATus:QUEStionable:ENABle", SCPI_StatusQuestionableEnable) \
    SCPI_COMMAND("STATus:QUEStionable:ENABle?", SCPI_StatusQuestionableEnableQ) \
    SCPI_COMMAND("STATus:PRESet", SCPI_StatusPreset) \
    SCPI_COMMAND("MEASure:VOLTage:DC?", DMM_MeasureVoltageDcQ) \
    SCPI_COMMAND("CONFigure:VOLTage:DC", DMM_ConfigureVoltageDc) \
    SCPI_COMMAND("MEASure:VOLTage:DC:RATio?", SCPI_MeasureVoltageDCRatio) \
    SCPI_COMMAND("MEASure:VOLTage:AC?", DMM_MeasureVoltageAcQ) \
    SCPI_COMMAND("MEASure:CURRent:DC?", SCPI_MeasureCurrentDC) \
    SCPI_COMMAND("MEASure:CURRent:AC?", SCPI_MeasureCurrentAC) \
    SCPI_COMMAND("MEASure:RESistance?", SCPI_MeasureResistance) \
    SCPI_COMMAND("MEASure:FRESistance?", SCPI_MeasureFResistance) \
    SCPI_COMMAND("MEASure:FREQuency?", SCPI_MeasureFrequency) \
    SCPI_COMMAND("MEASure:PERiod?", SCPI_MeasurePeriod) \
    SCPI_COMMAND("SYSTem:COMMunication:TCPIP:CONTROL?", SCPI_SystemCommTcpipControlQ) \
    SCPI_COMMAND("TEST:BOOL", TEST_Bool) \
    SCPI_COMMAND("TEST:CHOice?", TEST_ChoiceQ) \
    SCPI_COMMAND("TEST#:NUMbers#", TEST_Numbers) \
    SCPI_COMMAND("TEST:TEXT", TEST_Text) \
    SCPI_COMMAND("TEST:ARBitrary?", TEST_ArbQ) \

#if USE_64K_PROGMEM_FOR_CMD_LIST

#define SCPI_COMMAND(P, C) static const char C ## _pattern[] PROGMEM = P;
SCPI_COMMANDS
#undef SCPI_COMMAND
#define SCPI_COMMAND(P, C) {C ## _pattern, C},
static const scpi_command_t scpi_commands[] PROGMEM = {
    SCPI_COMMANDS
    SCPI_CMD_LIST_END
};

#elif USE_FULL_PROGMEM_FOR_CMD_LIST

#define SCPI_COMMAND(P, C) P
static const char scpi_command_patterns[] PROGMEM = SCPI_COMMANDS;
#undef SCPI_COMMAND
#define SCPI_COMMAND(P, C) {(const char *)(sizeof(P) - 1), C},
static const scpi_command_t scpi_commands[] PROGMEM = {
    SCPI_COMMANDS
    SCPI_CMD_LIST_END
};

#else

#define SCPI_COMMAND(P, C) {P, C},
static const scpi_command_t scpi_commands[] = {
    SCPI_COMMANDS
    SCPI_CMD_LIST_END
};

#endif

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


static scpi_t scpi_context = {
#if USE_FULL_PROGMEM_FOR_CMD_LIST
    /* cmdlist */ 0,
    /* cmdpatterns */ 0,
#else
    /* cmdlist */ scpi_commands,
#endif
    /* buffer */
    { /* length */ SCPI_INPUT_BUFFER_LENGTH, /* position */ 0, /* data */ scpi_input_buffer},
    /* param_list */
    { /* cmd */ NULL, /* lex_state */
        {NULL, NULL, 0}, /* cmd_raw */
        {0, 0, NULL}},
    /* interface */ &scpi_interface,
    /* output_count */ 0,
    /* input_count */ 0,
    /* cmd_error */ FALSE,
    /* error_queue */ NULL,
    /* registers */ scpi_regs,
    /* units */ scpi_units_def,
    /* user_context */ NULL,
    /* parser_state */
    { /* programHeader */
        {SCPI_TOKEN_UNKNOWN, NULL, 0}, /* programData */
        {SCPI_TOKEN_UNKNOWN, NULL, 0}, /* numberOfParameters */ 0, /* termination */ SCPI_MESSAGE_TERMINATION_NONE
    },
    /* idn */
    {"MANUFACTURE", "INSTR2013", NULL, "01-02"},
};

scpi_t *SCPI_GetContext() {
#if USE_FULL_PROGMEM_FOR_CMD_LIST
    scpi_context.cmdlist = pgm_get_far_address(scpi_commands);
    scpi_context.cmdpatterns = pgm_get_far_address(scpi_command_patterns);
#endif   
    return &scpi_context;
}