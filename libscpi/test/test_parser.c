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

#include <stdio.h>
#include <stdlib.h>
#include "CUnit/Basic.h"

#include "scpi/scpi.h"
#include "../src/fifo_private.h"

/*
 * CUnit Test Suite
 */

static scpi_result_t text_function(scpi_t* context) {
    char param[100];
    size_t param_len;

    if (!SCPI_ParamCopyText(context, param, 100, &param_len, TRUE)) {
        return SCPI_RES_ERR;
    }

    if (!SCPI_ParamCopyText(context, param, 100, &param_len, TRUE)) {
        return SCPI_RES_ERR;
    }

    SCPI_ResultText(context, param);

    return SCPI_RES_OK;
}

static scpi_result_t test_treeA(scpi_t* context) {

    SCPI_ResultInt32(context, 10);

    return SCPI_RES_OK;
}

static scpi_result_t test_treeB(scpi_t* context) {

    SCPI_ResultInt32(context, 20);

    return SCPI_RES_OK;
}

static double test_sample_received = NAN;

static scpi_result_t SCPI_Sample(scpi_t * context) {
    const char * val;
    size_t len;
    if (!SCPI_ParamArbitraryBlock(context, &val, &len, TRUE)) return SCPI_RES_ERR;
    if (len != sizeof(test_sample_received)) return SCPI_RES_ERR;
    memcpy(&test_sample_received, val, sizeof(test_sample_received));
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
    { .pattern = "SYSTem:ERRor[:NEXT]?", .callback = SCPI_SystemErrorNextQ,},
    { .pattern = "SYSTem:ERRor:COUNt?", .callback = SCPI_SystemErrorCountQ,},
    { .pattern = "SYSTem:VERSion?", .callback = SCPI_SystemVersionQ,},

    { .pattern = "STATus:QUEStionable[:EVENt]?", .callback = SCPI_StatusQuestionableEventQ,},
    { .pattern = "STATus:QUEStionable:CONDition?", .callback = SCPI_StatusQuestionableConditionQ,},
    { .pattern = "STATus:QUEStionable:ENABle", .callback = SCPI_StatusQuestionableEnable,},
    { .pattern = "STATus:QUEStionable:ENABle?", .callback = SCPI_StatusQuestionableEnableQ,},

    {.pattern = "STATus:OPERation[:EVENt]?", .callback = SCPI_StatusOperationEventQ, },
    {.pattern = "STATus:OPERation:CONDition?", .callback = SCPI_StatusOperationConditionQ, },
    {.pattern = "STATus:OPERation:ENABle", .callback = SCPI_StatusOperationEnable, },
    {.pattern = "STATus:OPERation:ENABle?", .callback = SCPI_StatusOperationEnableQ, },

    { .pattern = "STATus:PRESet", .callback = SCPI_StatusPreset,},

    { .pattern = "TEXTfunction?", .callback = text_function,},

    { .pattern = "TEST:TREEA?", .callback = test_treeA,},
    { .pattern = "TEST:TREEB?", .callback = test_treeB,},

    { .pattern = "STUB", .callback = SCPI_Stub,},
    { .pattern = "STUB?", .callback = SCPI_StubQ,},

    { .pattern = "SAMple", .callback = SCPI_Sample,},
    SCPI_CMD_LIST_END
};


char output_buffer[1024];
size_t output_buffer_pos = 0;

int_fast16_t err_buffer[128];
size_t err_buffer_pos = 0;

static void output_buffer_clear(void) {
    output_buffer[0] = '\0';
    output_buffer_pos = 0;
}

static size_t output_buffer_write(const char * data, size_t len) {
    memcpy(output_buffer + output_buffer_pos, data, len);
    output_buffer_pos += len;
    output_buffer[output_buffer_pos] = '\0';
    return len;
}

scpi_t scpi_context;

static void error_buffer_clear(void) {
    err_buffer[0] = 0;
    err_buffer_pos = 0;

    SCPI_RegClearBits(&scpi_context, SCPI_REG_STB, STB_QMA);
    SCPI_RegSet(&scpi_context, SCPI_REG_ESR, 0);
    SCPI_ErrorClear(&scpi_context);
}

static void error_buffer_add(int_fast16_t err) {
    err_buffer[err_buffer_pos] = err;
    err_buffer_pos++;
}

static size_t SCPI_Write(scpi_t * context, const char * data, size_t len) {
    (void) context;

    return output_buffer_write(data, len);
}

static scpi_result_t SCPI_Flush(scpi_t * context) {
    (void) context;

    return SCPI_RES_OK;
}

static int SCPI_Error(scpi_t * context, int_fast16_t err) {
    (void) context;

    error_buffer_add(err);

    return 0;
}

scpi_reg_val_t srq_val = 0;

static scpi_result_t SCPI_Control(scpi_t * context, scpi_ctrl_name_t ctrl, scpi_reg_val_t val) {
    (void) context;

    if (SCPI_CTRL_SRQ == ctrl) {
        srq_val = val;
    } else {
        fprintf(stderr, "**CTRL %02x: 0x%X (%d)\r\n", ctrl, val, val);
    }
    return SCPI_RES_OK;
}

scpi_bool_t RST_executed = FALSE;

static scpi_result_t SCPI_Reset(scpi_t * context) {
    (void) context;

    RST_executed = TRUE;
    return SCPI_RES_OK;
}

static scpi_interface_t scpi_interface = {
    .error = SCPI_Error,
    .write = SCPI_Write,
    .control = SCPI_Control,
    .flush = SCPI_Flush,
    .reset = SCPI_Reset,
};

#define SCPI_INPUT_BUFFER_LENGTH 256
static char scpi_input_buffer[SCPI_INPUT_BUFFER_LENGTH];

#define SCPI_ERROR_QUEUE_SIZE 4
static scpi_error_t scpi_error_queue_data[SCPI_ERROR_QUEUE_SIZE];

#define SCPI_ERROR_INFO_HEAP_SIZE 16
static char error_info_heap[SCPI_ERROR_INFO_HEAP_SIZE];

static int init_suite(void) {
    SCPI_Init(&scpi_context,
            scpi_commands,
            &scpi_interface,
            scpi_units_def,
            "MA", "IN", NULL, "VER",
            scpi_input_buffer, SCPI_INPUT_BUFFER_LENGTH,
            scpi_error_queue_data, SCPI_ERROR_QUEUE_SIZE);
#if USE_DEVICE_DEPENDENT_ERROR_INFORMATION && !USE_MEMORY_ALLOCATION_FREE
    SCPI_InitHeap(&scpi_context,
            error_info_heap, SCPI_ERROR_INFO_HEAP_SIZE);
#endif

    return 0;
}

static int clean_suite(void) {
    return 0;
}

static void testCommandsHandling(void) {
#define TEST_INPUT(data, output) {                              \
    SCPI_Input(&scpi_context, data, strlen(data));              \
    CU_ASSERT_STRING_EQUAL(output, output_buffer);              \
}
    output_buffer_clear();
    error_buffer_clear();

    /* Test single command */
    TEST_INPUT("*IDN?\r\n", "MA,IN,0,VER\r\n");
    output_buffer_clear();

    /* Test multiple commands in input buffer */
    TEST_INPUT("*IDN?\r\n*IDN?\r\n*IDN?\r\n*IDN?\r\n", "MA,IN,0,VER\r\nMA,IN,0,VER\r\nMA,IN,0,VER\r\nMA,IN,0,VER\r\n");
    output_buffer_clear();

    TEST_INPUT("*IDN?;*IDN?;*IDN?;*IDN?\r\n", "MA,IN,0,VER;MA,IN,0,VER;MA,IN,0,VER;MA,IN,0,VER\r\n");
    output_buffer_clear();

    TEST_INPUT("*IDN?;STUB\r\n", "MA,IN,0,VER\r\n");
    output_buffer_clear();

    TEST_INPUT("*IDN?;*OPC;*IDN?\r\n", "MA,IN,0,VER;MA,IN,0,VER\r\n");
    output_buffer_clear();

    /* Test one command in multiple buffers */
    TEST_INPUT("*IDN?", "");
    TEST_INPUT("\r\n", "MA,IN,0,VER\r\n");
    output_buffer_clear();

    /* Test empty command at the beggining */
    TEST_INPUT(";*IDN?\r\n", "MA,IN,0,VER\r\n");
    output_buffer_clear();

    TEST_INPUT(";", "");
    TEST_INPUT("*IDN?\r\n", "MA,IN,0,VER\r\n");
    output_buffer_clear();

    /* Test input "timeout" - input with length == 0 */
    TEST_INPUT("*IDN?", "");
    TEST_INPUT("", "MA,IN,0,VER\r\n");
    output_buffer_clear();

    /* Test ctree traversal */
    TEST_INPUT("TEST:TREEA?;TREEB?\r\n", "10;20\r\n");
    output_buffer_clear();

    TEST_INPUT("TEST:TREEA?;:TEXT? \"PARAM1\", \"PARAM2\"\r\n", "10;\"PARAM2\"\r\n");
    output_buffer_clear();

    /* Test special characters in parameters */
    TEST_INPUT("TEXT? \"\", \"test\r\n\"\r\n", "\"test\r\n\"\r\n");
    output_buffer_clear();

    CU_ASSERT_EQUAL(err_buffer_pos, 0);
    error_buffer_clear();
}

static void testErrorHandling(void) {
    output_buffer_clear();
    error_buffer_clear();

#define TEST_ERROR(data, output, expected_result, err_num) {    \
    output_buffer_clear();                                      \
    error_buffer_clear();                                       \
    scpi_bool_t result = SCPI_Input(&scpi_context, data, strlen(data)); \
    CU_ASSERT_STRING_EQUAL(output, output_buffer);              \
    CU_ASSERT_EQUAL(err_buffer[0], err_num);                    \
    CU_ASSERT_EQUAL(result, expected_result);                   \
}

    TEST_ERROR("*IDN?\r\n", "MA,IN,0,VER\r\n", TRUE, 0);
    TEST_ERROR("IDN?\r\n", "", FALSE, SCPI_ERROR_UNDEFINED_HEADER);
    TEST_ERROR("*ESE\r\n", "", FALSE, SCPI_ERROR_MISSING_PARAMETER);
    TEST_ERROR("*IDN? 12\r\n", "MA,IN,0,VER\r\n", FALSE, SCPI_ERROR_PARAMETER_NOT_ALLOWED);
    TEST_ERROR("TEXT? \"PARAM1\", \"PARAM2\"\r\n", "\"PARAM2\"\r\n", TRUE, 0);
    TEST_ERROR("ABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJ"
            "ABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJ"
            "ABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJ",
            "", FALSE, SCPI_ERROR_INPUT_BUFFER_OVERRUN);
    TEST_ERROR("*SRE\r\n", "", FALSE, SCPI_ERROR_MISSING_PARAMETER);



    /* TODO: SCPI_ERROR_INVALID_SEPARATOR
     * TODO: SCPI_ERROR_INVALID_SUFFIX
     * TODO: SCPI_ERROR_SUFFIX_NOT_ALLOWED
     * TODO: SCPI_ERROR_EXECUTION_ERROR
     * TODO: SCPI_ERROR_ILLEGAL_PARAMETER_VALUE
     */

    output_buffer_clear();
    error_buffer_clear();
}

static void testErrorHandlingDeviceDependent(void) {
#define TEST_CMDERR(output) {\
    SCPI_Input(&scpi_context, "SYST:ERR:NEXT?\r\n", strlen("SYST:ERR:NEXT?\r\n"));\
    CU_ASSERT_STRING_EQUAL(output, output_buffer);\
    output_buffer_clear();\
}

    output_buffer_clear();
    error_buffer_clear();

    SCPI_ErrorPushEx(&scpi_context, SCPI_ERROR_INVALID_CHARACTER, "Test1", 0);
    SCPI_ErrorPushEx(&scpi_context, SCPI_ERROR_INVALID_CHARACTER, NULL, 0);
    SCPI_ErrorPushEx(&scpi_context, SCPI_ERROR_INVALID_CHARACTER, NULL, 0);
    SCPI_ErrorPushEx(&scpi_context, SCPI_ERROR_INVALID_CHARACTER, "Test4", 0);
    SCPI_ErrorPushEx(&scpi_context, SCPI_ERROR_INVALID_CHARACTER, "Test5", 0);

#if USE_DEVICE_DEPENDENT_ERROR_INFORMATION
    TEST_CMDERR("-101,\"Invalid character;Test1\"\r\n");
#else /* USE_DEVICE_DEPENDENT_ERROR_INFORMATION */
    TEST_CMDERR("-101,\"Invalid character\"\r\n");
#endif /* USE_DEVICE_DEPENDENT_ERROR_INFORMATION */
    TEST_CMDERR("-101,\"Invalid character\"\r\n");
    TEST_CMDERR("-101,\"Invalid character\"\r\n");
    TEST_CMDERR("-350,\"Queue overflow\"\r\n");
    TEST_CMDERR("0,\"No error\"\r\n");

    SCPI_ErrorPushEx(&scpi_context, SCPI_ERROR_INVALID_CHARACTER, "Test6", 0);
    SCPI_ErrorPushEx(&scpi_context, SCPI_ERROR_INVALID_CHARACTER, "Test7", 0);
    SCPI_ErrorPushEx(&scpi_context, SCPI_ERROR_INVALID_CHARACTER, "Test8", 0);
    SCPI_ErrorPushEx(&scpi_context, SCPI_ERROR_INVALID_CHARACTER, "Test9", 0);
    SCPI_ErrorPushEx(&scpi_context, SCPI_ERROR_INVALID_CHARACTER, "Test10", 0);

#if USE_DEVICE_DEPENDENT_ERROR_INFORMATION
    TEST_CMDERR("-101,\"Invalid character;Test6\"\r\n");
    TEST_CMDERR("-101,\"Invalid character;Test7\"\r\n");
#if USE_MEMORY_ALLOCATION_FREE
    TEST_CMDERR("-101,\"Invalid character;Test8\"\r\n");
