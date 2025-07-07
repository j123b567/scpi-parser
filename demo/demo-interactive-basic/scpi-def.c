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


typedef enum waveform_type_t
{
    WAVEFORM_TYPE_SINE,
    WAVEFORM_TYPE_SQUARE,
    WAVEFORM_TYPE_TRIANGLE,
    WAVEFORM_TYPE_RAMP,
} waveform_type_t;

struct Channel
{
    double sample_rate;
    double sample_size;
    double sample_format;
    int sample_count;
    double frequency;
    double amplitude;
    double offset;
    double phase;
    double waveform;
    waveform_type_t waveform_type;
    double waveform_frequency;
};

struct Awg
{

    double sample_size;
    double sample_format;
    double sample_rate;
    int32_t sample_count;
    int64_t operation_duration;
    scpi_bool_t feature_enabled;
    char device_name;
    double frequency_max;
    double frequency_min;
    double frequency_default;
    double amplitude;
    double offset;
    double phase;
    double waveform;
    waveform_type_t waveform_type;
    double waveform_frequency;

    struct Channel channel[8];
    int channel_count;
};

#define MAX_FILE_NAME_LENGTH 256

struct Awg awg;

static scpi_result_t SCPI_SystemHelpHeaders(scpi_t *context) {
   SCPI_ResultText(context, "AWG:VOLTage?\n");
    SCPI_ResultText(context, "AWG:VOLTage\n");
    SCPI_ResultText(context, "AWG:Count\n");
    SCPI_ResultText(context, "AWG:Count\n?");
    SCPI_ResultText(context, "AWG:Duration\n");
    SCPI_ResultText(context, "AWG:Duration?\n");
    SCPI_ResultText(context, "AWG:Enable\n");
    SCPI_ResultText(context, "AWG:Enable?\n");
    SCPI_ResultText(context, "AWG:WAVEform\n");
    SCPI_ResultText(context, "AWG:WAVEform?\n");
    SCPI_ResultText(context, "AWG:NAME\n");
    SCPI_ResultText(context, "AWG:NAME?\n");
    SCPI_ResultText(context, "AWG:ARB:LOAD\n");
    SCPI_ResultText(context, "AWG:FREQ:INST?\n");
    SCPI_ResultText(context, "SYSTem:HELP:HEADers?\n");
    SCPI_ResultText(context, "MMEMory:DATA\n");
    SCPI_ResultText(context, "MMEMory:DATA:APPend\n");

    return SCPI_RES_OK;
}


//Implementation of double type voltage query command
static scpi_result_t SCPI_MyDevVoltageQuery(scpi_t *context) {
    SCPI_ResultDouble(context, awg.amplitude);
    return SCPI_RES_OK;
}
//Implementation of double type voltage setting command
static scpi_result_t SCPI_MyDevVoltageSet(scpi_t *context) {

    double val = 0;
    if (!SCPI_ParamDouble(context, &val, TRUE)) {
        return SCPI_RES_ERR;
    }

    awg.amplitude = val;
    return SCPI_RES_OK;
}

//Implementation of Counting Query Command现
static scpi_result_t SCPI_SampleCountQuery(scpi_t *context){
    SCPI_ResultInt32(context, awg.sample_count);
    return SCPI_RES_OK;
}
//Set Operation Duration Command
static scpi_result_t SCPI_SampleCountSet(scpi_t *context){
    int32_t count = 0;
    if (!SCPI_ParamInt32(context, &count, TRUE)) {
        return SCPI_RES_ERR;
    }

    awg.sample_count = count;
    return SCPI_RES_OK;
}


//Set Operation Duration Command
static scpi_result_t SCPI_DurationSet(scpi_t *context){
    int64_t duration = 0;
    if (!SCPI_ParamInt64(context, &duration, TRUE)) {
        return SCPI_RES_ERR;
    }

    awg.operation_duration = duration;
    return SCPI_RES_OK;
}
//Query Operation Duration Command
static scpi_result_t SCPI_DurationQuery(scpi_t *context){
    SCPI_ResultInt64(context,awg.operation_duration);
    return SCPI_RES_OK;
}

//Implementation of the Enable Function Switch Command
static scpi_result_t SCPI_EnableSet(scpi_t *context) {
    scpi_bool_t enable = FALSE; 

     if (!SCPI_ParamBool(context, &enable, TRUE)) {
        return SCPI_RES_ERR; 
    }

    awg.feature_enabled = enable; 

    return SCPI_RES_OK;
}
//Implementation of query enabling function switch command
static scpi_result_t SCPI_EnableQuery(scpi_t *context) {
    SCPI_ResultBool(context, awg.feature_enabled);
    return SCPI_RES_OK;
}


