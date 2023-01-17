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

static scpi_result_t DMM_MeasureVoltageDcQ(scpi_t * context) {
    scpi_number_t param1, param2;
    char bf[15];
    fprintf(stderr, "meas:volt:dc\r\n"); /* debug command name */

    /* read first parameter if present */
    if (!SCPI_ParamNumber(context, scpi_special_numbers_def, &param1, FALSE)) {
        /* do something, if parameter not present */
    }

    /* read second paraeter if present */
    if (!SCPI_ParamNumber(context, scpi_special_numbers_def, &param2, FALSE)) {
        /* do something, if parameter not present */
    }


    SCPI_NumberToStr(context, scpi_special_numbers_def, &param1, bf, 15);
    fprintf(stderr, "\tP1=%s\r\n", bf);


    SCPI_NumberToStr(context, scpi_special_numbers_def, &param2, bf, 15);
    fprintf(stderr, "\tP2=%s\r\n", bf);

    SCPI_ResultDouble(context, 0);

    return SCPI_RES_OK;
}

static scpi_result_t DMM_MeasureVoltageAcQ(scpi_t * context) {
    scpi_number_t param1, param2;
    char bf[15];
    fprintf(stderr, "meas:volt:ac\r\n"); /* debug command name */

    /* read first parameter if present */
    if (!SCPI_ParamNumber(context, scpi_special_numbers_def, &param1, FALSE)) {
        /* do something, if parameter not present */
    }

    /* read second paraeter if present */
    if (!SCPI_ParamNumber(context, scpi_special_numbers_def, &param2, FALSE)) {
        /* do something, if parameter not present */
    }


    SCPI_NumberToStr(context, scpi_special_numbers_def, &param1, bf, 15);
    fprintf(stderr, "\tP1=%s\r\n", bf);


    SCPI_NumberToStr(context, scpi_special_numbers_def, &param2, bf, 15);
    fprintf(stderr, "\tP2=%s\r\n", bf);

    SCPI_ResultDouble(context, 0);

    return SCPI_RES_OK;
}

static scpi_result_t DMM_ConfigureVoltageDc(scpi_t * context) {
    double param1, param2;
    fprintf(stderr, "conf:volt:dc\r\n"); /* debug command name */

    /* read first parameter if present */
    if (!SCPI_ParamDouble(context, &param1, TRUE)) {
        return SCPI_RES_ERR;
    }

    /* read second paraeter if present */
    if (!SCPI_ParamDouble(context, &param2, FALSE)) {
        /* do something, if parameter not present */
    }

    fprintf(stderr, "\tP1=%lf\r\n", param1);
    fprintf(stderr, "\tP2=%lf\r\n", param2);

    return SCPI_RES_OK;
}

