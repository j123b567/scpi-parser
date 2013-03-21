/*
 * File:   test_lexer.c
 * Author: jaybee
 *
 * Created on Thu Mar 21 14:39:03 UTC 2013
 */

#include <stdio.h>
#include <stdlib.h>
#include "CUnit/Basic.h"

#include "scpi/lexer.h"

/*
 * CUnit Test Suite
 */

int init_suite(void) {
    return 0;
}

int clean_suite(void) {
    return 0;
}

typedef int (*lexfn_t)(lex_state_t * state, token_t * token);


const char * typeToStr(token_type_t type) {
    switch(type) {
        case TokComma: return "TokComma";
        case TokSemicolon: return "TokSemicolon";
        case TokQuiestion: return "TokQuiestion";
        case TokNewLine: return "TokNewLine";
        case TokHexnum: return "TokHexnum";
        case TokOctnum: return "TokOctnum";
        case TokBinnum: return "TokBinnum";
        case TokProgramMnemonic: return "TokProgramMnemonic";
        case TokDecimalNumericProgramData: return "TokDecimalNumericProgramData";
        case TokSuffixProgramData: return "TokSuffixProgramData";
        case TokArbitraryBlockProgramData: return "TokArbitraryBlockProgramData";
        case TokSingleQuoteProgramData: return "TokSingleQuoteProgramData";
        case TokDoubleQuoteProgramData: return "TokDoubleQuoteProgramData";
        case TokProgramExpression: return "TokProgramExpression";
        case TokCompoundProgramHeader: return "TokCompoundProgramHeader";
        case TokCommonProgramHeader: return "TokCommonProgramHeader";
        case TokWhiteSpace: return "TokWhiteSpace";
        default: return "TokUnknown";
    }
}

void printToken(token_t * token) {
    printf("Token:\r\n");
    printf("\t->type = %s\r\n", typeToStr(token->type));
    printf("\t->ptr = %p (\"%.*s\")\r\n", token->ptr, token->len, token->ptr);
    printf("\t->len = %d\r\n", token->len);
}


#if 0
static void TEST_TOKEN(const char * str, lexfn_t fn, int offset, int len, token_type_t tp)  {
    lex_state_t state;
    token_t token;
    
    state.buffer = state.pos = str;
    state.len = strlen(str);
    fn(&state, &token);
    CU_ASSERT_EQUAL(str + offset, token.ptr);
    CU_ASSERT_EQUAL(len, token.len);
    CU_ASSERT_EQUAL(tp, token.type);
}
#endif

#define TEST_TOKEN(s, f, o, l, t) do {          \
    const char * str = s;                       \
    lexfn_t fn = f;                             \
    int offset = o;                             \
    int len = l;                                \
    token_type_t tp = t;                        \
    lex_state_t state;                          \
    token_t token;                              \
                                                \
    state.buffer = state.pos = str;             \
    state.len = strlen(str);                    \
    fn(&state, &token);                         \
    CU_ASSERT_EQUAL(str + offset, token.ptr);   \
    CU_ASSERT_EQUAL(len, token.len);            \
    CU_ASSERT_EQUAL(tp, token.type);            \
    /* printToken(&token); */                   \
} while(0)


void testWhiteSpace(void) {
    TEST_TOKEN("  \t MEA", SCPI_LexWhiteSpace, 0, 4, TokWhiteSpace);
}

void testNondecimal(void) {
    TEST_TOKEN("#H123fe5A", SCPI_LexNondecimalNumericData, 2, 7, TokHexnum);
    TEST_TOKEN("#B0111010101", SCPI_LexNondecimalNumericData, 2, 10, TokBinnum);
    TEST_TOKEN("#Q125725433", SCPI_LexNondecimalNumericData, 2, 9, TokOctnum);
}

void testCharacterProgramData(void) {
    TEST_TOKEN("abc_213as564 , ", SCPI_LexCharacterProgramData, 0, 12, TokProgramMnemonic);
}

void testDecimal(void) {
    TEST_TOKEN("10 , ", SCPI_LexDecimalNumericProgramData, 0, 2, TokDecimalNumericProgramData);
    TEST_TOKEN("-10.5 , ", SCPI_LexDecimalNumericProgramData, 0, 5, TokDecimalNumericProgramData);
    TEST_TOKEN("+.5 , ", SCPI_LexDecimalNumericProgramData, 0, 3, TokDecimalNumericProgramData);
    TEST_TOKEN("-. , ", SCPI_LexDecimalNumericProgramData, 0, 0, TokUnknown);
    TEST_TOKEN("-1 e , ", SCPI_LexDecimalNumericProgramData, 0, 2, TokDecimalNumericProgramData);
    TEST_TOKEN("-1 e 3, ", SCPI_LexDecimalNumericProgramData, 0, 6, TokDecimalNumericProgramData);
    TEST_TOKEN("1.5E12", SCPI_LexDecimalNumericProgramData, 0, 6, TokDecimalNumericProgramData);
}

