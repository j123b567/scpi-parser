/*
 * File:   test_lib.c
 * Author: Jan Breuer
 *
 * Created on 2013-11-08 11:49:53
 */

#include <stdio.h>
#include <stdlib.h>
#include "CUnit/Basic.h"

#include "scpi/scpi.h"

/*
 * CUnit Test Suite
 */

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

    {.pattern = "STATus:QUEStionable[:EVENt]?", .callback = SCPI_StatusQuestionableEventQ,},
    {.pattern = "STATus:QUEStionable:ENABle", .callback = SCPI_StatusQuestionableEnable,},
    {.pattern = "STATus:QUEStionable:ENABle?", .callback = SCPI_StatusQuestionableEnableQ,},

    {.pattern = "STATus:PRESet", .callback = SCPI_StatusPreset,},
    
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
}

scpi_t scpi_context;
static void error_buffer_clear(void) {
    err_buffer[0] = '\0';
    err_buffer_pos = 0;

    SCPI_EventClear(&scpi_context);
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
    return SCPI_RES_OK;
}

static int SCPI_Error(scpi_t * context, int_fast16_t err) {
    (void) context;

    error_buffer_add(err);

    return 0;
}

scpi_reg_val_t srq_val = 0;
static scpi_result_t SCPI_Control(scpi_t * context, scpi_ctrl_name_t ctrl, scpi_reg_val_t val) {
    if (SCPI_CTRL_SRQ == ctrl) {
        srq_val = val;
    } else {
        fprintf(stderr, "**CTRL %02x: 0x%X (%d)\r\n", ctrl, val, val);
    }
    return SCPI_RES_OK;
}

bool_t TST_executed = FALSE;
bool_t RST_executed = FALSE;
static scpi_result_t SCPI_Test(scpi_t * context) {
    TST_executed = TRUE;
    return SCPI_RES_OK;
}

static scpi_result_t SCPI_Reset(scpi_t * context) {
    RST_executed = TRUE;
    return SCPI_RES_OK;
}

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
    .idn = {"MA", "IN", NULL, "VER"},
};


int init_suite(void) {
    SCPI_Init(&scpi_context);

    return 0;
}

int clean_suite(void) {
    return 0;
}

void testCommandsHandling(void) {
#define TEST_INPUT(data, output) {                              \
    SCPI_Input(&scpi_context, data, strlen(data));              \
    CU_ASSERT_STRING_EQUAL(output, output_buffer);              \
}
    output_buffer_clear();

    /* Test single command */
    TEST_INPUT("*IDN?\r\n", "MA, IN, 0, VER\r\n");
    output_buffer_clear();

    /* Test multiple commands in input buffer */
    TEST_INPUT("*IDN?\r\n*IDN?\r\n*IDN?\r\n*IDN?\r\n", "MA, IN, 0, VER\r\nMA, IN, 0, VER\r\nMA, IN, 0, VER\r\nMA, IN, 0, VER\r\n");
    output_buffer_clear();

    /* Test one command in multiple buffers */
    TEST_INPUT("*IDN?", "");
    TEST_INPUT("\r\n", "MA, IN, 0, VER\r\n");
    output_buffer_clear();

    /* Test input "timeout" - input with length == 0 */
    TEST_INPUT("*IDN?", "");
    TEST_INPUT("", "MA, IN, 0, VER\r\n");
    output_buffer_clear();
    
    CU_ASSERT_EQUAL(err_buffer_pos, 0);
    error_buffer_clear();
    
    // TODO: Compound commands A:B;C -> A:B; A:C
}

