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
    /* {"pattern", callback} *
    
    /* IEEE Mandated Commands (SCPI std V1999.0 4.1.1) */
    {"*CLS", SCPI_CoreCls,},
    {"*ESE", SCPI_CoreEse,},
    {"*ESE?", SCPI_CoreEseQ,},
    {"*ESR?", SCPI_CoreEsrQ,},
    {"*IDN?", SCPI_CoreIdnQ,},
    {"*OPC", SCPI_CoreOpc,},
    {"*OPC?", SCPI_CoreOpcQ,},
    {"*RST", SCPI_CoreRst,},
    {"*SRE", SCPI_CoreSre,},
    {"*SRE?", SCPI_CoreSreQ,},
    {"*STB?", SCPI_CoreStbQ,},
    {"*TST?", SCPI_CoreTstQ,},
    {"*WAI", SCPI_CoreWai,},

    /* Required SCPI commands (SCPI std V1999.0 4.2.1) */
    {"SYSTem:ERRor?", SCPI_SystemErrorNextQ,},
    {"SYSTem:ERRor:NEXT?", SCPI_SystemErrorNextQ,},
    {"SYSTem:ERRor:COUNt?", SCPI_SystemErrorCountQ,},
    {"SYSTem:VERSion?", SCPI_SystemVersionQ,},

    //{"STATus:OPERation?", scpi_stub_callback,},
    //{"STATus:OPERation:EVENt?", scpi_stub_callback,},
    //{"STATus:OPERation:CONDition?", scpi_stub_callback,},
    //{"STATus:OPERation:ENABle", scpi_stub_callback,},
    //{"STATus:OPERation:ENABle?", scpi_stub_callback,},

    {"STATus:QUEStionable?", SCPI_StatusQuestionableEventQ,},
    {"STATus:QUEStionable:EVENt?", SCPI_StatusQuestionableEventQ,},
    //{"STATus:QUEStionable:CONDition?", scpi_stub_callback,},
    {"STATus:QUEStionable:ENABle", SCPI_StatusQuestionableEnable,},
    {"STATus:QUEStionable:ENABle?", SCPI_StatusQuestionableEnableQ,},

    {"STATus:PRESet", SCPI_StatusPreset,},

    /* DMM */
    {"MEASure:VOLTage:DC?", DMM_MeasureVoltageDcQ,},
    {"CONFigure:VOLTage:DC", DMM_ConfigureVoltageDc,},
    {"MEASure:VOLTage:DC:RATio?", SCPI_StubQ,},
    {"MEASure:VOLTage:AC?", SCPI_StubQ,},
    {"MEASure:CURRent:DC?", SCPI_StubQ,},
    {"MEASure:CURRent:AC?", SCPI_StubQ,},
    {"MEASure:RESistance?", SCPI_StubQ,},
    {"MEASure:FRESistance?", SCPI_StubQ,},
    {"MEASure:FREQuency?", SCPI_StubQ,},
    {"MEASure:PERiod?", SCPI_StubQ,},
    
    {"SYSTem:COMMunication:TCPIP:CONTROL?", SCPI_SystemCommTcpipControlQ,},

    SCPI_CMD_LIST_END
};

static scpi_interface_t scpi_interface = {
    /* error */ SCPI_Error,
    /* write */ SCPI_Write,
    /* control */ SCPI_Control,
    /* flush */ SCPI_Flush,
    /* reset */ SCPI_Reset,
    /* test */ SCPI_Test,
};

#define SCPI_INPUT_BUFFER_LENGTH 256
static char scpi_input_buffer[SCPI_INPUT_BUFFER_LENGTH];

static scpi_reg_val_t scpi_regs[SCPI_REG_COUNT];


scpi_t scpi_context = {
    /* cmdlist */ scpi_commands,
    /* buffer */ { /* length */ SCPI_INPUT_BUFFER_LENGTH, /* position */ 0,  /* data */ scpi_input_buffer, },
    /* paramlist */ { /* cmd */ NULL, /* parameters */ NULL, /* length */ 0, },
    /* interface */ &scpi_interface,
    /* output_count */ 0,
    /* input_count */ 0,
    /* cmd_error */ FALSE,
    /* error_queue */ NULL,
    /* registers */ scpi_regs,
    /* units */ scpi_units_def,
    /* special_numbers */ scpi_special_numbers_def,
    /* user_context */ NULL,
    /* idn */ {"MANUFACTURE", "INSTR2013", NULL, "01-02"},
};

