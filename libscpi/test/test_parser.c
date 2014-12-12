/*
 * File:   test_parser.c
 * Author: Jan Breuer
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
//    { .pattern = "*CLS", .callback = SCPI_CoreCls,},
//    { .pattern = "*ESE", .callback = SCPI_CoreEse,},
//    { .pattern = "*ESE?", .callback = SCPI_CoreEseQ,},
//    { .pattern = "*ESR?", .callback = SCPI_CoreEsrQ,},
//    { .pattern = "*IDN?", .callback = SCPI_CoreIdnQ,},
//    { .pattern = "*OPC", .callback = SCPI_CoreOpc,},
//    { .pattern = "*OPC?", .callback = SCPI_CoreOpcQ,},
//    { .pattern = "*RST", .callback = SCPI_CoreRst,},
//    { .pattern = "*SRE", .callback = SCPI_CoreSre,},
//    { .pattern = "*SRE?", .callback = SCPI_CoreSreQ,},
//    { .pattern = "*STB?", .callback = SCPI_CoreStbQ,},
//    { .pattern = "*TST?", .callback = SCPI_CoreTstQ,},
//    { .pattern = "*WAI", .callback = SCPI_CoreWai,},

    /* Required SCPI commands (SCPI std V1999.0 4.2.1) */
//    {.pattern = "SYSTem:ERRor[:NEXT]?", .callback = SCPI_SystemErrorNextQ,},
//    {.pattern = "SYSTem:ERRor:COUNt?", .callback = SCPI_SystemErrorCountQ,},
//    {.pattern = "SYSTem:VERSion?", .callback = SCPI_SystemVersionQ,},

//    {.pattern = "STATus:QUEStionable[:EVENt]?", .callback = SCPI_StatusQuestionableEventQ,},
//    {.pattern = "STATus:QUEStionable:ENABle", .callback = SCPI_StatusQuestionableEnable,},
//    {.pattern = "STATus:QUEStionable:ENABle?", .callback = SCPI_StatusQuestionableEnableQ,},

//    {.pattern = "STATus:PRESet", .callback = SCPI_StatusPreset,},

    SCPI_CMD_LIST_END
};

size_t SCPI_Write(scpi_t * context, const char * data, size_t len)
{
    (void)context;
    (void)data;
    (void)len;    
}

int SCPI_Error(scpi_t * context, int_fast16_t err)
{
    (void)context;
    (void)err;    
}

scpi_result_t SCPI_Control(scpi_t * context, scpi_ctrl_name_t ctrl, scpi_reg_val_t val)
{
    (void)context;
    (void)ctrl;
    (void)val;    
}

scpi_result_t SCPI_Reset(scpi_t * context)
{
    (void)context;
}

scpi_result_t SCPI_Test(scpi_t * context)
{
    (void)context;   
}

scpi_result_t SCPI_Flush(scpi_t * context)
{
    (void)context;
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
    .idn = {"MANUFACTURE", "INSTR2013", NULL, "01-02"},
};

int init_suite(void) {
    SCPI_Init(&scpi_context);

    return 0;
}

int clean_suite(void) {
    return 0;
}


#define TEST_ParamInt(data, mandatory, expected_value, expected_result, expected_error_code) \
{                                                                                       \
    int32_t value;                                                                      \
    scpi_bool_t result;                                                                 \
    int16_t errCode;                                                                    \
                                                                                        \
    SCPI_CoreCls(&scpi_context);                                                        \
    scpi_context.input_count = 0;                                                       \
    scpi_context.param_list.lex_state.buffer = data;                                    \
    scpi_context.param_list.lex_state.len = strlen(scpi_context.param_list.lex_state.buffer);\
    scpi_context.param_list.lex_state.pos = scpi_context.param_list.lex_state.buffer;   \
    result = SCPI_ParamInt(&scpi_context, &value, mandatory);                           \
                                                                                        \
    errCode = SCPI_ErrorPop(&scpi_context);                                             \
    CU_ASSERT_EQUAL(result, expected_result);                                           \
    if (expected_result) {                                                              \
        CU_ASSERT_EQUAL(value, expected_value);                                         \
    }                                                                                   \
    CU_ASSERT_EQUAL(errCode, expected_error_code);                                      \
}

void testSCPI_ParamInt(void) {
    TEST_ParamInt("10", TRUE, 10, TRUE, 0);
    TEST_ParamInt("", FALSE, 0, FALSE, 0);
    TEST_ParamInt("10.5", TRUE, 10, TRUE, 0); // TODO: should be FALSE, -104
    TEST_ParamInt("#B101010", TRUE, 42, TRUE, 0);
    TEST_ParamInt("#H101010", TRUE, 1052688, TRUE, 0);
    TEST_ParamInt("#Q10", TRUE, 8, TRUE, 0);

    TEST_ParamInt("", TRUE, 0, FALSE, -109); // missing parameter
    TEST_ParamInt("abcd", TRUE, 0, FALSE, -104); // Data type error
    TEST_ParamInt("10.5V", TRUE, 0, FALSE, -138);
    TEST_ParamInt("10V", TRUE, 0, FALSE, -138);
}