#else /* USE_MEMORY_ALLOCATION_FREE */
    TEST_CMDERR("-101,\"Invalid character\"\r\n");
#endif /* USE_MEMORY_ALLOCATION_FREE */
#else /* USE_DEVICE_DEPENDENT_ERROR_INFORMATION */
    TEST_CMDERR("-101,\"Invalid character\"\r\n");
    TEST_CMDERR("-101,\"Invalid character\"\r\n");
    TEST_CMDERR("-101,\"Invalid character\"\r\n");
#endif /* USE_DEVICE_DEPENDENT_ERROR_INFORMATION */
    TEST_CMDERR("-350,\"Queue overflow\"\r\n");
    TEST_CMDERR("0,\"No error\"\r\n");
}

static void testIEEE4882(void) {
#define TEST_IEEE4882(data, output) {                           \
    SCPI_Input(&scpi_context, data, strlen(data));              \
    CU_ASSERT_STRING_EQUAL(output, output_buffer);              \
    output_buffer_clear();                                      \
}

#define TEST_IEEE4882_REG(reg, expected) {                                     \
    CU_ASSERT_EQUAL(SCPI_RegGet(&scpi_context, (scpi_reg_name_t)(reg)), expected);\
}


#define TEST_IEEE4882_REG_SET(reg, val) {                                      \
    SCPI_RegSet(&scpi_context, reg, val);                                      \
}

    output_buffer_clear();
    error_buffer_clear();

    TEST_IEEE4882("*CLS\r\n", "");
    TEST_IEEE4882("*ESE #H20\r\n", "");
    TEST_IEEE4882("*ESE?\r\n", "32\r\n");
    TEST_IEEE4882("*ESR?\r\n", "0\r\n");
    TEST_IEEE4882("*IDN?\r\n", "MA,IN,0,VER\r\n");
    TEST_IEEE4882("*OPC\r\n", "");
    TEST_IEEE4882("*OPC?\r\n", "1\r\n");

    TEST_IEEE4882("*SRE #HFF\r\n", "");
    TEST_IEEE4882("*SRE?\r\n", "255\r\n");
    TEST_IEEE4882("*STB?\r\n", "0\r\n");
    TEST_IEEE4882("*ESR?\r\n", "1\r\n");

    srq_val = 0;
    TEST_IEEE4882("ABCD\r\n", ""); /* "Undefined header" cause command error */
    CU_ASSERT_EQUAL(srq_val, (STB_ESR | STB_SRQ | STB_QMA)); /* value of STB as service request */
    TEST_IEEE4882("*STB?\r\n", "100\r\n"); /* Event status register + Service request */
    TEST_IEEE4882("*ESR?\r\n", "32\r\n"); /* Command error */

    TEST_IEEE4882("*STB?\r\n", "68\r\n"); /* Error queue is still not empty */
    TEST_IEEE4882("*ESR?\r\n", "0\r\n");

    TEST_IEEE4882("SYST:ERR:COUNT?\r\n", "1\r\n");
#if USE_DEVICE_DEPENDENT_ERROR_INFORMATION
    TEST_IEEE4882("SYST:ERR:NEXT?\r\n", "-113,\"Undefined header;ABCD\"\r\n");
#else /* USE_DEVICE_DEPENDENT_ERROR_INFORMATION */
    TEST_IEEE4882("SYST:ERR:NEXT?\r\n", "-113,\"Undefined header\"\r\n");
#endif /* USE_DEVICE_DEPENDENT_ERROR_INFORMATION */
    TEST_IEEE4882("SYST:ERR:NEXT?\r\n", "0,\"No error\"\r\n");

    TEST_IEEE4882("*STB?\r\n", "0\r\n"); /* Error queue is now empty */

    scpi_context.interface->control = NULL;
    srq_val = 0;
    TEST_IEEE4882("ABCD\r\n", ""); /* "Undefined header" cause command error */
    CU_ASSERT_EQUAL(srq_val, 0); /* no control callback */
    TEST_IEEE4882("*STB?\r\n", "100\r\n"); /* Event status register + Service request */
    TEST_IEEE4882("*ESR?\r\n", "32\r\n"); /* Command error */
#if USE_DEVICE_DEPENDENT_ERROR_INFORMATION
    TEST_IEEE4882("SYST:ERR:NEXT?\r\n", "-113,\"Undefined header;ABCD\"\r\n");
#else /* USE_DEVICE_DEPENDENT_ERROR_INFORMATION */
    TEST_IEEE4882("SYST:ERR:NEXT?\r\n", "-113,\"Undefined header\"\r\n");
#endif /* USE_DEVICE_DEPENDENT_ERROR_INFORMATION */
    scpi_context.interface->control = SCPI_Control;

    RST_executed = FALSE;
    TEST_IEEE4882("*RST\r\n", "");
    CU_ASSERT_EQUAL(RST_executed, TRUE);

    scpi_context.interface->reset = NULL;
    RST_executed = FALSE;
    TEST_IEEE4882("*RST\r\n", "");
    CU_ASSERT_EQUAL(RST_executed, FALSE);
    scpi_context.interface->reset = SCPI_Reset;


    TEST_IEEE4882("*TST?\r\n", "0\r\n");

    TEST_IEEE4882("*WAI\r\n", "");

    TEST_IEEE4882("SYSTem:VERSion?\r\n", "1999.0\r\n");

    TEST_IEEE4882_REG_SET(SCPI_REG_QUES, 1);
    TEST_IEEE4882_REG(SCPI_REG_QUES, 1);
    TEST_IEEE4882("STATus:PRESet\r\n", "");
    TEST_IEEE4882_REG(SCPI_REG_QUES, 0);

    TEST_IEEE4882_REG_SET(SCPI_REG_QUESE, 1);
    TEST_IEEE4882("STATus:QUEStionable:ENABle?\r\n", "1\r\n");
    TEST_IEEE4882_REG(SCPI_REG_QUESE, 1);
    TEST_IEEE4882("STATus:QUEStionable:ENABle 2\r\n", "");
    TEST_IEEE4882_REG(SCPI_REG_QUESE, 2);

    TEST_IEEE4882("STATus:QUEStionable:CONDition?\r\n", "0\r\n");
    TEST_IEEE4882_REG_SET(SCPI_REG_QUESC, 1);
    TEST_IEEE4882("STATus:QUEStionable:CONDition?\r\n", "1\r\n");
    TEST_IEEE4882_REG(SCPI_REG_QUESC, 1);
    TEST_IEEE4882("STATus:QUEStionable:EVENt?\r\n", "1\r\n");
    TEST_IEEE4882_REG_SET(SCPI_REG_QUESC, 0);
    TEST_IEEE4882("STATus:QUEStionable:CONDition?\r\n", "0\r\n");
    TEST_IEEE4882("STATus:QUEStionable:EVENt?\r\n", "0\r\n");
    TEST_IEEE4882_REG_SET(SCPI_REG_QUES, 1);
    TEST_IEEE4882("STATus:QUEStionable:EVENt?\r\n", "1\r\n");
    TEST_IEEE4882_REG(SCPI_REG_QUES, 0);
    TEST_IEEE4882("STATus:QUEStionable:EVENt?\r\n", "0\r\n");

    TEST_IEEE4882_REG_SET(SCPI_REG_OPERE, 1);
    TEST_IEEE4882("STATus:OPERation:ENABle?\r\n", "1\r\n");
    TEST_IEEE4882_REG(SCPI_REG_OPERE, 1);
    TEST_IEEE4882("STATus:OPERation:ENABle 2\r\n", "");
    TEST_IEEE4882_REG(SCPI_REG_OPERE, 2);

    TEST_IEEE4882("STATus:OPERation:CONDition?\r\n", "0\r\n");
    TEST_IEEE4882_REG_SET(SCPI_REG_OPERC, 1);
    TEST_IEEE4882("STATus:OPERation:CONDition?\r\n", "1\r\n");
    TEST_IEEE4882_REG(SCPI_REG_OPERC, 1);
    TEST_IEEE4882("STATus:OPERation:EVENt?\r\n", "1\r\n");
    TEST_IEEE4882_REG_SET(SCPI_REG_OPERC, 0);
    TEST_IEEE4882("STATus:OPERation:CONDition?\r\n", "0\r\n");
    TEST_IEEE4882_REG(SCPI_REG_OPERC, 0);
    TEST_IEEE4882_REG_SET(SCPI_REG_OPER, 1);
    TEST_IEEE4882("STATus:OPERation:EVENt?\r\n", "1\r\n");
    TEST_IEEE4882_REG(SCPI_REG_OPER, 0);
    TEST_IEEE4882("STATus:OPERation:EVENt?\r\n", "0\r\n");

    TEST_IEEE4882("STUB\r\n", "");
    TEST_IEEE4882("STUB?\r\n", "0\r\n");

    TEST_IEEE4882_REG(SCPI_REG_COUNT + 1, 0);
    TEST_IEEE4882_REG_SET(SCPI_REG_OPERE, 1);
    TEST_IEEE4882_REG(SCPI_REG_OPERE, 1);
}

#define TEST_ParamInt32(data, mandatory, expected_value, expected_result, expected_error_code) \
{                                                                                       \
    int32_t value;                                                                      \
    scpi_bool_t result;                                                                 \
    scpi_error_t errCode;                                                               \
                                                                                        \
    SCPI_CoreCls(&scpi_context);                                                        \
    scpi_context.input_count = 0;                                                       \
    scpi_context.param_list.lex_state.buffer = data;                                    \
    scpi_context.param_list.lex_state.len = strlen(scpi_context.param_list.lex_state.buffer);\
    scpi_context.param_list.lex_state.pos = scpi_context.param_list.lex_state.buffer;   \
    result = SCPI_ParamInt32(&scpi_context, &value, mandatory);                         \
                                                                                        \
    SCPI_ErrorPop(&scpi_context, &errCode);                                             \
    CU_ASSERT_EQUAL(result, expected_result);                                           \
    if (expected_result) {                                                              \
        CU_ASSERT_EQUAL(value, expected_value);                                         \
    }                                                                                   \
    CU_ASSERT_EQUAL(errCode.error_code, expected_error_code);                           \
}

static void testSCPI_ParamInt32(void) {
    TEST_ParamInt32("10", TRUE, 10, TRUE, 0);
    TEST_ParamInt32("", FALSE, 0, FALSE, 0);
    TEST_ParamInt32("10.5", TRUE, 10, TRUE, 0); /* TODO: should be FALSE, -104 */
    TEST_ParamInt32("#B101010", TRUE, 42, TRUE, 0);
    TEST_ParamInt32("#H101010", TRUE, 1052688, TRUE, 0);
    TEST_ParamInt32("#Q10", TRUE, 8, TRUE, 0);

    TEST_ParamInt32("", TRUE, 0, FALSE, SCPI_ERROR_MISSING_PARAMETER); /* missing parameter */
    TEST_ParamInt32("abcd", TRUE, 0, FALSE, SCPI_ERROR_DATA_TYPE_ERROR); /* Data type error */
    TEST_ParamInt32("10.5V", TRUE, 0, FALSE, SCPI_ERROR_SUFFIX_NOT_ALLOWED);
    TEST_ParamInt32("10V", TRUE, 0, FALSE, SCPI_ERROR_SUFFIX_NOT_ALLOWED);

    /* test range */
    TEST_ParamInt32("2147483647", TRUE, 2147483647, TRUE, 0);
    TEST_ParamInt32("-2147483647", TRUE, -2147483647, TRUE, 0);
}

#define TEST_ParamUInt32(data, mandatory, expected_value, expected_result, expected_error_code) \
{                                                                                       \
    uint32_t value;                                                                     \
    scpi_bool_t result;                                                                 \
    scpi_error_t errCode;                                                               \
                                                                                        \
    SCPI_CoreCls(&scpi_context);                                                        \
    scpi_context.input_count = 0;                                                       \
    scpi_context.param_list.lex_state.buffer = data;                                    \
    scpi_context.param_list.lex_state.len = strlen(scpi_context.param_list.lex_state.buffer);\
    scpi_context.param_list.lex_state.pos = scpi_context.param_list.lex_state.buffer;   \
    result = SCPI_ParamUInt32(&scpi_context, &value, mandatory);                        \
                                                                                        \
    SCPI_ErrorPop(&scpi_context, &errCode);                                             \
    CU_ASSERT_EQUAL(result, expected_result);                                           \
    if (expected_result) {                                                              \
        CU_ASSERT_EQUAL(value, expected_value);                                         \
    }                                                                                   \
    CU_ASSERT_EQUAL(errCode.error_code, expected_error_code);                           \
}

static void testSCPI_ParamUInt32(void) {
    TEST_ParamUInt32("10", TRUE, 10, TRUE, 0);
    TEST_ParamUInt32("", FALSE, 0, FALSE, 0);
    TEST_ParamUInt32("10.5", TRUE, 10, TRUE, 0); /* TODO: should be FALSE, -104 */
    TEST_ParamUInt32("#B101010", TRUE, 42, TRUE, 0);
    TEST_ParamUInt32("#H101010", TRUE, 1052688, TRUE, 0);
    TEST_ParamUInt32("#Q10", TRUE, 8, TRUE, 0);

    TEST_ParamUInt32("", TRUE, 0, FALSE, SCPI_ERROR_MISSING_PARAMETER); /* missing parameter */
    TEST_ParamUInt32("abcd", TRUE, 0, FALSE, SCPI_ERROR_DATA_TYPE_ERROR); /* Data type error */
    TEST_ParamUInt32("10.5V", TRUE, 0, FALSE, SCPI_ERROR_SUFFIX_NOT_ALLOWED);
    TEST_ParamUInt32("10V", TRUE, 0, FALSE, SCPI_ERROR_SUFFIX_NOT_ALLOWED);

    /* test range */
    TEST_ParamUInt32("2147483647", TRUE, 2147483647ULL, TRUE, 0);
    TEST_ParamUInt32("4294967295", TRUE, 4294967295ULL, TRUE, 0);
}