void testSuffix(void) {
    TEST_TOKEN("A/V , ", SCPI_LexSuffixProgramData, 0, 3, TokSuffixProgramData);
    TEST_TOKEN("mA.h", SCPI_LexSuffixProgramData, 0, 4, TokSuffixProgramData);
}

void testProgramHeader(void) {
    TEST_TOKEN("*IDN? ", SCPI_LexCommonProgramHeader, 0, 4, TokCommonProgramHeader);
    TEST_TOKEN("*?; ", SCPI_LexCommonProgramHeader, 0, 0, TokUnknown);
    TEST_TOKEN("MEAS:VOLT:DC? ", SCPI_LexCommonProgramHeader, 0, 0, TokUnknown);
    TEST_TOKEN("MEAS:VOLT:DC? ", SCPI_LexCompoundProgramHeader, 0, 12, TokCompoundProgramHeader);
    TEST_TOKEN(":MEAS:VOLT:DC? ", SCPI_LexCompoundProgramHeader, 0, 13, TokCompoundProgramHeader);
    TEST_TOKEN(":MEAS::VOLT:DC? ", SCPI_LexCompoundProgramHeader, 0, 6, TokCompoundProgramHeader);
    TEST_TOKEN(":MEAS::VOLT:DC? ", SCPI_LexProgramHeader, 0, 6, TokCompoundProgramHeader);
    TEST_TOKEN("*IDN?", SCPI_LexProgramHeader, 0, 4, TokCommonProgramHeader);
}

void testArbitraryBlock(void) {
    TEST_TOKEN("#12AB, ", SCPI_LexArbitraryBlockProgramData, 3, 2, TokArbitraryBlockProgramData);
    TEST_TOKEN("#13AB", SCPI_LexArbitraryBlockProgramData, 0, 0, TokUnknown);
    TEST_TOKEN("#12\r\n, ", SCPI_LexArbitraryBlockProgramData, 3, 2, TokArbitraryBlockProgramData);
    TEST_TOKEN("#02AB, ", SCPI_LexArbitraryBlockProgramData, 0, 0, TokUnknown);
}

void testExpression(void) {
    TEST_TOKEN("( 1 + 2 ) , ", SCPI_LexProgramExpression, 0, 9, TokProgramExpression);
    TEST_TOKEN("( 1 + 2  , ", SCPI_LexProgramExpression, 0, 0, TokUnknown);
}

void testString(void) {
    TEST_TOKEN("\"ahoj\" ", SCPI_LexStringProgramData, 1, 4, TokDoubleQuoteProgramData);
    TEST_TOKEN("'ahoj' ", SCPI_LexStringProgramData, 1, 4, TokSingleQuoteProgramData);
    TEST_TOKEN("'ahoj ", SCPI_LexStringProgramData, 0, 0, TokUnknown);
    TEST_TOKEN("'ah''oj' ", SCPI_LexStringProgramData, 1, 6, TokSingleQuoteProgramData);
    TEST_TOKEN("'ah\"oj' ", SCPI_LexStringProgramData, 1, 5, TokSingleQuoteProgramData);
    TEST_TOKEN("\"ah\"\"oj\" ", SCPI_LexStringProgramData, 1, 6, TokDoubleQuoteProgramData);
}


int main() {
    CU_pSuite pSuite = NULL;
    
    /* Initialize the CUnit test registry */
    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();
    
    /* Add a suite to the registry */
    pSuite = CU_add_suite("Lexer", init_suite, clean_suite);
    if (NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    
    /* Add the tests to the suite */
    if ((NULL == CU_add_test(pSuite, "WhiteSpace", testWhiteSpace))
        || (NULL == CU_add_test(pSuite, "Nondecimal", testNondecimal))
        || (NULL == CU_add_test(pSuite, "CharacterProgramData", testCharacterProgramData))
        || (NULL == CU_add_test(pSuite, "Decimal", testDecimal))
        || (NULL == CU_add_test(pSuite, "Suffix", testSuffix))
        || (NULL == CU_add_test(pSuite, "ProgramHeader", testProgramHeader))
        || (NULL == CU_add_test(pSuite, "ArbitraryBlock", testArbitraryBlock))
        || (NULL == CU_add_test(pSuite, "Expression", testExpression))
        || (NULL == CU_add_test(pSuite, "String", testString))
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

