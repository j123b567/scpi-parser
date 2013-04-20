/*
 * Copyright (c) 2012-2013 Jan Breuer, Richard.hmm
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
 * @file scpi-def-hmm.c
 * @brief SCPI parser test by hmm
 * @details
 * @version V1.0.0
 * @author Richard.hmm
 * @date 2013-4-14
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scpi/scpi.h"
#include "scpi-def-hmm.h"

void user_api(void)
{
#if 1
    fprintf(stderr, "handle instrument in user_api demo\r\n");
#endif
}

/**
 * @detail show demo of using SCPI_ParamInt.
 * @param context
 * @return 
 */
static scpi_result_t HMM_AcquireAverageTime(scpi_t * context) {
    int32_t param1;

    // read first parameter if present
    if (!SCPI_ParamInt(context, &param1, false)) {
        // do something, if parameter not present
        fprintf(stderr, "read parameter error\r\n"); // debug command name 
        return SCPI_RES_ERR;
    }
    fprintf(stderr, "\tP1=%d\r\n", param1);

    // call user api handle instrument
    user_api();

    return SCPI_RES_OK;
}

/**
 * @detail show demo of using SCPI_ParamDouble.
 * @param context
 * @return 
 */
static scpi_result_t HMM_AcquireAttenuation(scpi_t * context) {
    double param1;

    // read first parameter if present
    if (!SCPI_ParamDouble(context, &param1, false)) {
        // do something, if parameter not present
        fprintf(stderr, "read parameter error\r\n"); // debug command name 
        return SCPI_RES_ERR;
    }
    fprintf(stderr, "\tP1=%lf\r\n", param1);

    // call user api handle instrument
    user_api();

    return SCPI_RES_OK;
}

/**
 * @detail show demo of using SCPI_ParamNumber.
 * @param context
 * @return 
*/
static scpi_result_t HMM_AcquireWavelength(scpi_t * context) {
    scpi_number_t param1;
    char bf[15];

    //fprintf(stderr, "ACQuire:WAVelength\r\n"); // debug command name   

    // read first parameter if present
    if (!SCPI_ParamNumber(context, &param1, false)) {
        // do something, if parameter not present
        SCPI_ErrorPush(context, SCPI_ERROR_MISSING_PARAMETER);
        return SCPI_RES_ERR;
    }

    SCPI_NumberToStr(context, &param1, bf, 15);
    fprintf(stderr, "\tP1=%s\r\n", bf);

    // call user api handle instrument
    user_api();

    return SCPI_RES_OK;
}

/**
 * @detail show demo of using SCPI_ParamString.
 * @param context
 * @return 
*/
static scpi_result_t HMM_AnalysisFmarkerLmtechnique(scpi_t * context) {
    const char *param1;
    size_t len;
    //fprintf(stderr, "COMMunicate:HEADer\r\n"); // debug command name   

    // read first parameter if present
    if (!SCPI_ParamString(context, &param1, &len, false)) {
        // do something, if parameter not present
        SCPI_ErrorPush(context, SCPI_ERROR_MISSING_PARAMETER);
        //fprintf(stderr, "read parameter error\r\n"); // debug command name 
        return SCPI_RES_ERR;
    }
    fprintf(stderr, "\tP1=%s\r\n", param1);

    // call user api handle instrument
    user_api();

    return SCPI_RES_OK;
}

/**
 * @detail show demo of using SCPI_ResultDouble.
 * @param context
 * @return 
*/
static scpi_result_t HMM_AcquireSmpintervalValueQ(scpi_t * context) {
    double result;


    // call user api handle instrument
    user_api();

    // get query result
    result = 4.0;  // 4.0 from input in this demo

    SCPI_ResultDouble(context, result);
    
    return SCPI_RES_OK;
}

/**
 * @detail show demo of using SCPI_ResultText.
 * @param context
 * @return 
*/
static scpi_result_t HMM_WavedataSendAsciiQ(scpi_t * context) {
    const char result[100] = "0.00,1.23,2.34,3.45";


    // call user api handle instrument
    user_api();

    // get query result
    //result[] = 4.0;  // 4.0 from input in this demo

    SCPI_ResultText(context, result);
    
    return SCPI_RES_OK;
}

/**
 * @detail show demo of using SCPI_ResultString.
 * @param context
 * @return 
*/
static scpi_result_t HMM_WavedataSendBinaryQ(scpi_t * context) {
    const char result[100] = "#6123456789ABCDEF01111";


    // call user api handle instrument
    user_api();

    // get query result
    //result[] = 4.0;  // 4.0 from input in this demo

    SCPI_ResultString(context, result);

    return SCPI_RES_OK;
}



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

#if 1
    /* DMM */
    {.pattern = "[:MEASure]:VOLTage:DC?", .callback = DMM_MeasureVoltageDcQ,},
    {.pattern = "CONFigure:VOLTage:DC", .callback = DMM_ConfigureVoltageDc,},
    {.pattern = "MEASure:VOLTage:DC:RATio?", .callback = SCPI_StubQ,},
    {.pattern = "MEASure:VOLTage:AC?", .callback = SCPI_StubQ,},
    {.pattern = "MEASure:CURRent:DC?", .callback = SCPI_StubQ,},
    {.pattern = "MEASure:CURRent:AC?", .callback = SCPI_StubQ,},
    {.pattern = "MEASure:RESistance?", .callback = SCPI_StubQ,},
    {.pattern = "MEASure:FRESistance?", .callback = SCPI_StubQ,},
    {.pattern = "MEASure:FREQuency?", .callback = SCPI_StubQ,},
    {.pattern = "MEASure:PERiod?", .callback = SCPI_StubQ,},
    
    {.pattern = "SYSTem:COMMunication:TCPIP:CONTROL?", .callback = SCPI_SystemCommTcpipControlQ,},
#endif

    /* HMM */
    {.pattern = "ACQuire:WAVelength", .callback = HMM_AcquireWavelength,},
    {.pattern = "ACQuire:ATTenuation", .callback = HMM_AcquireAttenuation,},
    {.pattern = "ACQuire[:SMPinterval]:VALue?", .callback = HMM_AcquireSmpintervalValueQ,},
    {.pattern = "ACQuire:AVERage[:TIME]", .callback = HMM_AcquireAverageTime,},
    {.pattern = "[:ANALysis]:FMARker:LMTechnique", .callback = HMM_AnalysisFmarkerLmtechnique,},
    {.pattern = "WAVedata[:SEND]:ASCii?", .callback = HMM_WavedataSendAsciiQ,},
    {.pattern = "WAVedata:SEND[:BINary]?", .callback = HMM_WavedataSendBinaryQ,},

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
};