#define TEST_ParamInt64(data, mandatory, expected_value, expected_result, expected_error_code) \
{                                                                                       \
    int64_t value;                                                                      \
    scpi_bool_t result;                                                                 \
    scpi_error_t errCode;                                                               \
                                                                                        \
    SCPI_CoreCls(&scpi_context);                                                        \
    scpi_context.input_count = 0;                                                       \
    scpi_context.param_list.lex_state.buffer = data;                                    \
    scpi_context.param_list.lex_state.len = strlen(scpi_context.param_list.lex_state.buffer);\
    scpi_context.param_list.lex_state.pos = scpi_context.param_list.lex_state.buffer;   \
    result = SCPI_ParamInt64(&scpi_context, &value, mandatory);                         \
                                                                                        \
    SCPI_ErrorPop(&scpi_context, &errCode);                                             \
    CU_ASSERT_EQUAL(result, expected_result);                                           \
    if (expected_result) {                                                              \
        CU_ASSERT_EQUAL(value, expected_value);                                         \
    }                                                                                   \
    CU_ASSERT_EQUAL(errCode.error_code, expected_error_code);                           \
}

static void testSCPI_ParamInt64(void) {
    TEST_ParamInt64("10", TRUE, 10, TRUE, 0);
    TEST_ParamInt64("", FALSE, 0, FALSE, 0);
    TEST_ParamInt64("10.5", TRUE, 10, TRUE, 0); /* TODO: should be FALSE, -104 */
    TEST_ParamInt64("#B101010", TRUE, 42, TRUE, 0);
    TEST_ParamInt64("#H101010", TRUE, 1052688, TRUE, 0);
    TEST_ParamInt64("#Q10", TRUE, 8, TRUE, 0);

    TEST_ParamInt64("", TRUE, 0, FALSE, SCPI_ERROR_MISSING_PARAMETER); /* missing parameter */
    TEST_ParamInt64("abcd", TRUE, 0, FALSE, SCPI_ERROR_DATA_TYPE_ERROR); /* Data type error */
    TEST_ParamInt64("10.5V", TRUE, 0, FALSE, SCPI_ERROR_SUFFIX_NOT_ALLOWED);
    TEST_ParamInt64("10V", TRUE, 0, FALSE, SCPI_ERROR_SUFFIX_NOT_ALLOWED);

    /* test range */
    TEST_ParamInt64("2147483647", TRUE, 2147483647LL, TRUE, 0);
    TEST_ParamInt64("-2147483647", TRUE, -2147483647LL, TRUE, 0);
    TEST_ParamInt64("9223372036854775807", TRUE, 9223372036854775807LL, TRUE, 0);
    TEST_ParamInt64("-9223372036854775807", TRUE, -9223372036854775807LL, TRUE, 0);
}

#define TEST_ParamUInt64(data, mandatory, expected_value, expected_result, expected_error_code) \
{                                                                                       \
    uint64_t value;                                                                     \
    scpi_bool_t result;                                                                 \
    scpi_error_t errCode;                                                               \
                                                                                        \
    SCPI_CoreCls(&scpi_context);                                                        \
    scpi_context.input_count = 0;                                                       \
    scpi_context.param_list.lex_state.buffer = data;                                    \
    scpi_context.param_list.lex_state.len = strlen(scpi_context.param_list.lex_state.buffer);\
    scpi_context.param_list.lex_state.pos = scpi_context.param_list.lex_state.buffer;   \
    result = SCPI_ParamUInt64(&scpi_context, &value, mandatory);                        \
                                                                                        \
    SCPI_ErrorPop(&scpi_context, &errCode);                                             \
    CU_ASSERT_EQUAL(result, expected_result);                                           \
    if (expected_result) {                                                              \
        CU_ASSERT_EQUAL(value, expected_value);                                         \
    }                                                                                   \
    CU_ASSERT_EQUAL(errCode.error_code, expected_error_code);                           \
}

static void testSCPI_ParamUInt64(void) {
    TEST_ParamUInt64("10", TRUE, 10, TRUE, 0);
    TEST_ParamUInt64("", FALSE, 0, FALSE, 0);
    TEST_ParamUInt64("10.5", TRUE, 10, TRUE, 0); /* TODO: should be FALSE, -104 */
    TEST_ParamUInt64("#B101010", TRUE, 42, TRUE, 0);
    TEST_ParamUInt64("#H101010", TRUE, 1052688, TRUE, 0);
    TEST_ParamUInt64("#Q10", TRUE, 8, TRUE, 0);

    TEST_ParamUInt64("", TRUE, 0, FALSE, SCPI_ERROR_MISSING_PARAMETER); /* missing parameter */
    TEST_ParamUInt64("abcd", TRUE, 0, FALSE, SCPI_ERROR_DATA_TYPE_ERROR); /* Data type error */
    TEST_ParamUInt64("10.5V", TRUE, 0, FALSE, SCPI_ERROR_SUFFIX_NOT_ALLOWED);
    TEST_ParamUInt64("10V", TRUE, 0, FALSE, SCPI_ERROR_SUFFIX_NOT_ALLOWED);

    /* test range */
    TEST_ParamUInt64("2147483647", TRUE, 2147483647ULL, TRUE, 0);
    TEST_ParamUInt64("4294967295", TRUE, 4294967295ULL, TRUE, 0);
    TEST_ParamUInt64("9223372036854775807", TRUE, 9223372036854775807ULL, TRUE, 0);
    TEST_ParamUInt64("18446744073709551615", TRUE, 18446744073709551615ULL, TRUE, 0);
}


#define TEST_ParamFloat(data, mandatory, expected_value, expected_result, expected_error_code) \
{                                                                                       \
    float value;                                                                        \
    scpi_bool_t result;                                                                 \
    scpi_error_t errCode;                                                               \
                                                                                        \
    SCPI_CoreCls(&scpi_context);                                                        \
    scpi_context.input_count = 0;                                                       \
    scpi_context.param_list.lex_state.buffer = data;                                    \
    scpi_context.param_list.lex_state.len = strlen(scpi_context.param_list.lex_state.buffer);\
    scpi_context.param_list.lex_state.pos = scpi_context.param_list.lex_state.buffer;   \
    result = SCPI_ParamFloat(&scpi_context, &value, mandatory);                         \
                                                                                        \
    SCPI_ErrorPop(&scpi_context, &errCode);                                             \
    CU_ASSERT_EQUAL(result, expected_result);                                           \
    if (expected_result) {                                                              \
        CU_ASSERT_DOUBLE_EQUAL(value, expected_value, 0.000001);                        \
    }                                                                                   \
    CU_ASSERT_EQUAL(errCode.error_code, expected_error_code);                           \
}

static void testSCPI_ParamFloat(void) {
    TEST_ParamFloat("10", TRUE, 10, TRUE, 0);
    TEST_ParamFloat("", FALSE, 0, FALSE, 0);
    TEST_ParamFloat("10.5", TRUE, 10.5, TRUE, 0);
    TEST_ParamFloat("#B101010", TRUE, 42, TRUE, 0);
    TEST_ParamFloat("#H101010", TRUE, 1052688, TRUE, 0);
    TEST_ParamFloat("#Q10", TRUE, 8, TRUE, 0);
    TEST_ParamFloat("#HFFFFFF", TRUE, 0xFFFFFFu, TRUE, 0);

    TEST_ParamFloat("", TRUE, 0, FALSE, SCPI_ERROR_MISSING_PARAMETER); /* missing parameter */
    TEST_ParamFloat("abcd", TRUE, 0, FALSE, SCPI_ERROR_DATA_TYPE_ERROR); /* Data type error */
    TEST_ParamFloat("10.5V", TRUE, 0, FALSE, SCPI_ERROR_SUFFIX_NOT_ALLOWED);
    TEST_ParamFloat("10V", TRUE, 0, FALSE, SCPI_ERROR_SUFFIX_NOT_ALLOWED);
}

#define TEST_ParamDouble(data, mandatory, expected_value, expected_result, expected_error_code) \
{                                                                                       \
    double value;                                                                       \
    scpi_bool_t result;                                                                 \
    scpi_error_t errCode;                                                               \
                                                                                        \
    SCPI_CoreCls(&scpi_context);                                                        \
    scpi_context.input_count = 0;                                                       \
    scpi_context.param_list.lex_state.buffer = data;                                    \
    scpi_context.param_list.lex_state.len = strlen(scpi_context.param_list.lex_state.buffer);\
    scpi_context.param_list.lex_state.pos = scpi_context.param_list.lex_state.buffer;   \
    result = SCPI_ParamDouble(&scpi_context, &value, mandatory);                        \
                                                                                        \
    SCPI_ErrorPop(&scpi_context, &errCode);                                             \
    CU_ASSERT_EQUAL(result, expected_result);                                           \
    if (expected_result) {                                                              \
        CU_ASSERT_DOUBLE_EQUAL(value, expected_value, 0.000001);                        \
    }                                                                                   \
    CU_ASSERT_EQUAL(errCode.error_code, expected_error_code);                           \
}

static void testSCPI_ParamDouble(void) {
    TEST_ParamDouble("10", TRUE, 10, TRUE, 0);
    TEST_ParamDouble("", FALSE, 0, FALSE, 0);
    TEST_ParamDouble("10.5", TRUE, 10.5, TRUE, 0);
    TEST_ParamDouble("#B101010", TRUE, 42, TRUE, 0);
    TEST_ParamDouble("#H101010", TRUE, 1052688, TRUE, 0);
    TEST_ParamDouble("#Q10", TRUE, 8, TRUE, 0);
    TEST_ParamDouble("#HFFFFFFFF", TRUE, 0xFFFFFFFFu, TRUE, 0);

    TEST_ParamDouble("", TRUE, 0, FALSE, SCPI_ERROR_MISSING_PARAMETER); /* missing parameter */
    TEST_ParamDouble("abcd", TRUE, 0, FALSE, SCPI_ERROR_DATA_TYPE_ERROR); /* Data type error */
    TEST_ParamDouble("10.5V", TRUE, 0, FALSE, SCPI_ERROR_SUFFIX_NOT_ALLOWED);
    TEST_ParamDouble("10V", TRUE, 0, FALSE, SCPI_ERROR_SUFFIX_NOT_ALLOWED);
}

#define TEST_ParamCharacters(data, mandatory, expected_value, expected_result, expected_error_code) \
{                                                                                       \
    const char * value;                                                                 \
    size_t value_len;                                                                   \
    scpi_bool_t result;                                                                 \
    scpi_error_t errCode;                                                               \
                                                                                        \
    SCPI_CoreCls(&scpi_context);                                                        \
    scpi_context.input_count = 0;                                                       \
    scpi_context.param_list.lex_state.buffer = data;                                    \
    scpi_context.param_list.lex_state.len = strlen(scpi_context.param_list.lex_state.buffer);\
    scpi_context.param_list.lex_state.pos = scpi_context.param_list.lex_state.buffer;   \
    result = SCPI_ParamCharacters(&scpi_context, &value, &value_len, mandatory);        \
    /*printf("%.*s\r\n",  (int)value_len, value);*/                                     \
    SCPI_ErrorPop(&scpi_context, &errCode);                                             \
    CU_ASSERT_EQUAL(result, expected_result);                                           \
    if (expected_result) {                                                              \
        CU_ASSERT_NSTRING_EQUAL(value, expected_value, value_len);                      \
    }                                                                                   \
    CU_ASSERT_EQUAL(errCode.error_code, expected_error_code);                           \
}

static void testSCPI_ParamCharacters(void) {
    TEST_ParamCharacters("10", TRUE, "10", TRUE, 0);
    TEST_ParamCharacters(" ABCD", TRUE, "ABCD", TRUE, 0); /* TokProgramMnemonic */
    TEST_ParamCharacters("\"ABCD\"", TRUE, "ABCD", TRUE, 0); /* TokDoubleQuoteProgramData */
    TEST_ParamCharacters("\'ABCD\'", TRUE, "ABCD", TRUE, 0); /* TokSingleQuoteProgramData */
    TEST_ParamCharacters("#204ABCD", TRUE, "ABCD", TRUE, 0); /* TokArbitraryBlockProgramData */
    TEST_ParamCharacters("#210ABCD", TRUE, "", FALSE, SCPI_ERROR_INVALID_STRING_DATA); /* invalid Block Data */
}


#define TEST_ParamCopyText(data, mandatory, expected_value, expected_len, expected_result, expected_error_code) \
{                                                                                       \
    char value[100];                                                                    \
    size_t value_len;                                                                   \
    scpi_bool_t result;                                                                 \
    scpi_error_t errCode;                                                               \
                                                                                        \
    SCPI_CoreCls(&scpi_context);                                                        \
    scpi_context.input_count = 0;                                                       \
    scpi_context.param_list.lex_state.buffer = data;                                    \
    scpi_context.param_list.lex_state.len = strlen(scpi_context.param_list.lex_state.buffer);\
    scpi_context.param_list.lex_state.pos = scpi_context.param_list.lex_state.buffer;   \
    result = SCPI_ParamCopyText(&scpi_context, value, sizeof(value), &value_len, mandatory);\
    /*printf("%.*s\r\n",  (int)value_len, value);*/                                     \
    SCPI_ErrorPop(&scpi_context, &errCode);                                             \
    CU_ASSERT_EQUAL(result, expected_result);                                           \
    if (expected_result) {                                                              \
        CU_ASSERT_STRING_EQUAL(value, expected_value);                                  \
        CU_ASSERT_EQUAL(value_len, expected_len);                                       \
    }                                                                                   \
    CU_ASSERT_EQUAL(errCode.error_code, expected_error_code);                           \
}