//Definition of waveform type enumeration
scpi_choice_def_t waveform_type[] = {
    { "SINE", WAVEFORM_TYPE_SINE },
    { "SQUARE", WAVEFORM_TYPE_SQUARE },
    { "TRIANGLE", WAVEFORM_TYPE_TRIANGLE },
    { "RAMP", WAVEFORM_TYPE_RAMP },
    { NULL, 0 },

    SCPI_CHOICE_LIST_END
};

//Implementation of Set Waveform Type Command
static scpi_result_t SCPI_WaveFormSet(scpi_t *context) {
    int32_t param;
     if (!SCPI_ParamChoice(context, waveform_type, &param, TRUE)) {
        return SCPI_RES_ERR;
    }
    awg.waveform_type = (waveform_type_t)param;  // Set waveform type
    return SCPI_RES_OK;
}
//Implementation of Query Waveform Type Command
static scpi_result_t SCPI_WaveFormQuery(scpi_t *context) {
    SCPI_ResultInt32(context, awg.waveform_type);
    return SCPI_RES_OK;
}

static scpi_result_t SCPI_DevNameSet(scpi_t *context) {
    char device_name[100];
    size_t length;

    if (!SCPI_ParamCopyText(context, device_name, sizeof(device_name), &length, TRUE)) {
        return SCPI_RES_ERR;
    }
    strncpy(awg.device_name, device_name, sizeof(awg.device_name));
    return SCPI_RES_OK;
}

//Implementation of Query Device Name Command
static scpi_result_t SCPI_DevNameQuery(scpi_t *context) {
    // Here the name of the current device can be returned, for example from a variable set earlier.
    const char *device_name = "My Device"; // Suppose this is the current device name

    SCPI_ResultText(context, awg.device_name);
    return SCPI_RES_OK;
}

//Load Custom Waveform Command
static scpi_result_t  SCPI_LoadArbitraryWaveform(scpi_t *context){
    const char *data;
    size_t length;

    if (SCPI_ParamArbitraryBlock(context, &data, &length, TRUE)) {
        // load_waveform(data, length);  // Assume that load _ waveform is a function of loading waveform data
        //mmemdata
        return SCPI_RES_OK;
    }

    return SCPI_RES_OK;
}

//Maximum, minimum, and default values for query frequency
static scpi_result_t SCPI_FrequencyInstQ(scpi_t *context) {
    const char *query_type;
    size_t length;

    if (!SCPI_ParamCopyText(context, query_type,sizeof(query_type),&length, TRUE)) {
        return SCPI_RES_ERR;
    }

    if (strcmp(query_type, "MAX") == 0) {
        SCPI_ResultDouble(context, awg.frequency_max);  //Return the maximum frequency
    } else if (strcmp(query_type, "MIN") == 0) {
        SCPI_ResultDouble(context, awg.frequency_min);  // Return minimum frequency
    } else if (strcmp(query_type, "DEF") == 0) {
        SCPI_ResultDouble(context, awg.frequency_default);  // Return to default frequency
    } else {
        return SCPI_RES_ERR;


    return SCPI_RES_OK;
}
}

static scpi_result_t SCPI_MemoryDataAppend(scpi_t *context){
    char file_name[MAX_FILE_NAME_LENGTH] = {0};
    const char *data;
    size_t data_length;

    if (!SCPI_ParamCopyText(context, file_name, sizeof(file_name), &data_length, TRUE)) {
         return SCPI_RES_ERR;
     }

    if (!SCPI_ParamArbitraryBlock(context, &data, &data_length, TRUE)) {
        return SCPI_RES_ERR; 
    }

    FILE *file = fopen(file_name, "a");
    if (file == NULL) {
        return SCPI_RES_ERR;
    }

    fwrite(data, 1, data_length, file);

    fclose(file);
    return SCPI_RES_OK;
}


// // //多参数命令
// static scpi_result_t SCPI_RunCommand(scpi_t *context) {
//     int32_t mode;
//     const char *trigger_source;

//     if (!SCPI_ParamInt32(context, &mode, TRUE)) {
//         return SCPI_RES_ERR;
//     }

//     if (!SCPI_ParamText(context, &trigger_source, FALSE)) {
//         return SCPI_RES_ERR;
//     }

//     // 执行命令
//     run(mode, trigger_source);  // 假设 run() 是实际执行的函数

//     return SCPI_RES_OK;
// }


// //无参数的 Action 命令
// static scpi_result_t SCPI_RunAction(scpi_t *context) {
//     run();  // 执行实际动作
//     return SCPI_RES_OK;
// }