static scpi_result_t TEST_Bool(scpi_t * context) {
    scpi_bool_t param1;
    fprintf(stderr, "TEST:BOOL\r\n"); /* debug command name */

    /* read first parameter if present */
    if (!SCPI_ParamBool(context, &param1, TRUE)) {
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

static scpi_result_t TEST_ChoiceQ(scpi_t * context) {

    int32_t param;
    const char * name;

    if (!SCPI_ParamChoice(context, trigger_source, &param, TRUE)) {
        return SCPI_RES_ERR;
    }

    SCPI_ChoiceToName(trigger_source, param, &name);
    fprintf(stderr, "\tP1=%s (%ld)\r\n", name, (long int) param);

    SCPI_ResultInt32(context, param);

    return SCPI_RES_OK;
}

static scpi_result_t TEST_Numbers(scpi_t * context) {
    int32_t numbers[2];

    SCPI_CommandNumbers(context, numbers, 2, 1);

    fprintf(stderr, "TEST numbers %d %d\r\n", numbers[0], numbers[1]);

    return SCPI_RES_OK;
}

static scpi_result_t TEST_Text(scpi_t * context) {
    char buffer[100];
    size_t copy_len;

    if (!SCPI_ParamCopyText(context, buffer, sizeof (buffer), &copy_len, FALSE)) {
        buffer[0] = '\0';
    }

    fprintf(stderr, "TEXT: ***%s***\r\n", buffer);

    return SCPI_RES_OK;
}

static scpi_result_t TEST_ArbQ(scpi_t * context) {
    const char * data;
    size_t len;

    if (SCPI_ParamArbitraryBlock(context, &data, &len, FALSE)) {
        SCPI_ResultArbitraryBlock(context, data, len);
    }

    return SCPI_RES_OK;
}

struct _scpi_channel_value_t {
    int32_t row;
    int32_t col;
};
typedef struct _scpi_channel_value_t scpi_channel_value_t;

/**
 * @brief
 * parses lists
 * channel numbers > 0.
 * no checks yet.
 * valid: (@1), (@3!1:1!3), ...
 * (@1!1:3!2) would be 1!1, 1!2, 2!1, 2!2, 3!1, 3!2.
 * (@3!1:1!3) would be 3!1, 3!2, 3!3, 2!1, 2!2, 2!3, ... 1!3.
 *
 * @param channel_list channel list, compare to SCPI99 Vol 1 Ch. 8.3.2
 */
static scpi_result_t TEST_Chanlst(scpi_t *context) {
    scpi_parameter_t channel_list_param;
#define MAXROW 2    /* maximum number of rows */
#define MAXCOL 6    /* maximum number of columns */
#define MAXDIM 2    /* maximum number of dimensions */
    scpi_channel_value_t array[MAXROW * MAXCOL]; /* array which holds values in order (2D) */
    size_t chanlst_idx; /* index for channel list */
    size_t arr_idx = 0; /* index for array */
    size_t n, m = 1; /* counters for row (n) and columns (m) */

    /* get channel list */
    if (SCPI_Parameter(context, &channel_list_param, TRUE)) {
        scpi_expr_result_t res;
        scpi_bool_t is_range;
        int32_t values_from[MAXDIM];
        int32_t values_to[MAXDIM];
        size_t dimensions;

        bool for_stop_row = FALSE; /* true if iteration for rows has to stop */
        bool for_stop_col = FALSE; /* true if iteration for columns has to stop */
        int32_t dir_row = 1; /* direction of counter for rows, +/-1 */
        int32_t dir_col = 1; /* direction of counter for columns, +/-1 */

        /* the next statement is valid usage and it gets only real number of dimensions for the first item (index 0) */
        if (!SCPI_ExprChannelListEntry(context, &channel_list_param, 0, &is_range, NULL, NULL, 0, &dimensions)) {
            chanlst_idx = 0; /* call first index */
            arr_idx = 0; /* set arr_idx to 0 */
            do { /* if valid, iterate over channel_list_param index while res == valid (do-while cause we have to do it once) */
                res = SCPI_ExprChannelListEntry(context, &channel_list_param, chanlst_idx, &is_range, values_from, values_to, 4, &dimensions);
                if (is_range == FALSE) { /* still can have multiple dimensions */
                    if (dimensions == 1) {
                        /* here we have our values
                         * row == values_from[0]
                         * col == 0 (fixed number)
                         * call a function or something */
                        array[arr_idx].row = values_from[0];
                        array[arr_idx].col = 0;
                    } else if (dimensions == 2) {
                        /* here we have our values
                         * row == values_fom[0]
                         * col == values_from[1]
                         * call a function or something */
                        array[arr_idx].row = values_from[0];
                        array[arr_idx].col = values_from[1];
                    } else {
                        return SCPI_RES_ERR;
                    }
                    arr_idx++; /* inkrement array where we want to save our values to, not neccessary otherwise */
                    if (arr_idx >= MAXROW * MAXCOL) {
                        return SCPI_RES_ERR;
                    }
                } else if (is_range == TRUE) {
                    if (values_from[0] > values_to[0]) {
                        dir_row = -1; /* we have to decrement from values_from */
                    } else { /* if (values_from[0] < values_to[0]) */
                        dir_row = +1; /* default, we increment from values_from */
                    }

                    /* iterating over rows, do it once -> set for_stop_row = false
                     * needed if there is channel list index isn't at end yet */
                    for_stop_row = FALSE;
                    for (n = values_from[0]; for_stop_row == FALSE; n += dir_row) {
                        /* usual case for ranges, 2 dimensions */
                        if (dimensions == 2) {
                            if (values_from[1] > values_to[1]) {
                                dir_col = -1;
                            } else if (values_from[1] < values_to[1]) {
                                dir_col = +1;
                            }
                            /* iterating over columns, do it at least once -> set for_stop_col = false
                             * needed if there is channel list index isn't at end yet */
                            for_stop_col = FALSE;
                            for (m = values_from[1]; for_stop_col == FALSE; m += dir_col) {
                                /* here we have our values
                                 * row == n
                                 * col == m
                                 * call a function or something */
                                array[arr_idx].row = n;
                                array[arr_idx].col = m;
                                arr_idx++;
                                if (arr_idx >= MAXROW * MAXCOL) {
                                    return SCPI_RES_ERR;
                                }
                                if (m == (size_t)values_to[1]) {
                                    /* endpoint reached, stop column for-loop */
                                    for_stop_col = TRUE;
                                }
                            }
                            /* special case for range, example: (@2!1) */
                        } else if (dimensions == 1) {
                            /* here we have values
                             * row == n
                             * col == 0 (fixed number)
                             * call function or sth. */
                            array[arr_idx].row = n;
                            array[arr_idx].col = 0;
                            arr_idx++;
                            if (arr_idx >= MAXROW * MAXCOL) {
                                return SCPI_RES_ERR;
                            }
                        }
                        if (n == (size_t)values_to[0]) {
                            /* endpoint reached, stop row for-loop */
                            for_stop_row = TRUE;
                        }
                    }


                } else {
                    return SCPI_RES_ERR;
                }
                /* increase index */
                chanlst_idx++;
            } while (SCPI_EXPR_OK == SCPI_ExprChannelListEntry(context, &channel_list_param, chanlst_idx, &is_range, values_from, values_to, 4, &dimensions));
            /* while checks, whether incremented index is valid */
        }
        /* do something at the end if needed */
        /* array[arr_idx].row = 0; */
        /* array[arr_idx].col = 0; */
    }

    {
        size_t i;
        fprintf(stderr, "TEST_Chanlst: ");
        for (i = 0; i< arr_idx; i++) {
            fprintf(stderr, "%d!%d, ", array[i].row, array[i].col);
        }
        fprintf(stderr, "\r\n");
    }
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
static scpi_result_t My_CoreTstQ(scpi_t * context) {

    SCPI_ResultInt32(context, 0);

    return SCPI_RES_OK;
}


const scpi_command_t scpi_commands[] = {
    /* Optional help command */
#if USE_HELP_FILTER
    {"HELP?", SCPI_HelpQ, SCPI_CMD_DESC("[<string>] - list supported commands [containing \"<string>\"] (multiple block data)") SCPI_CMD_TAG(0)},
#else 
    {"HELP?", SCPI_HelpQ, SCPI_CMD_DESC("\t - list supported commands (multiple block data)") SCPI_CMD_TAG(0)},
#endif
    /* IEEE Mandated Commands (SCPI std V1999.0 4.1.1): *CLS *ESE *ESE? *ESR? *IDN? *OPC *OPC? *RST *SRE *SRE? *STB? *TST? *WAI  */
    {"*CLS",  SCPI_CoreCls,  SCPI_CMD_DESC("\t - clear all Event Status registers, errors, output queue") SCPI_CMD_TAG(0)},
    /* -- Standard Event Status Group [EVENT]-[ENABLE] */
    {"*ESE",  SCPI_CoreEse,  SCPI_CMD_DESC("<0..255> - set Standard Event Status Enable / event mask") SCPI_CMD_TAG(0)},
    {"*ESE?", SCPI_CoreEseQ, SCPI_CMD_DESC("\t - read ESE (0..255)") SCPI_CMD_TAG(0)},
    {"*ESR?", SCPI_CoreEsrQ, SCPI_CMD_DESC("\t - read+clear Standard Event Status register (0..255)") SCPI_CMD_TAG(0)},
    /* -- IEEE Mandated Commands (continued ...) */
    {"*IDN?", SCPI_CoreIdnQ, SCPI_CMD_DESC("\t - read device identifier (str,str,str,str)") SCPI_CMD_TAG(0)},
    {"*OPC",  SCPI_CoreOpc,  SCPI_CMD_DESC("\t - complete ops preceding Operation Complete Command, set ESR.OPC 1") SCPI_CMD_TAG(0)},
    {"*OPC?", SCPI_CoreOpcQ, SCPI_CMD_DESC("\t - read ESR.OPC (0:ongoing-ops 1:done)") SCPI_CMD_TAG(0)},
    {"*RST",  SCPI_CoreRst,  SCPI_CMD_DESC("\t - reset instrument and interface") SCPI_CMD_TAG(0)},
    {"*SRE",  SCPI_CoreSre,  SCPI_CMD_DESC("<0..255> - set Service Request Enable / event mask over STB") SCPI_CMD_TAG(0)},
    {"*SRE?", SCPI_CoreSreQ, SCPI_CMD_DESC("\t - read SRE (0..255)") SCPI_CMD_TAG(0)},
    {"*STB?", SCPI_CoreStbQ, SCPI_CMD_DESC("\t - read STatus Byte (0..255)") SCPI_CMD_TAG(0)},
    {"*TST?", My_CoreTstQ,   SCPI_CMD_DESC("\t - read self-test result (0:no-failures)") SCPI_CMD_TAG(0)},
    {"*WAI",  SCPI_CoreWai,  SCPI_CMD_DESC("\t - halt cmd execution until pending operations complete") SCPI_CMD_TAG(0)},
    
    /* Required SCPI commands (SCPI std V1999.0 4.2.1) : SYSTem:ERRor, STATus:OPERation, STATus:QUEStionable and STATus:PRESet */
    {"SYSTem:ERRor[:NEXT]?", SCPI_SystemErrorNextQ,  SCPI_CMD_DESC("\t - get next error in queue (int-errno,str)") SCPI_CMD_TAG(0)},
    {"SYSTem:ERRor:COUNt?",  SCPI_SystemErrorCountQ, SCPI_CMD_DESC("\t - queued error count (0.." SCPI_ERROR_QUEUE_SIZE_STR ")") SCPI_CMD_TAG(0)},
    {"SYSTem:VERSion?",      SCPI_SystemVersionQ,    SCPI_CMD_DESC("\t - query system version (str)") SCPI_CMD_TAG(0)},
    /* -- Operation Status Group [CONDITION]-[EVENT]-[ENABLE] */
    {"STATus:OPERation:CONDition?", SCPI_StubQ, SCPI_CMD_DESC("\t - not implemented (0)") SCPI_CMD_TAG(0)},
    {"STATus:OPERation[:EVENt]?", SCPI_StubQ, SCPI_CMD_DESC("\t - not implemented (0)") SCPI_CMD_TAG(0)},
    {"STATus:OPERation:ENABle", SCPI_Stub, SCPI_CMD_DESC("\t - not implemented") SCPI_CMD_TAG(0)},
    {"STATus:OPERation:ENABle?", SCPI_StubQ, SCPI_CMD_DESC("\t - not implemented (0)") SCPI_CMD_TAG(0)},
    /* -- Questionable Status Group [CONDITION]-[EVENT]-[ENABLE] */
    {"STATus:QUEStionable:CONDition?", SCPI_StubQ, SCPI_CMD_DESC("\t - not implemented (0)") SCPI_CMD_TAG(0)}, /* "\t - read momentary Questionable Condition register (0..65535)" */
    {"STATus:QUEStionable[:EVENt]?",   SCPI_StatusQuestionableEventQ, SCPI_CMD_DESC("\t - read Questionable Event register (0..65535)") SCPI_CMD_TAG(0)},
    {"STATus:QUEStionable:ENABle",     SCPI_StatusQuestionableEnable, SCPI_CMD_DESC("<0..65535> - set Questionable Enable / event mask") SCPI_CMD_TAG(0)},
    {"STATus:QUEStionable:ENABle?",    SCPI_StatusQuestionableEnableQ, SCPI_CMD_DESC("\t - Questionable Status Enable (0..65535)") SCPI_CMD_TAG(0)},
    {"STATus:PRESet", SCPI_StatusPreset, SCPI_CMD_DESC("\t - load Status sub-system register defaults") SCPI_CMD_TAG(0)},
    
    /* commands specific to DMM example with TEST sub-system */
    {"SYSTem:COMMunication:TCPIP:CONTROL?", SCPI_SystemCommTcpipControlQ, SCPI_CMD_DESC("\t - read TCPIP control port (int)") SCPI_CMD_TAG(0)},
    {"CONFigure:VOLTage:DC", DMM_ConfigureVoltageDc, SCPI_CMD_DESC("<float>[,<float>] - test command") SCPI_CMD_TAG(0)},
    {"MEASure:VOLTage:DC?", DMM_MeasureVoltageDcQ, SCPI_CMD_DESC("[<float>[,<float>]] - test command (0)") SCPI_CMD_TAG(0)},
    {"MEASure:VOLTage:DC:RATio?", SCPI_StubQ, SCPI_CMD_DESC("\t - not implemented  (0)") SCPI_CMD_TAG(0)},
    {"MEASure:VOLTage:AC?", DMM_MeasureVoltageAcQ, SCPI_CMD_DESC("[<float>[,<float>]] - test command (0)") SCPI_CMD_TAG(0)},
    {"MEASure:CURRent:DC?", SCPI_StubQ, SCPI_CMD_DESC("\t - not implemented (0)") SCPI_CMD_TAG(0)},
    {"MEASure:CURRent:AC?", SCPI_StubQ, SCPI_CMD_DESC("\t - not implemented (0)") SCPI_CMD_TAG(0)},
    {"MEASure:RESistance?", SCPI_StubQ, SCPI_CMD_DESC("\t - not implemented (0)") SCPI_CMD_TAG(0)},
    {"MEASure:FRESistance?", SCPI_StubQ, SCPI_CMD_DESC("\t - not implemented (0)") SCPI_CMD_TAG(0)},
    {"MEASure:FREQuency?", SCPI_StubQ, SCPI_CMD_DESC("\t - not implemented (0)") SCPI_CMD_TAG(0)},
    {"MEASure:PERiod?", SCPI_StubQ, SCPI_CMD_DESC("\t - not implemented (0)") SCPI_CMD_TAG(0)},
    {"TEST:BOOL", TEST_Bool, SCPI_CMD_DESC("<bool> - test command") SCPI_CMD_TAG(0)},
    {"TEST:CHOice?", TEST_ChoiceQ, SCPI_CMD_DESC("<choice> - test command (\"BUS\":5 \"IMMediate\":6 \"EXTernal\":7)") SCPI_CMD_TAG(0)},
    {"TEST#:NUMbers#", TEST_Numbers, SCPI_CMD_DESC("\t - test command with optional numbers - default 1") SCPI_CMD_TAG(0)},
    {"TEST:TEXT", TEST_Text, SCPI_CMD_DESC("<param>[, ...] - debug print param as received") SCPI_CMD_TAG(0)},
    {"TEST:ARBitrary?", TEST_ArbQ, SCPI_CMD_DESC("<block data> - receive and return block data (block data)") SCPI_CMD_TAG(0)},
    {"TEST:CHANnellist", TEST_Chanlst, SCPI_CMD_DESC("<channel_list> - test channel list parsing") SCPI_CMD_TAG(0)},
    
    SCPI_CMD_LIST_END
};

scpi_interface_t scpi_interface = {
    .error   = SCPI_Error,
    .write   = SCPI_Write,
    .control = SCPI_Control,
    .flush   = SCPI_Flush,
    .reset   = SCPI_Reset,
};

char scpi_input_buffer[SCPI_INPUT_BUFFER_LENGTH];
scpi_error_t scpi_error_queue_data[SCPI_ERROR_QUEUE_SIZE];

scpi_t scpi_context;