static void testSCPI_ParamCopyText(void) {
    TEST_ParamCopyText("\'abc\'", TRUE, "abc", 3, TRUE, 0);
    TEST_ParamCopyText("\"abc\"", TRUE, "abc", 3, TRUE, 0);
    TEST_ParamCopyText("\'a\'", TRUE, "a", 1, TRUE, 0);
    TEST_ParamCopyText("\'a\'\'c\'", TRUE, "a\'c", 3, TRUE, 0);
    TEST_ParamCopyText("\'a\"c\'", TRUE, "a\"c", 3, TRUE, 0);
    TEST_ParamCopyText("\"a\"\"c\"", TRUE, "a\"c", 3, TRUE, 0);
    TEST_ParamCopyText("\"a\'c\"", TRUE, "a\'c", 3, TRUE, 0);
    TEST_ParamCopyText("\"\"", TRUE, "", 0, TRUE, 0);
    TEST_ParamCopyText("", FALSE, "", 0, FALSE, 0);
    TEST_ParamCopyText("\"\"", FALSE, "", 0, TRUE, 0);
}

#define TEST_ParamArbitraryBlock(data, mandatory, expected_value, expected_result, expected_error_code) \
{                                                                                       \
    const char * value;                                                                 \
    size_t value_len;                                                                   \
    scpi_bool_t result;                                                                 \
    scpi_error_t errCode;                                                               \
                                                                                        \
    SCPI_CoreCls(&scpi_context);                                                        \
    scpi_context.input_count = 0;                                                       \
    scpi_context.param_list.lex_state.buffer = data;                                    \
    scpi_context.param_list.lex_state.len = strlen(scpi_context.param_list.lex_state.buffer);\
    scpi_context.param_list.lex_state.pos = scpi_context.param_list.lex_state.buffer;   \
    result = SCPI_ParamArbitraryBlock(&scpi_context, &value, &value_len, mandatory);    \
    /*printf("%.*s\r\n",  (int)value_len, value);*/                                     \
    SCPI_ErrorPop(&scpi_context, &errCode);                                             \
    CU_ASSERT_EQUAL(result, expected_result);                                           \
    if (expected_result) {                                                              \
        CU_ASSERT_NSTRING_EQUAL(value, expected_value, value_len);                      \
    }                                                                                   \
    CU_ASSERT_EQUAL(errCode.error_code, expected_error_code);                           \
}
static void testSCPI_ParamArbitraryBlock(void) {
    TEST_ParamArbitraryBlock("#204ABCD", TRUE, "ABCD", TRUE, 0);
    TEST_ParamArbitraryBlock("\"ABCD\"", TRUE, "", FALSE, SCPI_ERROR_DATA_TYPE_ERROR);
}

#define TEST_ParamBool(data, mandatory, expected_value, expected_result, expected_error_code) \
{                                                                                       \
    scpi_bool_t value;                                                                  \
    scpi_bool_t result;                                                                 \
    scpi_error_t errCode;                                                               \
                                                                                        \
    SCPI_CoreCls(&scpi_context);                                                        \
    scpi_context.input_count = 0;                                                       \
    scpi_context.param_list.lex_state.buffer = data;                                    \
    scpi_context.param_list.lex_state.len = strlen(scpi_context.param_list.lex_state.buffer);\
    scpi_context.param_list.lex_state.pos = scpi_context.param_list.lex_state.buffer;   \
    result = SCPI_ParamBool(&scpi_context, &value, mandatory);                          \
                                                                                        \
    SCPI_ErrorPop(&scpi_context, &errCode);                                             \
    CU_ASSERT_EQUAL(result, expected_result);                                           \
    if (expected_result) {                                                              \
        CU_ASSERT_EQUAL(value, expected_value);                                         \
    }                                                                                   \
    CU_ASSERT_EQUAL(errCode.error_code, expected_error_code);                           \
}
static void testSCPI_ParamBool(void) {
    TEST_ParamBool("ON", TRUE, TRUE, TRUE, 0);
    TEST_ParamBool("1", TRUE, TRUE, TRUE, 0);
    TEST_ParamBool("100", TRUE, TRUE, TRUE, 0);
    TEST_ParamBool("OFF", TRUE, FALSE, TRUE, 0);
    TEST_ParamBool("0", TRUE, FALSE, TRUE, 0);
    TEST_ParamBool("XYZ", TRUE, FALSE, FALSE, SCPI_ERROR_ILLEGAL_PARAMETER_VALUE);
}

#define TEST_ParamChoice(data, mandatory, expected_value, expected_result, expected_error_code) \
{                                                                                       \
    int32_t value;                                                                      \
    scpi_bool_t result;                                                                 \
    scpi_error_t errCode;                                                               \
                                                                                        \
    SCPI_CoreCls(&scpi_context);                                                        \
    scpi_context.input_count = 0;                                                       \
    scpi_context.param_list.lex_state.buffer = data;                                    \
    scpi_context.param_list.lex_state.len = strlen(scpi_context.param_list.lex_state.buffer);\
    scpi_context.param_list.lex_state.pos = scpi_context.param_list.lex_state.buffer;   \
    result = SCPI_ParamChoice(&scpi_context, test_options, &value, mandatory);          \
                                                                                        \
    SCPI_ErrorPop(&scpi_context, &errCode);                                             \
    CU_ASSERT_EQUAL(result, expected_result);                                           \
    if (expected_result) {                                                              \
        CU_ASSERT_EQUAL(value, expected_value);                                         \
    }                                                                                   \
    CU_ASSERT_EQUAL(errCode.error_code, expected_error_code);                           \
}
static void testSCPI_ParamChoice(void) {
    scpi_choice_def_t test_options[] = {
        {"OPTIONA", 1},
        {"OPTIONB", 2},
        {"SOURce", 3},
        SCPI_CHOICE_LIST_END /* termination of option list */
    };

    TEST_ParamChoice("ON", TRUE, 0, FALSE, SCPI_ERROR_ILLEGAL_PARAMETER_VALUE);
    TEST_ParamChoice("OPTIONA", TRUE, 1, TRUE, 0);
    TEST_ParamChoice("OPTIONB", TRUE, 2, TRUE, 0);
    TEST_ParamChoice("SOURCE", TRUE, 3, TRUE, 0);
    TEST_ParamChoice("SOUR", TRUE, 3, TRUE, 0);
}

#define TEST_NumericListInt(data, index, expected_range, expected_from, expected_to, expected_result, expected_error_code) \
{                                                                                       \
    scpi_bool_t result;                                                                 \
    scpi_expr_result_t result2;                                                         \
    scpi_error_t errCode;                                                               \
    scpi_parameter_t param;                                                             \
    int32_t val_from, val_to;                                                           \
    scpi_bool_t val_range;                                                              \
                                                                                        \
    SCPI_CoreCls(&scpi_context);                                                        \
    scpi_context.input_count = 0;                                                       \
    scpi_context.param_list.lex_state.buffer = data;                                    \
    scpi_context.param_list.lex_state.len = strlen(scpi_context.param_list.lex_state.buffer);\
    scpi_context.param_list.lex_state.pos = scpi_context.param_list.lex_state.buffer;   \
    result = SCPI_Parameter(&scpi_context, &param, TRUE);                               \
    result2 = SCPI_ExprNumericListEntryInt(&scpi_context, &param, index, &val_range, &val_from, &val_to);\
    SCPI_ErrorPop(&scpi_context, &errCode);                                             \
    CU_ASSERT_EQUAL(result2, expected_result);                                          \
    if (expected_result == SCPI_EXPR_OK) {                                              \
        CU_ASSERT_EQUAL(val_range, expected_range);                                     \
        CU_ASSERT_EQUAL(val_from, expected_from);                                       \
        if (expected_range) {                                                           \
            CU_ASSERT_EQUAL(val_to, expected_to);                                       \
        }                                                                               \
    }                                                                                   \
    CU_ASSERT_EQUAL(errCode.error_code, expected_error_code);                           \
}

#define TEST_NumericListDouble(data, index, expected_range, expected_from, expected_to, expected_result, expected_error_code) \
{                                                                                       \
    scpi_bool_t result;                                                                 \
    scpi_expr_result_t result2;                                                         \
    scpi_error_t errCode;                                                               \
    scpi_parameter_t param;                                                             \
    double val_from, val_to;                                                            \
    scpi_bool_t val_range;                                                              \
                                                                                        \
    SCPI_CoreCls(&scpi_context);                                                        \
    scpi_context.input_count = 0;                                                       \
    scpi_context.param_list.lex_state.buffer = data;                                    \
    scpi_context.param_list.lex_state.len = strlen(scpi_context.param_list.lex_state.buffer);\
    scpi_context.param_list.lex_state.pos = scpi_context.param_list.lex_state.buffer;   \
    result = SCPI_Parameter(&scpi_context, &param, TRUE);                               \
    result2 = SCPI_ExprNumericListEntryDouble(&scpi_context, &param, index, &val_range, &val_from, &val_to);\
    SCPI_ErrorPop(&scpi_context, &errCode);                                             \
    CU_ASSERT_EQUAL(result2, expected_result);                                          \
    if (expected_result == SCPI_EXPR_OK) {                                              \
        CU_ASSERT_EQUAL(val_range, expected_range);                                     \
        CU_ASSERT_DOUBLE_EQUAL(val_from, expected_from, 0.0001);                        \
        if (expected_range) {                                                           \
            CU_ASSERT_DOUBLE_EQUAL(val_to, expected_to, 0.0001);                        \
        }                                                                               \
    }                                                                                   \
    CU_ASSERT_EQUAL(errCode.error_code, expected_error_code);                           \
}

static void testNumericList(void) {
    TEST_NumericListInt("(1:2,5:6)", 0, TRUE, 1, 2, SCPI_EXPR_OK, 0);
    TEST_NumericListInt("(1:2,5:6)", 1, TRUE, 5, 6, SCPI_EXPR_OK, 0);
    TEST_NumericListInt("(1:2,5:6)", 2, FALSE, 0, 0, SCPI_EXPR_NO_MORE, 0);

    TEST_NumericListInt("(12,5:6)", 0, FALSE, 12, 0, SCPI_EXPR_OK, 0);
    TEST_NumericListInt("(12,5:6)", 1, TRUE, 5, 6, SCPI_EXPR_OK, 0);
    TEST_NumericListInt("(12,5:6)", 2, FALSE, 0, 0, SCPI_EXPR_NO_MORE, 0);

    TEST_NumericListInt("(12,5:6:3)", 0, FALSE, 12, 0, SCPI_EXPR_OK, 0);
    TEST_NumericListInt("(12,5:6:3)", 1, TRUE, 5, 6, SCPI_EXPR_OK, 0);
    TEST_NumericListInt("(12,5:6:3)", 2, FALSE, 0, 0, SCPI_EXPR_ERROR, SCPI_ERROR_EXPRESSION_PARSING_ERROR);

    TEST_NumericListDouble("(1:2,5:6)", 0, TRUE, 1, 2, SCPI_EXPR_OK, 0);
    TEST_NumericListDouble("(1:2,5:6)", 1, TRUE, 5, 6, SCPI_EXPR_OK, 0);
    TEST_NumericListDouble("(1:2,5:6)", 2, FALSE, 0, 0, SCPI_EXPR_NO_MORE, 0);

    TEST_NumericListDouble("(12,5:6)", 0, FALSE, 12, 0, SCPI_EXPR_OK, 0);
    TEST_NumericListDouble("(12,5:6)", 1, TRUE, 5, 6, SCPI_EXPR_OK, 0);
    TEST_NumericListDouble("(12,5:6)", 2, FALSE, 0, 0, SCPI_EXPR_NO_MORE, 0);

    TEST_NumericListDouble("(12,5:6:3)", 0, FALSE, 12, 0, SCPI_EXPR_OK, 0);
    TEST_NumericListDouble("(12,5:6:3)", 1, TRUE, 5, 6, SCPI_EXPR_OK, 0);
    TEST_NumericListDouble("(12,5:6:3)", 2, FALSE, 0, 0, SCPI_EXPR_ERROR, SCPI_ERROR_EXPRESSION_PARSING_ERROR);
    TEST_NumericListDouble("(12,5:)", 2, FALSE, 0, 0, SCPI_EXPR_ERROR, SCPI_ERROR_EXPRESSION_PARSING_ERROR);
    TEST_NumericListDouble("aaaa", 2, FALSE, 0, 0, SCPI_EXPR_ERROR, SCPI_ERROR_DATA_TYPE_ERROR);
}

#define NOPAREN(...) __VA_ARGS__

#define TEST_ChannelList(data, index, val_len, expected_range, expected_dimensions, _expected_from, _expected_to, expected_result, expected_error_code) \
{                                                                                       \
    scpi_bool_t result;                                                                 \
    scpi_expr_result_t result2;                                                         \
    scpi_error_t errCode;                                                               \
    scpi_parameter_t param;                                                             \
    int32_t val_from[val_len], val_to[val_len];                                         \
    scpi_bool_t val_range;                                                              \
    int32_t expected_from[] = {NOPAREN _expected_from};                                 \
    int32_t expected_to[] = {NOPAREN _expected_to};                                     \
    size_t val_dimensions;                                                              \
                                                                                        \
    SCPI_CoreCls(&scpi_context);                                                        \
    scpi_context.input_count = 0;                                                       \
    scpi_context.param_list.lex_state.buffer = data;                                    \
    scpi_context.param_list.lex_state.len = strlen(scpi_context.param_list.lex_state.buffer);\
    scpi_context.param_list.lex_state.pos = scpi_context.param_list.lex_state.buffer;   \
    result = SCPI_Parameter(&scpi_context, &param, TRUE);                               \
    result2 = SCPI_ExprChannelListEntry(&scpi_context, &param, index, &val_range, val_from, val_to, val_len, &val_dimensions);\
    SCPI_ErrorPop(&scpi_context, &errCode);                                             \
    CU_ASSERT_EQUAL(result2, expected_result);                                          \
    if (expected_result == SCPI_EXPR_OK) {                                              \
        CU_ASSERT_EQUAL(val_dimensions, expected_dimensions);                           \
        CU_ASSERT_EQUAL(val_range, expected_range);                                     \
        { size_t i; for(i = 0; (i < val_len) && (i < val_dimensions); i++) {            \
            CU_ASSERT_EQUAL(val_from[i], expected_from[i]);                             \
        }}                                                                              \
        if (expected_range) {                                                           \
            { size_t i; for(i = 0; (i < val_len) && (i < val_dimensions); i++) {        \
                CU_ASSERT_EQUAL(val_to[i], expected_to[i]);                             \
            }}                                                                          \
        }                                                                               \
    }                                                                                   \
    CU_ASSERT_EQUAL(errCode.error_code, expected_error_code);                           \
}