static scpi_result_t SCPI_MemoryDataSet(scpi_t *context) {
    char file_name[MAX_FILE_NAME_LENGTH] = {0};
    const char *data;
    size_t length;

    if(!SCPI_ParamCopyText(context, file_name, sizeof(file_name), &length, TRUE)) {
        return SCPI_RES_ERR;
    }

    if(!SCPI_ParamArbitraryBlock(context, &data, &length, TRUE)) {
        
        return SCPI_RES_ERR;
    }

    FILE *file = fopen(file_name, "wb");
    if (file == NULL) {
        return SCPI_RES_ERR;
    }

    if (fwrite(data, 1, length, file) != length) {
        fclose(file);
        return SCPI_RES_ERR;
    }

    return SCPI_RES_OK;
}




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
    { .pattern = "*TST?", .callback = My_CoreTstQ,},
    { .pattern = "*WAI", .callback = SCPI_CoreWai,},

    /* Required SCPI commands (SCPI std V1999.0 4.2.1) */
    {.pattern = "SYSTem:ERRor[:NEXT]?", .callback = SCPI_SystemErrorNextQ,},
    {.pattern = "SYSTem:ERRor:COUNt?", .callback = SCPI_SystemErrorCountQ,},
    {.pattern = "SYSTem:VERSion?", .callback = SCPI_SystemVersionQ,},

    /* {.pattern = "STATus:OPERation?", .callback = scpi_stub_callback,}, */
    /* {.pattern = "STATus:OPERation:EVENt?", .callback = scpi_stub_callback,}, */
    /* {.pattern = "STATus:OPERation:CONDition?", .callback = scpi_stub_callback,}, */
    /* {.pattern = "STATus:OPERation:ENABle", .callback = scpi_stub_callback,}, */
    /* {.pattern = "STATus:OPERation:ENABle?", .callback = scpi_stub_callback,}, */

    {.pattern = "STATus:QUEStionable[:EVENt]?", .callback = SCPI_StatusQuestionableEventQ,},
    /* {.pattern = "STATus:QUEStionable:CONDition?", .callback = scpi_stub_callback,}, */
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
    {.pattern = "TEST:TEXT", .callback = TEST_Text,},
    {.pattern = "TEST:ARBitrary?", .callback = TEST_ArbQ,},
    {.pattern = "TEST:CHANnellist", .callback = TEST_Chanlst,},

    //double type
    {.pattern="AWG:VOLTage?",.callback=SCPI_MyDevVoltageQuery},
    {.pattern="AWG:VOLTage",.callback=SCPI_MyDevVoltageSet},

    //int32 type, set and query sample count commands
    {.pattern="AWG:Count",.callback=SCPI_SampleCountSet},
    {.pattern="AWG:Count?",.callback=SCPI_SampleCountQuery},

    //Int64 type, set and query operation duration commands
    {.pattern="AWG:Duration",.callback=SCPI_DurationSet},
    {.pattern="AWG:Duration?",.callback=SCPI_DurationQuery},

    //Bool type, setting and query enable feature switch commands
    {.pattern="AWG:Enable",.callback=SCPI_EnableSet},
    {.pattern="AWG:Enable?",.callback=SCPI_EnableQuery},

    // Enum type, set and query waveform type commands
    {.pattern="AWG:WAVEform",.callback=SCPI_WaveFormSet},
    {.pattern="AWG:WAVEform?",.callback=SCPI_WaveFormQuery},

    // String type, set device name command
    {.pattern="AWG:NAME",.callback=SCPI_DevNameSet},
    {.pattern="AWG:NAME?",.callback=SCPI_DevNameQuery},

    //Arb type, load custom waveform command
    {.pattern="AWG:ARB:LOAD",.callback=SCPI_LoadArbitraryWaveform},

    //Commands with numeric suffixes, query the maximum, minimum and default values of frequency
    {.pattern="AWG:FREQ:INST?",.callback=SCPI_FrequencyInstQ},

    //Query system help information
    { .pattern = "SYSTem:HELP:HEADers?", .callback = SCPI_SystemHelpHeaders },

    //Storing waveform data
     { .pattern = "MMEMory:DATA", .callback = SCPI_MemoryDataSet },

    // Append data to memory
     { .pattern = "MMEMory:DATA:APPend", .callback = SCPI_MemoryDataAppend },

    // Multi-parameter command
    // { .pattern = "RUN", .callback = SCPI_RunCommand },

    // Action commands without parameters
    // { .pattern = "RUN", .callback = SCPI_RunAction },





    SCPI_CMD_LIST_END
};

scpi_interface_t scpi_interface = {
    .error = SCPI_Error,
    .write = SCPI_Write,
    .control = SCPI_Control,
    .flush = SCPI_Flush,
    .reset = SCPI_Reset,
};

char scpi_input_buffer[SCPI_INPUT_BUFFER_LENGTH];
scpi_error_t scpi_error_queue_data[SCPI_ERROR_QUEUE_SIZE];

scpi_t scpi_context;