void testErrorHandling(void) {
    output_buffer_clear();
    error_buffer_clear();

#define TEST_ERROR(data, output, err_num) {                     \
    SCPI_Input(&scpi_context, data, strlen(data));              \
    CU_ASSERT_STRING_EQUAL(output, output_buffer);              \
    error_buffer_clear();                                       \
}

    TEST_ERROR("*IDN?\r\n", "MA, IN, 0, VER\r\n", 0);
    output_buffer_clear();
    TEST_ERROR("IDN?\r\n", "", SCPI_ERROR_UNDEFINED_HEADER);
    TEST_ERROR("*ESE\r\n", "", SCPI_ERROR_MISSING_PARAMETER);
    TEST_ERROR("*IDN? 12\r\n", "MA, IN, 0, VER\r\n", SCPI_ERROR_PARAMETER_NOT_ALLOWED);
    output_buffer_clear();

    // TODO: SCPI_ERROR_INVALID_SEPARATOR
    // TODO: SCPI_ERROR_INVALID_SUFFIX
    // TODO: SCPI_ERROR_SUFFIX_NOT_ALLOWED
    // TODO: SCPI_ERROR_EXECUTION_ERROR
    // TODO: SCPI_ERROR_ILLEGAL_PARAMETER_VALUE
}

void testIEEE4882(void) {
#define TEST_IEEE4882(data, output) {                           \
    SCPI_Input(&scpi_context, data, strlen(data));              \
    CU_ASSERT_STRING_EQUAL(output, output_buffer);              \
    output_buffer_clear();                                      \
}

    output_buffer_clear();
    error_buffer_clear();

    TEST_IEEE4882("*CLS\r\n", "");
    TEST_IEEE4882("*ESE 0x20\r\n", "");
    TEST_IEEE4882("*ESE?\r\n", "32\r\n");
    TEST_IEEE4882("*ESR?\r\n", "0\r\n");
    TEST_IEEE4882("*IDN?\r\n", "MA, IN, 0, VER\r\n");
    TEST_IEEE4882("*OPC\r\n", "");
    TEST_IEEE4882("*OPC?\r\n", "1\r\n");

    TEST_IEEE4882("*SRE 0xFF\r\n", "");
    TEST_IEEE4882("*SRE?\r\n", "255\r\n");
    TEST_IEEE4882("*STB?\r\n", "0\r\n"); 
    TEST_IEEE4882("*ESR?\r\n", "1\r\n");
    
    srq_val = 0;
    TEST_IEEE4882("ABCD\r\n", ""); /* "Undefined header" cause command error */
    CU_ASSERT_EQUAL(srq_val, 96); /* value of STB as service request */
    TEST_IEEE4882("*STB?\r\n", "96\r\n"); /* Event status register + Service request */
    TEST_IEEE4882("*ESR?\r\n", "32\r\n"); /* Command error */

    TEST_IEEE4882("*STB?\r\n", "0\r\n");
    TEST_IEEE4882("*ESR?\r\n", "0\r\n");
    
    TEST_IEEE4882("SYST:ERR:NEXT?\r\n", "-113, \"Undefined header\"\r\n");
    TEST_IEEE4882("SYST:ERR:NEXT?\r\n", "0, \"No error\"\r\n");
    
    RST_executed = FALSE;
    TEST_IEEE4882("*RST\r\n", "");
    CU_ASSERT_EQUAL(RST_executed, TRUE);

    TST_executed = FALSE;
    TEST_IEEE4882("*TST?\r\n", "1\r\n");
    CU_ASSERT_EQUAL(TST_executed, TRUE);
    
    TEST_IEEE4882("*WAI\r\n", "");

    TEST_IEEE4882("SYSTem:VERSion?\r\n", "1999.0\r\n");
}

void testParameters(void) {
    // TODO: test parsin parameters
    
    // TODO: Int
    // TODO: Double
    // TODO: String
    // TODO: Text
    // TODO: Bool
    // TODO: Choice
}

void testResults(void) {
    // TODO: test producing results
    
    // TODO: String
    // TODO: Int
    // TODO: Double
    // TODO: Text
    // TODO: Bool
}

int main() {
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
    if ((NULL == CU_add_test(pSuite, "Commands handling", testCommandsHandling)) ||
        (NULL == CU_add_test(pSuite, "Error handling", testErrorHandling)) ||
        (NULL == CU_add_test(pSuite, "IEEE 488.2 Mandatory commands", testIEEE4882)) ||
        (NULL == CU_add_test(pSuite, "Parameters", testParameters)) ||
        (NULL == CU_add_test(pSuite, "Results", testResults))
    ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* Run all tests using the CUnit Basic interface */
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}