static void testChannelList(void) {
    TEST_ChannelList("(1)", 0, 1, FALSE, 0, (0), (0), SCPI_EXPR_ERROR, SCPI_ERROR_EXPRESSION_PARSING_ERROR);

    TEST_ChannelList("(@1)", 0, 1, FALSE, 1, (1), (0), SCPI_EXPR_OK, 0);
    TEST_ChannelList("(@1)", 1, 1, FALSE, 0, (0), (0), SCPI_EXPR_NO_MORE, 0);

    TEST_ChannelList("(@1,2)", 0, 1, FALSE, 1, (1), (0), SCPI_EXPR_OK, 0);
    TEST_ChannelList("(@1,2)", 1, 1, FALSE, 1, (2), (0), SCPI_EXPR_OK, 0);
    TEST_ChannelList("(@1,2)", 2, 1, FALSE, 0, (0), (0), SCPI_EXPR_NO_MORE, 0);

    TEST_ChannelList("(@1,2:3)", 0, 1, FALSE, 1, (1), (0), SCPI_EXPR_OK, 0);
    TEST_ChannelList("(@1,2:3)", 1, 1, TRUE, 1, (2), (3), SCPI_EXPR_OK, 0);
    TEST_ChannelList("(@1,2:3)", 2, 1, FALSE, 0, (0), (0), SCPI_EXPR_NO_MORE, 0);

    TEST_ChannelList("(@1,2!5:3!6)", 0, 2, FALSE, 1, (1), (0), SCPI_EXPR_OK, 0);
    TEST_ChannelList("(@1,2!5:3!6)", 1, 2, TRUE, 2, (2, 5), (3, 6), SCPI_EXPR_OK, 0);
    TEST_ChannelList("(@1,2!5:3!6)", 2, 2, FALSE, 0, (0), (0), SCPI_EXPR_NO_MORE, 0);

    TEST_ChannelList("(@1,2!5:3!6)", 0, 1, FALSE, 1, (1), (0), SCPI_EXPR_OK, 0);
    TEST_ChannelList("(@1,2!5:3!6)", 1, 1, TRUE, 2, (2), (3), SCPI_EXPR_OK, 0);
    TEST_ChannelList("(@1,2!5:3!6)", 2, 1, FALSE, 0, (0), (0), SCPI_EXPR_NO_MORE, 0);

    TEST_ChannelList("(@1,2!5:3!6!7)", 0, 2, FALSE, 1, (1), (0), SCPI_EXPR_OK, 0);
    TEST_ChannelList("(@1,2!5:3!6!7)", 1, 2, FALSE, 0, (0), (0), SCPI_EXPR_ERROR, SCPI_ERROR_EXPRESSION_PARSING_ERROR);
    TEST_ChannelList("(@1,2!5:3!6!7)", 2, 2, FALSE, 0, (0), (0), SCPI_EXPR_ERROR, SCPI_ERROR_EXPRESSION_PARSING_ERROR);

    TEST_ChannelList("(@1, 2)", 0, 1, FALSE, 1, (1), (0), SCPI_EXPR_OK, 0);
    TEST_ChannelList("(@1, 2)", 1, 1, FALSE, 0, (0), (0), SCPI_EXPR_ERROR, SCPI_ERROR_EXPRESSION_PARSING_ERROR);

    TEST_ChannelList("(@1,)", 1, 1, FALSE, 0, (0), (0), SCPI_EXPR_ERROR, SCPI_ERROR_EXPRESSION_PARSING_ERROR);
    TEST_ChannelList("(@1,2:)", 1, 1, FALSE, 0, (0), (0), SCPI_EXPR_ERROR, SCPI_ERROR_EXPRESSION_PARSING_ERROR);
    TEST_ChannelList("abcd", 1, 1, FALSE, 0, (0), (0), SCPI_EXPR_ERROR, SCPI_ERROR_DATA_TYPE_ERROR);
}


#define TEST_ParamNumber(data, mandatory, expected_special, expected_tag, expected_value, expected_unit, expected_base, expected_result, expected_error_code) \
{                                                                                       \
    scpi_number_t value;                                                                \
    scpi_bool_t result;                                                                 \
    scpi_error_t errCode;                                                               \
                                                                                        \
    SCPI_CoreCls(&scpi_context);                                                        \
    scpi_context.input_count = 0;                                                       \
    scpi_context.param_list.lex_state.buffer = data;                                    \
    scpi_context.param_list.lex_state.len = strlen(scpi_context.param_list.lex_state.buffer);\
    scpi_context.param_list.lex_state.pos = scpi_context.param_list.lex_state.buffer;   \
    result = SCPI_ParamNumber(&scpi_context, scpi_special_numbers_def, &value, mandatory);\
                                                                                        \
    SCPI_ErrorPop(&scpi_context, &errCode);                                             \
    CU_ASSERT_EQUAL(result, expected_result);                                           \
    if (expected_result) {                                                              \
        CU_ASSERT_EQUAL(value.special, expected_special);                               \
        if (value.special) CU_ASSERT_EQUAL(value.content.tag, expected_tag);                    \
        if (!value.special) CU_ASSERT_DOUBLE_EQUAL(value.content.value, expected_value, 0.000001);\
        CU_ASSERT_EQUAL(value.unit, expected_unit);                                     \
        CU_ASSERT_EQUAL(value.base, expected_base);                                     \
    }                                                                                   \
    CU_ASSERT_EQUAL(errCode.error_code, expected_error_code);                           \
}

static void testParamNumber(void) {
    TEST_ParamNumber("1", TRUE, FALSE, SCPI_NUM_NUMBER, 1, SCPI_UNIT_NONE, 10, TRUE, 0);
    TEST_ParamNumber("#Q20", TRUE, FALSE, SCPI_NUM_NUMBER, 16, SCPI_UNIT_NONE, 8, TRUE, 0);
    TEST_ParamNumber("#H20", TRUE, FALSE, SCPI_NUM_NUMBER, 32, SCPI_UNIT_NONE, 16, TRUE, 0);
    TEST_ParamNumber("#B11", TRUE, FALSE, SCPI_NUM_NUMBER, 3, SCPI_UNIT_NONE, 2, TRUE, 0);
    TEST_ParamNumber("1.2", TRUE, FALSE, SCPI_NUM_NUMBER, 1.2, SCPI_UNIT_NONE, 10, TRUE, 0);
    TEST_ParamNumber("1.2e-1", TRUE, FALSE, SCPI_NUM_NUMBER, 0.12, SCPI_UNIT_NONE, 10, TRUE, 0);
    TEST_ParamNumber("1.2e-1V", TRUE, FALSE, SCPI_NUM_NUMBER, 0.12, SCPI_UNIT_VOLT, 10, TRUE, 0);
    TEST_ParamNumber("1.2mV", TRUE, FALSE, SCPI_NUM_NUMBER, 0.0012, SCPI_UNIT_VOLT, 10, TRUE, 0);
    TEST_ParamNumber("100 OHM", TRUE, FALSE, SCPI_NUM_NUMBER, 100, SCPI_UNIT_OHM, 10, TRUE, 0);
    TEST_ParamNumber("min", TRUE, TRUE, SCPI_NUM_MIN, 0, SCPI_UNIT_NONE, 10, TRUE, 0);
    TEST_ParamNumber("infinity", TRUE, TRUE, SCPI_NUM_INF, 0, SCPI_UNIT_NONE, 10, TRUE, 0);
    TEST_ParamNumber("minc", TRUE, TRUE, SCPI_NUM_NUMBER, 0, SCPI_UNIT_NONE, 10, FALSE, SCPI_ERROR_ILLEGAL_PARAMETER_VALUE);
    TEST_ParamNumber("100 xyz", TRUE, FALSE, SCPI_NUM_NUMBER, 100, SCPI_UNIT_NONE, 10, FALSE, SCPI_ERROR_INVALID_SUFFIX);
}

#define TEST_Result(func, value, expected_result) \
{\
    output_buffer_clear();\
    scpi_context.output_count = 0;\
    size_t expected_len = strlen(expected_result);\
    size_t len = SCPI_Result##func(&scpi_context, (value));\
    CU_ASSERT_EQUAL(len, expected_len);\
    CU_ASSERT_EQUAL(output_buffer_pos, expected_len);\
    CU_ASSERT_EQUAL(memcmp(output_buffer, expected_result, expected_len), 0);\
}

#define TEST_ResultBase(func, value, base, expected_result) \
{\
    output_buffer_clear();\
    scpi_context.output_count = 0;\
    size_t expected_len = strlen(expected_result);\
    size_t len = SCPI_Result##func##Base(&scpi_context, (value), (base));\
    CU_ASSERT_EQUAL(len, expected_len);\
    CU_ASSERT_EQUAL(output_buffer_pos, expected_len);\
    CU_ASSERT_EQUAL(memcmp(output_buffer, expected_result, expected_len), 0);\
}

static void testResultInt8(void) {
    TEST_Result(Int8, 10, "10");
    TEST_Result(Int8, -10, "-10");
    TEST_Result(Int8, 100, "100");
    TEST_Result(Int8, -100, "-100");
    TEST_Result(Int8, 256, "0");
    TEST_Result(Int8, 1111, "87");
    TEST_Result(Int8, 127, "127");
    TEST_Result(Int8, -128, "-128");
}

static void testResultUInt8(void) {
    TEST_Result(UInt8, 10, "10");
    TEST_Result(UInt8, -10, "246");
    TEST_Result(UInt8, 100, "100");
    TEST_Result(UInt8, -100, "156");
    TEST_Result(UInt8, 256, "0");
    TEST_Result(UInt8, 1111, "87");
    TEST_Result(UInt8, 127, "127");
    TEST_Result(UInt8, -128, "128");
    TEST_Result(UInt8, 0, "0");
    TEST_Result(UInt8, 255, "255");

    TEST_ResultBase(UInt8, 0xff, 16, "#HFF");
    TEST_ResultBase(UInt8, 0xff, 8, "#Q377");
    TEST_ResultBase(UInt8, 0xff, 2, "#B11111111");
}

static void testResultInt16(void) {
    TEST_Result(Int16, 10, "10");
    TEST_Result(Int16, -10, "-10");
    TEST_Result(Int16, 100, "100");
    TEST_Result(Int16, -100, "-100");
    TEST_Result(Int16, 256, "256");
    TEST_Result(Int16, 1111, "1111");
    TEST_Result(Int16, 127, "127");
    TEST_Result(Int16, -128, "-128");
    TEST_Result(Int16, 111111, "-19961");
    TEST_Result(Int16, 32767, "32767");
    TEST_Result(Int16, -32768, "-32768");
}

static void testResultUInt16(void) {
    TEST_Result(UInt16, 10, "10");
    TEST_Result(UInt16, -10, "65526");
    TEST_Result(UInt16, 100, "100");
    TEST_Result(UInt16, -100, "65436");
    TEST_Result(UInt16, 256, "256");
    TEST_Result(UInt16, 1111, "1111");
    TEST_Result(UInt16, 127, "127");
    TEST_Result(UInt16, -128, "65408");
    TEST_Result(UInt16, 111111, "45575");
    TEST_Result(UInt16, 32767, "32767");
    TEST_Result(UInt16, -32768, "32768");
    TEST_Result(UInt16, 65535, "65535");

    TEST_ResultBase(UInt16, 0xffff, 16, "#HFFFF");
    TEST_ResultBase(UInt16, 0xffff, 8, "#Q177777");
    TEST_ResultBase(UInt16, 0xffff, 2, "#B1111111111111111");
}

static void testResultInt32(void) {
    TEST_Result(Int32, 10, "10");
    TEST_Result(Int32, -10, "-10");
    TEST_Result(Int32, 2147483647L, "2147483647");
    /* TEST_Result(Int32, -2147483648L, "-2147483648"); bug in GCC */
    TEST_Result(Int32, -2147483647L, "-2147483647");
}

static void testResultUInt32(void) {
    TEST_Result(UInt32, 10, "10");
    TEST_Result(UInt32, -10, "4294967286");
    TEST_Result(UInt32, 2147483647L, "2147483647");
    /* TEST_Result(UInt32, -2147483648L, "2147483648"); bug in GCC */
    TEST_Result(UInt32, -2147483647L, "2147483649");
    TEST_Result(UInt32, 4294967295UL, "4294967295");

    TEST_ResultBase(UInt32, 0xffffffff, 16, "#HFFFFFFFF");
    TEST_ResultBase(UInt32, 0xffffffff, 8, "#Q37777777777");
    TEST_ResultBase(UInt32, 0xffffffff, 2, "#B11111111111111111111111111111111");
}

static void testResultInt64(void) {
    TEST_Result(Int64, 10, "10");
    TEST_Result(Int64, -10, "-10");
    TEST_Result(Int64, 127, "127");
    TEST_Result(Int64, -128, "-128");
    TEST_Result(Int64, 32767, "32767");
    TEST_Result(Int64, -32768, "-32768");
    TEST_Result(Int64, 2147483647L, "2147483647");
    /* TEST_Result(Int64, -2147483648, "-2147483648"); bug in gcc */
    TEST_Result(Int64, -2147483647L, "-2147483647");
    TEST_Result(Int64, 9223372036854775807LL, "9223372036854775807");
    /* TEST_Result(Int64, -9223372036854775808LL, "-9223372036854775808"); bug in GCC */
    TEST_Result(Int64, -9223372036854775807LL, "-9223372036854775807");
}