#define TEST_ParamDouble(data, mandatory, expected_value, expected_result, expected_error_code) \
{                                                                                       \
    double value;                                                                       \
    scpi_bool_t result;                                                                 \
    int16_t errCode;                                                                    \
                                                                                        \
    SCPI_CoreCls(&scpi_context);                                                        \
    scpi_context.input_count = 0;                                                       \
    scpi_context.param_list.lex_state.buffer = data;                                    \
    scpi_context.param_list.lex_state.len = strlen(scpi_context.param_list.lex_state.buffer);\
    scpi_context.param_list.lex_state.pos = scpi_context.param_list.lex_state.buffer;   \
    result = SCPI_ParamDouble(&scpi_context, &value, mandatory);                        \
                                                                                        \
    errCode = SCPI_ErrorPop(&scpi_context);                                             \
    CU_ASSERT_EQUAL(result, expected_result);                                           \
    if (expected_result) {                                                              \
        CU_ASSERT_DOUBLE_EQUAL(value, expected_value, 0.000001);                        \
    }                                                                                   \
    CU_ASSERT_EQUAL(errCode, expected_error_code);                                      \
}

void testSCPI_ParamDouble(void) {
    TEST_ParamDouble("10", TRUE, 10, TRUE, 0);
    TEST_ParamDouble("", FALSE, 0, FALSE, 0);
    TEST_ParamDouble("10.5", TRUE, 10.5, TRUE, 0);
    TEST_ParamDouble("#B101010", TRUE, 42, TRUE, 0);
    TEST_ParamDouble("#H101010", TRUE, 1052688, TRUE, 0);
    TEST_ParamDouble("#Q10", TRUE, 8, TRUE, 0);

    TEST_ParamDouble("", TRUE, 0, FALSE, -109); // missing parameter
    TEST_ParamDouble("abcd", TRUE, 0, FALSE, -104); // Data type error
    TEST_ParamDouble("10.5V", TRUE, 0, FALSE, -138);
    TEST_ParamDouble("10V", TRUE, 0, FALSE, -138);
}

#define TEST_ParamCharacters(data, mandatory, expected_value, expected_result, expected_error_code) \
{                                                                                       \
    const char * value;                                                                 \
    size_t value_len;                                                                   \
    scpi_bool_t result;                                                                 \
    int16_t errCode;                                                                    \
                                                                                        \
    SCPI_CoreCls(&scpi_context);                                                        \
    scpi_context.input_count = 0;                                                       \
    scpi_context.param_list.lex_state.buffer = data;                                    \
    scpi_context.param_list.lex_state.len = strlen(scpi_context.param_list.lex_state.buffer);\
    scpi_context.param_list.lex_state.pos = scpi_context.param_list.lex_state.buffer;   \
    result = SCPI_ParamCharacters(&scpi_context, &value, &value_len, mandatory);        \
    /*printf("%.*s\r\n",  (int)value_len, value);*/                                     \
    errCode = SCPI_ErrorPop(&scpi_context);                                             \
    CU_ASSERT_EQUAL(result, expected_result);                                           \
    if (expected_result) {                                                              \
        CU_ASSERT_NSTRING_EQUAL(value, expected_value, value_len);                      \
    }                                                                                   \
    CU_ASSERT_EQUAL(errCode, expected_error_code);                                      \
}

void testSCPI_ParamCharacters(void) {
    TEST_ParamCharacters("10", TRUE, "10", TRUE, 0);
    TEST_ParamCharacters(" ABCD", TRUE, "ABCD", TRUE, 0); // TokProgramMnemonic
    TEST_ParamCharacters("\"ABCD\"", TRUE, "ABCD", TRUE, 0); // TokDoubleQuoteProgramData
    TEST_ParamCharacters("\'ABCD\'", TRUE, "ABCD", TRUE, 0); // TokSingleQuoteProgramData
    TEST_ParamCharacters("#204ABCD", TRUE, "ABCD", TRUE, 0); // TokArbitraryBlockProgramData
    TEST_ParamCharacters("#210ABCD", TRUE, "", FALSE, -151); // invalid Block Data
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
    if ((NULL == CU_add_test(pSuite, "SCPI_ParamInt", testSCPI_ParamInt))
            || (NULL == CU_add_test(pSuite, "SCPI_ParamDouble", testSCPI_ParamDouble))
            || (NULL == CU_add_test(pSuite, "SCPI_ParamCharacters", testSCPI_ParamCharacters))
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