static void testResultUInt64(void) {
    TEST_Result(UInt64, 10, "10");
    TEST_Result(UInt64, -10, "18446744073709551606");
    TEST_Result(UInt64, 127, "127");
    TEST_Result(UInt64, -128, "18446744073709551488");
    TEST_Result(UInt64, 32767, "32767");
    TEST_Result(UInt64, -32768, "18446744073709518848");
    TEST_Result(UInt64, 2147483647L, "2147483647");
    /* TEST_Result(UInt64, -2147483648L, "18446744071562067968"); bug in GCC */
    TEST_Result(UInt64, -2147483647L, "18446744071562067969");
    TEST_Result(UInt64, 9223372036854775807LL, "9223372036854775807");
    /* TEST_Result(Int64, -9223372036854775808LL, "9223372036854775808"); bug in GCC */
    TEST_Result(UInt64, -9223372036854775807LL, "9223372036854775809");
    TEST_Result(UInt64, 18446744073709551615ULL, "18446744073709551615");

    TEST_ResultBase(UInt64, 0xffffffffffffffffULL, 16, "#HFFFFFFFFFFFFFFFF");
    TEST_ResultBase(UInt64, 0xffffffffffffffffULL, 8, "#Q1777777777777777777777");
    TEST_ResultBase(UInt64, 0xffffffffffffffffULL, 2, "#B1111111111111111111111111111111111111111111111111111111111111111");
}

static void testResultFloat(void) {
    TEST_Result(Float, 10, "10");
    TEST_Result(Float, -10, "-10");
    TEST_Result(Float, 127, "127");
    TEST_Result(Float, -128, "-128");
    TEST_Result(Float, 32767, "32767");
    TEST_Result(Float, -32768, "-32768");
    TEST_Result(Float, 2147483647L, "2.14748e+09");
    /* TEST_Result(Float, -2147483648, "-2.14748e+09"); bug in GCC */
    TEST_Result(Float, -2147483647L, "-2.14748e+09");
    TEST_Result(Float, 9223372036854775807LL, "9.22337e+18");
    TEST_Result(Float, -9223372036854775807LL, "-9.22337e+18");

    TEST_Result(Float, 1.256e-17, "1.256e-17");
    TEST_Result(Float, -1.256e-17, "-1.256e-17");
}

static void testResultDouble(void) {
    TEST_Result(Double, 10, "10");
    TEST_Result(Double, -10, "-10");
    TEST_Result(Double, 127, "127");
    TEST_Result(Double, -128, "-128");
    TEST_Result(Double, 32767, "32767");
    TEST_Result(Double, -32768, "-32768");
    TEST_Result(Double, 2147483647, "2147483647");
    /* TEST_Result(Double, -2147483648, "-2147483648"); bug in GCC */
    TEST_Result(Double, -2147483647, "-2147483647");
    /* TEST_Result(Double, 9223372036854775807LL, "9.22337203685478e+18"); */
    /* TEST_Result(Double, -9223372036854775807LL, "-9.22337203685478e+18"); */
    TEST_Result(Double, 9223372036854700000LL, "9.2233720368547e+18");
    TEST_Result(Double, -9223372036854700000LL, "-9.2233720368547e+18");

    TEST_Result(Double, 1.256e-17, "1.256e-17");
    TEST_Result(Double, -1.256e-17, "-1.256e-17");
}

static void testResultBool(void) {
    TEST_Result(Bool, TRUE, "1");
    TEST_Result(Bool, FALSE, "0");
    TEST_Result(Bool, 1000, "1");
}

static void testResultMnemonic(void) {
    TEST_Result(Mnemonic, "a", "a");
    TEST_Result(Mnemonic, "abcd", "abcd");
    TEST_Result(Mnemonic, "abcd123", "abcd123");
}

static void testResultText(void) {
    TEST_Result(Text, "a", "\"a\"");
    TEST_Result(Text, "abcd", "\"abcd\"");
    TEST_Result(Text, "abcd123", "\"abcd123\"");
    TEST_Result(Text, "abcd\"123", "\"abcd\"\"123\"");
    TEST_Result(Text, "abcd\"", "\"abcd\"\"\"");
}

static void testResultArbitraryBlock(void) {
#define SCPI_ResultArbitraryBlockString(c, s) SCPI_ResultArbitraryBlock((c), (s), strlen(s))

    TEST_Result(ArbitraryBlockString, "a", "#11a");
    TEST_Result(ArbitraryBlockString, "a\"", "#12a\"");
    TEST_Result(ArbitraryBlockString, "a\r\n", "#13a\r\n");
    TEST_Result(ArbitraryBlockString, "X1234567890", "#211X1234567890");
    TEST_Result(ArbitraryBlockString, "X1234567890\x80x", "#213X1234567890\x80x");
}

static void testResultArray(void) {

#define SCPI_ResultArrayInt8ASCII(c, a) SCPI_ResultArrayInt8((c), (a), sizeof(a)/sizeof(*a), SCPI_FORMAT_ASCII)
#define SCPI_ResultArrayUInt8ASCII(c, a) SCPI_ResultArrayUInt8((c), (a), sizeof(a)/sizeof(*a), SCPI_FORMAT_ASCII)
#define SCPI_ResultArrayInt16ASCII(c, a) SCPI_ResultArrayInt16((c), (a), sizeof(a)/sizeof(*a), SCPI_FORMAT_ASCII)
#define SCPI_ResultArrayUInt16ASCII(c, a) SCPI_ResultArrayUInt16((c), (a), sizeof(a)/sizeof(*a), SCPI_FORMAT_ASCII)
#define SCPI_ResultArrayInt32ASCII(c, a) SCPI_ResultArrayInt32((c), (a), sizeof(a)/sizeof(*a), SCPI_FORMAT_ASCII)
#define SCPI_ResultArrayUInt32ASCII(c, a) SCPI_ResultArrayUInt32((c), (a), sizeof(a)/sizeof(*a), SCPI_FORMAT_ASCII)
#define SCPI_ResultArrayInt64ASCII(c, a) SCPI_ResultArrayInt64((c), (a), sizeof(a)/sizeof(*a), SCPI_FORMAT_ASCII)
#define SCPI_ResultArrayUInt64ASCII(c, a) SCPI_ResultArrayUInt64((c), (a), sizeof(a)/sizeof(*a), SCPI_FORMAT_ASCII)
#define SCPI_ResultArrayFloatASCII(c, a) SCPI_ResultArrayFloat((c), (a), sizeof(a)/sizeof(*a), SCPI_FORMAT_ASCII)
#define SCPI_ResultArrayDoubleASCII(c, a) SCPI_ResultArrayDouble((c), (a), sizeof(a)/sizeof(*a), SCPI_FORMAT_ASCII)

#define SCPI_ResultArrayInt8NORMAL(c, a) SCPI_ResultArrayInt8((c), (a), sizeof(a)/sizeof(*a), SCPI_FORMAT_NORMAL)
#define SCPI_ResultArrayUInt8NORMAL(c, a) SCPI_ResultArrayUInt8((c), (a), sizeof(a)/sizeof(*a), SCPI_FORMAT_NORMAL)
#define SCPI_ResultArrayInt16NORMAL(c, a) SCPI_ResultArrayInt16((c), (a), sizeof(a)/sizeof(*a), SCPI_FORMAT_NORMAL)
#define SCPI_ResultArrayUInt16NORMAL(c, a) SCPI_ResultArrayUInt16((c), (a), sizeof(a)/sizeof(*a), SCPI_FORMAT_NORMAL)
#define SCPI_ResultArrayInt32NORMAL(c, a) SCPI_ResultArrayInt32((c), (a), sizeof(a)/sizeof(*a), SCPI_FORMAT_NORMAL)
#define SCPI_ResultArrayUInt32NORMAL(c, a) SCPI_ResultArrayUInt32((c), (a), sizeof(a)/sizeof(*a), SCPI_FORMAT_NORMAL)
#define SCPI_ResultArrayInt64NORMAL(c, a) SCPI_ResultArrayInt64((c), (a), sizeof(a)/sizeof(*a), SCPI_FORMAT_NORMAL)
#define SCPI_ResultArrayUInt64NORMAL(c, a) SCPI_ResultArrayUInt64((c), (a), sizeof(a)/sizeof(*a), SCPI_FORMAT_NORMAL)
#define SCPI_ResultArrayFloatNORMAL(c, a) SCPI_ResultArrayFloat((c), (a), sizeof(a)/sizeof(*a), SCPI_FORMAT_NORMAL)
#define SCPI_ResultArrayDoubleNORMAL(c, a) SCPI_ResultArrayDouble((c), (a), sizeof(a)/sizeof(*a), SCPI_FORMAT_NORMAL)

#define SCPI_ResultArrayInt8SWAPPED(c, a) SCPI_ResultArrayInt8((c), (a), sizeof(a)/sizeof(*a), SCPI_FORMAT_SWAPPED)
#define SCPI_ResultArrayUInt8SWAPPED(c, a) SCPI_ResultArrayUInt8((c), (a), sizeof(a)/sizeof(*a), SCPI_FORMAT_SWAPPED)
#define SCPI_ResultArrayInt16SWAPPED(c, a) SCPI_ResultArrayInt16((c), (a), sizeof(a)/sizeof(*a), SCPI_FORMAT_SWAPPED)
#define SCPI_ResultArrayUInt16SWAPPED(c, a) SCPI_ResultArrayUInt16((c), (a), sizeof(a)/sizeof(*a), SCPI_FORMAT_SWAPPED)
#define SCPI_ResultArrayInt32SWAPPED(c, a) SCPI_ResultArrayInt32((c), (a), sizeof(a)/sizeof(*a), SCPI_FORMAT_SWAPPED)
#define SCPI_ResultArrayUInt32SWAPPED(c, a) SCPI_ResultArrayUInt32((c), (a), sizeof(a)/sizeof(*a), SCPI_FORMAT_SWAPPED)
#define SCPI_ResultArrayInt64SWAPPED(c, a) SCPI_ResultArrayInt64((c), (a), sizeof(a)/sizeof(*a), SCPI_FORMAT_SWAPPED)
#define SCPI_ResultArrayUInt64SWAPPED(c, a) SCPI_ResultArrayUInt64((c), (a), sizeof(a)/sizeof(*a), SCPI_FORMAT_SWAPPED)
#define SCPI_ResultArrayFloatSWAPPED(c, a) SCPI_ResultArrayFloat((c), (a), sizeof(a)/sizeof(*a), SCPI_FORMAT_SWAPPED)
#define SCPI_ResultArrayDoubleSWAPPED(c, a) SCPI_ResultArrayDouble((c), (a), sizeof(a)/sizeof(*a), SCPI_FORMAT_SWAPPED)

    int8_t int8_arr[] = {-5, 48, 49, 109, 87};
    TEST_Result(ArrayInt8ASCII, int8_arr, "-5,48,49,109,87");
    TEST_Result(ArrayInt8NORMAL, int8_arr, "#15" "\xFB" "01mW");
    TEST_Result(ArrayInt8SWAPPED, int8_arr, "#15" "\xFB" "01mW");

    uint8_t uint8_arr[] = {250, 48, 49, 109, 87};
    TEST_Result(ArrayUInt8ASCII, uint8_arr, "250,48,49,109,87");
    TEST_Result(ArrayUInt8NORMAL, uint8_arr, "#15" "\xFA" "01mW");
    TEST_Result(ArrayUInt8SWAPPED, uint8_arr, "#15" "\xFA" "01mW");

    int16_t int16_arr[] = {-5, 18505, 12340};
    TEST_Result(ArrayInt16ASCII, int16_arr, "-5,18505,12340");
    TEST_Result(ArrayInt16NORMAL, int16_arr, "#16" "\xFF\xFB" "HI" "04");
    TEST_Result(ArrayInt16SWAPPED, int16_arr, "#16" "\xFB\xFF" "IH" "40");

    uint16_t uint16_arr[] = {65531, 18505, 12340};
    TEST_Result(ArrayUInt16ASCII, uint16_arr, "65531,18505,12340");
    TEST_Result(ArrayUInt16NORMAL, uint16_arr, "#16" "\xFF\xFB" "HI" "04");
    TEST_Result(ArrayUInt16SWAPPED, uint16_arr, "#16" "\xFB\xFF" "IH" "40");

    int32_t int32_arr[] = {-5L, 808530483L, 1094861636L};
    TEST_Result(ArrayInt32ASCII, int32_arr, "-5,808530483,1094861636");
    TEST_Result(ArrayInt32NORMAL, int32_arr, "#212" "\xFF\xFF\xFF\xFB" "0123" "ABCD");
    TEST_Result(ArrayInt32SWAPPED, int32_arr, "#212" "\xFB\xFF\xFF\xFF" "3210" "DCBA");

    uint32_t uint32_arr[] = {4294967291UL, 808530483UL, 1094861636UL};
    TEST_Result(ArrayUInt32ASCII, uint32_arr, "4294967291,808530483,1094861636");
    TEST_Result(ArrayUInt32NORMAL, uint32_arr, "#212" "\xFF\xFF\xFF\xFB" "0123" "ABCD");
    TEST_Result(ArrayUInt32SWAPPED, uint32_arr, "#212" "\xFB\xFF\xFF\xFF" "3210" "DCBA");

    int64_t int64_arr[] = {-5LL, 3472611983179986487LL};
    TEST_Result(ArrayInt64ASCII, int64_arr, "-5,3472611983179986487");
    TEST_Result(ArrayInt64NORMAL, int64_arr, "#216" "\xFF\xFF\xFF\xFF" "\xFF\xFF\xFF\xFB" "01234567");
    TEST_Result(ArrayInt64SWAPPED, int64_arr, "#216" "\xFB\xFF\xFF\xFF" "\xFF\xFF\xFF\xFF" "76543210");

    uint64_t uint64_arr[] = {18446744073709551611ULL, 3472611983179986487ULL};
    TEST_Result(ArrayUInt64ASCII, uint64_arr, "18446744073709551611,3472611983179986487");
    TEST_Result(ArrayUInt64NORMAL, uint64_arr, "#216" "\xFF\xFF\xFF\xFF" "\xFF\xFF\xFF\xFB" "01234567");
    TEST_Result(ArrayUInt64SWAPPED, uint64_arr, "#216" "\xFB\xFF\xFF\xFF" "\xFF\xFF\xFF\xFF" "76543210");

    float float_arr[] = {0.7549173, 3.0196693};
    TEST_Result(ArrayFloatASCII, float_arr, "0.754917,3.01967");
    TEST_Result(ArrayFloatNORMAL, float_arr, "#18" "?ABC" "@ABC");
    TEST_Result(ArrayFloatSWAPPED, float_arr, "#18" "CBA?" "CBA@");

    double double_arr[] = {76543217654321, 1234567891234567};
    TEST_Result(ArrayDoubleASCII, double_arr, "76543217654321,1.23456789123457e+15");
    TEST_Result(ArrayDoubleNORMAL, double_arr, "#216" "\x42\xd1\x67\x66\xd3\x16\x8c\x40" "\x43\x11\x8b\x54\xf2\x6e\xbc\x1c");
    TEST_Result(ArrayDoubleSWAPPED, double_arr, "#216" "\x40\x8c\x16\xd3\x66\x67\xd1\x42" "\x1c\xbc\x6e\xf2\x54\x8b\x11\x43");
}

#define _countof(a) (sizeof(a)/sizeof(*(a)))

#define TEST_ParamArrayDouble(T, func, data, mandatory, _expected_value, expected_result, expected_error_code) \
{                                                                                       \
    T value[10];                                                                        \
    scpi_bool_t result;                                                                 \
    scpi_error_t errCode;                                                               \
    T expected_value[] = {NOPAREN _expected_value};                                     \
    size_t o_count;                                                                     \
    size_t i_count = _countof(expected_value);                                          \
                                                                                        \
    SCPI_CoreCls(&scpi_context);                                                        \
    scpi_context.input_count = 0;                                                       \
    scpi_context.param_list.lex_state.buffer = data;                                    \
    scpi_context.param_list.lex_state.len = strlen(scpi_context.param_list.lex_state.buffer);\
    scpi_context.param_list.lex_state.pos = scpi_context.param_list.lex_state.buffer;   \
    result = func(&scpi_context, value, 10, &o_count, SCPI_FORMAT_ASCII, mandatory);    \
                                                                                        \
    SCPI_ErrorPop(&scpi_context, &errCode);                                             \
    CU_ASSERT_EQUAL(result, expected_result);                                           \
    if (expected_result) {                                                              \
        CU_ASSERT_EQUAL(i_count, o_count);                                              \
        size_t i;                                                                       \
        for(i = 0; i < o_count; i++) {                                                  \
            CU_ASSERT_DOUBLE_EQUAL(value[i], expected_value[i], 0.000001);              \
        }                                                                               \
    }                                                                                   \
    CU_ASSERT_EQUAL(errCode.error_code, expected_error_code);                           \
}

#define TEST_ParamArrayInt(T, func, data, mandatory, _expected_value, expected_result, expected_error_code) \
{                                                                                       \
    T value[10];                                                                        \
    scpi_bool_t result;                                                                 \
    scpi_error_t errCode;                                                               \
    T expected_value[] = {NOPAREN _expected_value};                                     \
    size_t o_count;                                                                     \
    size_t i_count = _countof(expected_value);                                          \
                                                                                        \
    SCPI_CoreCls(&scpi_context);                                                        \
    scpi_context.input_count = 0;                                                       \
    scpi_context.param_list.lex_state.buffer = data;                                    \
    scpi_context.param_list.lex_state.len = strlen(scpi_context.param_list.lex_state.buffer);\
    scpi_context.param_list.lex_state.pos = scpi_context.param_list.lex_state.buffer;   \
    result = func(&scpi_context, value, 10, &o_count, SCPI_FORMAT_ASCII, mandatory);    \
                                                                                        \
    SCPI_ErrorPop(&scpi_context, &errCode);                                             \
    CU_ASSERT_EQUAL(result, expected_result);                                           \
    if (expected_result) {                                                              \
        CU_ASSERT_EQUAL(i_count, o_count);                                              \
        size_t i;                                                                       \
        for(i = 0; i < o_count; i++) {                                                  \
            CU_ASSERT_EQUAL(value[i], expected_value[i]);                               \
        }                                                                               \
    }                                                                                   \
    CU_ASSERT_EQUAL(errCode.error_code, expected_error_code);                           \
}

static void testParamArray(void) {
    TEST_ParamArrayDouble(double, SCPI_ParamArrayDouble, "1, 2, 3", TRUE, (1, 2, 3), TRUE, SCPI_ERROR_NO_ERROR);
    TEST_ParamArrayDouble(double, SCPI_ParamArrayDouble, "", TRUE, (0), FALSE, SCPI_ERROR_MISSING_PARAMETER);
    TEST_ParamArrayDouble(double, SCPI_ParamArrayDouble, "1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11", TRUE, (1, 2, 3, 4, 5, 6, 7, 8, 9, 10), TRUE, SCPI_ERROR_NO_ERROR);

    TEST_ParamArrayDouble(float, SCPI_ParamArrayFloat, "1, 2, 3", TRUE, (1, 2, 3), TRUE, SCPI_ERROR_NO_ERROR);
    TEST_ParamArrayDouble(float, SCPI_ParamArrayFloat, "", TRUE, (0), FALSE, SCPI_ERROR_MISSING_PARAMETER);
    TEST_ParamArrayDouble(float, SCPI_ParamArrayFloat, "1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11", TRUE, (1, 2, 3, 4, 5, 6, 7, 8, 9, 10), TRUE, SCPI_ERROR_NO_ERROR);

    TEST_ParamArrayInt(int32_t, SCPI_ParamArrayInt32, "1, 2, 3", TRUE, (1, 2, 3), TRUE, SCPI_ERROR_NO_ERROR);
    TEST_ParamArrayInt(int32_t, SCPI_ParamArrayInt32, "", TRUE, (0), FALSE, SCPI_ERROR_MISSING_PARAMETER);
    TEST_ParamArrayInt(int32_t, SCPI_ParamArrayInt32, "1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11", TRUE, (1, 2, 3, 4, 5, 6, 7, 8, 9, 10), TRUE, SCPI_ERROR_NO_ERROR);

    TEST_ParamArrayInt(uint32_t, SCPI_ParamArrayUInt32, "1, 2, 3", TRUE, (1, 2, 3), TRUE, SCPI_ERROR_NO_ERROR);
    TEST_ParamArrayInt(uint32_t, SCPI_ParamArrayUInt32, "", TRUE, (0), FALSE, SCPI_ERROR_MISSING_PARAMETER);
    TEST_ParamArrayInt(uint32_t, SCPI_ParamArrayUInt32, "1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11", TRUE, (1, 2, 3, 4, 5, 6, 7, 8, 9, 10), TRUE, SCPI_ERROR_NO_ERROR);

    TEST_ParamArrayInt(int64_t, SCPI_ParamArrayInt64, "1, 2, 3", TRUE, (1, 2, 3), TRUE, SCPI_ERROR_NO_ERROR);
    TEST_ParamArrayInt(int64_t, SCPI_ParamArrayInt64, "", TRUE, (0), FALSE, SCPI_ERROR_MISSING_PARAMETER);
    TEST_ParamArrayInt(int64_t, SCPI_ParamArrayInt64, "1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11", TRUE, (1, 2, 3, 4, 5, 6, 7, 8, 9, 10), TRUE, SCPI_ERROR_NO_ERROR);

    TEST_ParamArrayInt(uint64_t, SCPI_ParamArrayUInt64, "1, 2, 3", TRUE, (1, 2, 3), TRUE, SCPI_ERROR_NO_ERROR);
    TEST_ParamArrayInt(uint64_t, SCPI_ParamArrayUInt64, "", TRUE, (0), FALSE, SCPI_ERROR_MISSING_PARAMETER);
    TEST_ParamArrayInt(uint64_t, SCPI_ParamArrayUInt64, "1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11", TRUE, (1, 2, 3, 4, 5, 6, 7, 8, 9, 10), TRUE, SCPI_ERROR_NO_ERROR);
}

static void testNumberToStr(void) {

#define TEST_SCPI_NumberToStr(_special, _value, _unit, expected_result) do {\
    scpi_number_t number;\
    number.base = 10;\
    number.special = (_special);\
    number.unit = (_unit);\
    if (number.special) { number.content.tag = (int)(_value); } else { number.content.value = (_value); }\
    char buffer[100 + 1];\
    size_t res_len;\
    res_len = SCPI_NumberToStr(&scpi_context, scpi_special_numbers_def, &number, buffer, 100);\
    CU_ASSERT_STRING_EQUAL(buffer, expected_result);\
    CU_ASSERT_EQUAL(res_len, strlen(expected_result));\
} while(0)

#define TEST_SCPI_NumberToStr_limited(_special, _value, _unit, expected_result, limit) do {\
    scpi_number_t number;\
    number.base = 10;\
    number.special = (_special);\
    number.unit = (_unit);\
    if (number.special) { number.content.tag = (int)(_value); } else { number.content.value = (_value); }\
    char buffer[100];\
    memset(buffer, 0xaa, 100);\
    size_t res_len;\
    res_len = SCPI_NumberToStr(&scpi_context, scpi_special_numbers_def, &number, buffer, limit);\
    size_t expected_len = SCPIDEFINE_strnlen(expected_result, limit - 1);\
    CU_ASSERT_NSTRING_EQUAL(buffer, expected_result, expected_len);\
    CU_ASSERT_EQUAL(buffer[expected_len], 0);\
    CU_ASSERT_EQUAL((unsigned char)buffer[limit], 0xaa);\
    CU_ASSERT_EQUAL(res_len, expected_len);\
} while(0)

    TEST_SCPI_NumberToStr(FALSE, 10.5, SCPI_UNIT_NONE, "10.5");
    TEST_SCPI_NumberToStr(FALSE, 10.5, SCPI_UNIT_VOLT, "10.5 V");
    TEST_SCPI_NumberToStr(TRUE, SCPI_NUM_DEF, SCPI_UNIT_NONE, "DEFault");

    TEST_SCPI_NumberToStr_limited(FALSE, 10.5, SCPI_UNIT_NONE, "10.5", 1);
    TEST_SCPI_NumberToStr_limited(FALSE, 10.5, SCPI_UNIT_VOLT, "10.5 V", 1);
    TEST_SCPI_NumberToStr_limited(TRUE, SCPI_NUM_DEF, SCPI_UNIT_NONE, "DEFault", 1);

    TEST_SCPI_NumberToStr_limited(FALSE, 10.5, SCPI_UNIT_NONE, "10.5", 2);
    TEST_SCPI_NumberToStr_limited(FALSE, 10.5, SCPI_UNIT_VOLT, "10.5 V", 2);
    TEST_SCPI_NumberToStr_limited(TRUE, SCPI_NUM_DEF, SCPI_UNIT_NONE, "DEFault", 2);

    TEST_SCPI_NumberToStr_limited(FALSE, 10.5, SCPI_UNIT_NONE, "10.5", 3);
    TEST_SCPI_NumberToStr_limited(FALSE, 10.5, SCPI_UNIT_VOLT, "10.5 V", 3);
    TEST_SCPI_NumberToStr_limited(TRUE, SCPI_NUM_DEF, SCPI_UNIT_NONE, "DEFault", 3);

    TEST_SCPI_NumberToStr_limited(FALSE, 10.5, SCPI_UNIT_NONE, "10.5", 4);
    TEST_SCPI_NumberToStr_limited(FALSE, 10.5, SCPI_UNIT_VOLT, "10.5 V", 4);
    TEST_SCPI_NumberToStr_limited(TRUE, SCPI_NUM_DEF, SCPI_UNIT_NONE, "DEFault", 4);

    TEST_SCPI_NumberToStr_limited(FALSE, 10.5, SCPI_UNIT_NONE, "10.5", 5);
    TEST_SCPI_NumberToStr_limited(FALSE, 10.5, SCPI_UNIT_VOLT, "10.5 V", 5);
    TEST_SCPI_NumberToStr_limited(TRUE, SCPI_NUM_DEF, SCPI_UNIT_NONE, "DEFault", 5);

    TEST_SCPI_NumberToStr_limited(FALSE, 10.5, SCPI_UNIT_NONE, "10.5", 6);
    TEST_SCPI_NumberToStr_limited(FALSE, 10.5, SCPI_UNIT_VOLT, "10.5 V", 6);
    TEST_SCPI_NumberToStr_limited(TRUE, SCPI_NUM_DEF, SCPI_UNIT_NONE, "DEFault", 6);

    TEST_SCPI_NumberToStr_limited(FALSE, 10.5, SCPI_UNIT_NONE, "10.5", 7);
    TEST_SCPI_NumberToStr_limited(FALSE, 10.5, SCPI_UNIT_VOLT, "10.5 V", 7);
    TEST_SCPI_NumberToStr_limited(TRUE, SCPI_NUM_DEF, SCPI_UNIT_NONE, "DEFault", 7);

    TEST_SCPI_NumberToStr_limited(FALSE, 10.5, SCPI_UNIT_NONE, "10.5", 8);
    TEST_SCPI_NumberToStr_limited(FALSE, 10.5, SCPI_UNIT_VOLT, "10.5 V", 8);
    TEST_SCPI_NumberToStr_limited(TRUE, SCPI_NUM_DEF, SCPI_UNIT_NONE, "DEFault", 8);

    TEST_SCPI_NumberToStr_limited(FALSE, 10.5, SCPI_UNIT_NONE, "10.5", 9);
    TEST_SCPI_NumberToStr_limited(FALSE, 10.5, SCPI_UNIT_VOLT, "10.5 V", 9);
    TEST_SCPI_NumberToStr_limited(TRUE, SCPI_NUM_DEF, SCPI_UNIT_NONE, "DEFault", 9);
}

static void testErrorQueue(void) {
    scpi_error_t val;
    SCPI_ErrorClear(&scpi_context);
    CU_ASSERT_EQUAL(SCPI_ErrorCount(&scpi_context), 0);
    SCPI_ErrorPush(&scpi_context, -1);
    CU_ASSERT_EQUAL(SCPI_ErrorCount(&scpi_context), 1);
    SCPI_ErrorPush(&scpi_context, -2);
    CU_ASSERT_EQUAL(SCPI_ErrorCount(&scpi_context), 2);
    SCPI_ErrorPush(&scpi_context, -3);
    CU_ASSERT_EQUAL(SCPI_ErrorCount(&scpi_context), 3);
    SCPI_ErrorPush(&scpi_context, -4);
    CU_ASSERT_EQUAL(SCPI_ErrorCount(&scpi_context), 4);
    SCPI_ErrorPush(&scpi_context, -5);
    CU_ASSERT_EQUAL(SCPI_ErrorCount(&scpi_context), 4);
    SCPI_ErrorPush(&scpi_context, -6);
    CU_ASSERT_EQUAL(SCPI_ErrorCount(&scpi_context), 4);

    SCPI_ErrorPop(&scpi_context, &val);
    CU_ASSERT_EQUAL(val.error_code, -1);
    CU_ASSERT_EQUAL(SCPI_ErrorCount(&scpi_context), 3);
    SCPI_ErrorPop(&scpi_context, &val);
    CU_ASSERT_EQUAL(val.error_code, -2);
    CU_ASSERT_EQUAL(SCPI_ErrorCount(&scpi_context), 2);
    SCPI_ErrorPop(&scpi_context, &val);
    CU_ASSERT_EQUAL(val.error_code, -3);
    CU_ASSERT_EQUAL(SCPI_ErrorCount(&scpi_context), 1);
    SCPI_ErrorPop(&scpi_context, &val);
    CU_ASSERT_EQUAL(val.error_code, SCPI_ERROR_QUEUE_OVERFLOW);
    CU_ASSERT_EQUAL(SCPI_ErrorCount(&scpi_context), 0);
    SCPI_ErrorPop(&scpi_context, &val);
    CU_ASSERT_EQUAL(val.error_code, 0);
    CU_ASSERT_EQUAL(SCPI_ErrorCount(&scpi_context), 0);

    SCPI_ErrorClear(&scpi_context);
}

#define TEST_INCOMPLETE_ARB(_val, _part_len) do {\
    double val = _val;\
    char command_text[] = "SAMple #18[DOUBLE]\r";\
    char * command = command_text;\
    size_t command_len = strlen(command);\
    memcpy(command + 10, &val, sizeof(val));\
    test_sample_received = NAN;\
    size_t part_len = _part_len;\
    SCPI_ErrorClear(&scpi_context);\
    while (command_len) {\
        part_len = part_len > command_len ? command_len : part_len;\
        SCPI_Input(&scpi_context, command, part_len);\
        command += part_len;\
        command_len -= part_len;\
    }\
    CU_ASSERT_EQUAL(SCPI_ErrorCount(&scpi_context), 0);\
    CU_ASSERT_EQUAL(test_sample_received, val);\
} while(0)

static void testIncompleteArbitraryParameter(void) {
    TEST_INCOMPLETE_ARB(0.5, 19);
    TEST_INCOMPLETE_ARB(0.5, 18);
    TEST_INCOMPLETE_ARB(0.5, 17);
    TEST_INCOMPLETE_ARB(0.5, 16);
    TEST_INCOMPLETE_ARB(0.5, 15);
    TEST_INCOMPLETE_ARB(0.5, 14);
    TEST_INCOMPLETE_ARB(0.5, 13);
    TEST_INCOMPLETE_ARB(0.5, 12);
    TEST_INCOMPLETE_ARB(0.5, 11);
    TEST_INCOMPLETE_ARB(0.5, 10);
    TEST_INCOMPLETE_ARB(0.5, 9);
    TEST_INCOMPLETE_ARB(0.5, 8);
    TEST_INCOMPLETE_ARB(0.5, 7);
    TEST_INCOMPLETE_ARB(0.5, 6);
    TEST_INCOMPLETE_ARB(0.5, 5);
    TEST_INCOMPLETE_ARB(0.5, 4);
    TEST_INCOMPLETE_ARB(0.5, 3);
    TEST_INCOMPLETE_ARB(0.5, 2);
    TEST_INCOMPLETE_ARB(0.5, 1);

    TEST_INCOMPLETE_ARB(0.501220703125, 19);
    TEST_INCOMPLETE_ARB(0.501220703125, 18);
    TEST_INCOMPLETE_ARB(0.501220703125, 17);
    TEST_INCOMPLETE_ARB(0.501220703125, 16);
    TEST_INCOMPLETE_ARB(0.501220703125, 15);
    TEST_INCOMPLETE_ARB(0.501220703125, 14);
    TEST_INCOMPLETE_ARB(0.501220703125, 13);
    TEST_INCOMPLETE_ARB(0.501220703125, 12);
    TEST_INCOMPLETE_ARB(0.501220703125, 11);
    TEST_INCOMPLETE_ARB(0.501220703125, 10);
    TEST_INCOMPLETE_ARB(0.501220703125, 9);
    TEST_INCOMPLETE_ARB(0.501220703125, 8);
    TEST_INCOMPLETE_ARB(0.501220703125, 7);
    TEST_INCOMPLETE_ARB(0.501220703125, 6);
    TEST_INCOMPLETE_ARB(0.501220703125, 5);
    TEST_INCOMPLETE_ARB(0.501220703125, 4);
    TEST_INCOMPLETE_ARB(0.501220703125, 3);
    TEST_INCOMPLETE_ARB(0.501220703125, 2);
    TEST_INCOMPLETE_ARB(0.501220703125, 1);

    TEST_INCOMPLETE_ARB(0.500000024214387, 19);
    TEST_INCOMPLETE_ARB(0.500000024214387, 18);
    TEST_INCOMPLETE_ARB(0.500000024214387, 17);
    TEST_INCOMPLETE_ARB(0.500000024214387, 16);
    TEST_INCOMPLETE_ARB(0.500000024214387, 15);
    TEST_INCOMPLETE_ARB(0.500000024214387, 14);
    TEST_INCOMPLETE_ARB(0.500000024214387, 13);
    TEST_INCOMPLETE_ARB(0.500000024214387, 12);
    TEST_INCOMPLETE_ARB(0.500000024214387, 11);
    TEST_INCOMPLETE_ARB(0.500000024214387, 10);
    TEST_INCOMPLETE_ARB(0.500000024214387, 9);
    TEST_INCOMPLETE_ARB(0.500000024214387, 8);
    TEST_INCOMPLETE_ARB(0.500000024214387, 7);
    TEST_INCOMPLETE_ARB(0.500000024214387, 6);
    TEST_INCOMPLETE_ARB(0.500000024214387, 5);
    TEST_INCOMPLETE_ARB(0.500000024214387, 4);
    TEST_INCOMPLETE_ARB(0.500000024214387, 3);
    TEST_INCOMPLETE_ARB(0.500000024214387, 2);
    TEST_INCOMPLETE_ARB(0.500000024214387, 1);
}

#define TEST_INCOMPLETE_TEXT(_text, _part_len) do {\
    char command_text[] = "TEXT? \"\", \"" _text "\"\r";\
    char * command = command_text;\
    size_t command_len = strlen(command);\
    output_buffer_clear();\
    size_t part_len = _part_len;\
    SCPI_ErrorClear(&scpi_context);\
    while (command_len) {\
        part_len = part_len > command_len ? command_len : part_len;\
        SCPI_Input(&scpi_context, command, part_len);\
        command += part_len;\
        command_len -= part_len;\
    }\
    CU_ASSERT_EQUAL(SCPI_ErrorCount(&scpi_context), 0);\
    CU_ASSERT_STRING_EQUAL("\"" _text "\"\r\n", output_buffer);\
} while(0)

static void testIncompleteTextParameter(void) {
    TEST_INCOMPLETE_TEXT("AbcdEfgh", 20);
    TEST_INCOMPLETE_TEXT("AbcdEfgh", 19);
    TEST_INCOMPLETE_TEXT("AbcdEfgh", 18);
    TEST_INCOMPLETE_TEXT("AbcdEfgh", 17);
    TEST_INCOMPLETE_TEXT("AbcdEfgh", 16);
    TEST_INCOMPLETE_TEXT("AbcdEfgh", 15);
    TEST_INCOMPLETE_TEXT("AbcdEfgh", 14);
    TEST_INCOMPLETE_TEXT("AbcdEfgh", 13);
    TEST_INCOMPLETE_TEXT("AbcdEfgh", 12);
    TEST_INCOMPLETE_TEXT("AbcdEfgh", 11);
    TEST_INCOMPLETE_TEXT("AbcdEfgh", 10);
    TEST_INCOMPLETE_TEXT("AbcdEfgh", 9);
    TEST_INCOMPLETE_TEXT("AbcdEfgh", 8);
    TEST_INCOMPLETE_TEXT("AbcdEfgh", 7);
    TEST_INCOMPLETE_TEXT("AbcdEfgh", 6);
    TEST_INCOMPLETE_TEXT("AbcdEfgh", 5);
    TEST_INCOMPLETE_TEXT("AbcdEfgh", 4);
    TEST_INCOMPLETE_TEXT("AbcdEfgh", 3);
    TEST_INCOMPLETE_TEXT("AbcdEfgh", 2);
    TEST_INCOMPLETE_TEXT("AbcdEfgh", 1);
}

int main() {
    unsigned int result;
    CU_pSuite pSuite = NULL;

    /* Initialize the CUnit test registry */
    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

    /* Add a suite to the registry */
    pSuite = CU_add_suite("Parser", init_suite, clean_suite);
    if (NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* Add the tests to the suite */
    if ((NULL == CU_add_test(pSuite, "SCPI_ParamInt32", testSCPI_ParamInt32))
            || (NULL == CU_add_test(pSuite, "SCPI_ParamUInt32", testSCPI_ParamUInt32))
            || (NULL == CU_add_test(pSuite, "SCPI_ParamInt64", testSCPI_ParamInt64))
            || (NULL == CU_add_test(pSuite, "SCPI_ParamUInt64", testSCPI_ParamUInt64))
            || (NULL == CU_add_test(pSuite, "SCPI_ParamFloat", testSCPI_ParamFloat))
            || (NULL == CU_add_test(pSuite, "SCPI_ParamDouble", testSCPI_ParamDouble))
            || (NULL == CU_add_test(pSuite, "SCPI_ParamCharacters", testSCPI_ParamCharacters))
            || (NULL == CU_add_test(pSuite, "SCPI_ParamCopyText", testSCPI_ParamCopyText))
            || (NULL == CU_add_test(pSuite, "SCPI_ParamArbitraryBlock", testSCPI_ParamArbitraryBlock))
            || (NULL == CU_add_test(pSuite, "SCPI_ParamBool", testSCPI_ParamBool))
            || (NULL == CU_add_test(pSuite, "SCPI_ParamChoice", testSCPI_ParamChoice))
            || (NULL == CU_add_test(pSuite, "Commands handling", testCommandsHandling))
            || (NULL == CU_add_test(pSuite, "Error handling", testErrorHandling))
            || (NULL == CU_add_test(pSuite, "Device dependent error handling", testErrorHandlingDeviceDependent))
            || (NULL == CU_add_test(pSuite, "IEEE 488.2 Mandatory commands", testIEEE4882))
            || (NULL == CU_add_test(pSuite, "Numeric list", testNumericList))
            || (NULL == CU_add_test(pSuite, "Channel list", testChannelList))
            || (NULL == CU_add_test(pSuite, "SCPI_ParamNumber", testParamNumber))
            || (NULL == CU_add_test(pSuite, "SCPI_ResultInt8", testResultInt8))
            || (NULL == CU_add_test(pSuite, "SCPI_ResultUInt8", testResultUInt8))
            || (NULL == CU_add_test(pSuite, "SCPI_ResultInt16", testResultInt16))
            || (NULL == CU_add_test(pSuite, "SCPI_ResultUInt16", testResultUInt16))
            || (NULL == CU_add_test(pSuite, "SCPI_ResultInt32", testResultInt32))
            || (NULL == CU_add_test(pSuite, "SCPI_ResultUInt32", testResultUInt32))
            || (NULL == CU_add_test(pSuite, "SCPI_ResultInt64", testResultInt64))
            || (NULL == CU_add_test(pSuite, "SCPI_ResultUInt64", testResultUInt64))
            || (NULL == CU_add_test(pSuite, "SCPI_ResultFloat", testResultFloat))
            || (NULL == CU_add_test(pSuite, "SCPI_ResultDouble", testResultDouble))
            || (NULL == CU_add_test(pSuite, "SCPI_ResultBool", testResultBool))
            || (NULL == CU_add_test(pSuite, "SCPI_ResultMnemonic", testResultMnemonic))
            || (NULL == CU_add_test(pSuite, "SCPI_ResultText", testResultText))
            || (NULL == CU_add_test(pSuite, "SCPI_ResultArbitraryBlock", testResultArbitraryBlock))
            || (NULL == CU_add_test(pSuite, "SCPI_ResultArray", testResultArray))
            || (NULL == CU_add_test(pSuite, "SCPI_ParamArray", testParamArray))
            || (NULL == CU_add_test(pSuite, "SCPI_NumberToStr", testNumberToStr))
            || (NULL == CU_add_test(pSuite, "SCPI_ErrorQueue", testErrorQueue))
            || (NULL == CU_add_test(pSuite, "Incomplete arbitrary parameter", testIncompleteArbitraryParameter))
            || (NULL == CU_add_test(pSuite, "Incomplete text parameter", testIncompleteTextParameter))
            ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* Run all tests using the CUnit Basic interface */
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    result = CU_get_number_of_tests_failed();
    CU_cleanup_registry();
    return result ? result : CU_get_error();
}

