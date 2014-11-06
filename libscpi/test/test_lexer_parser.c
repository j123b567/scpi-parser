/*
 * File:   test_lexer.c
 * Author: jaybee
 *
 * Created on Thu Mar 21 14:39:03 UTC 2013
 */

#include <stdio.h>
#include <stdlib.h>
#include "CUnit/Basic.h"

#include "../src/lexer_private.h"
#include "scpi/parser.h"
#include "../src/parser_private.h"

/*
 * CUnit Test Suite
 */

int init_suite(void) {
    return 0;
}

int clean_suite(void) {
    return 0;
}

typedef int (*lexfn_t)(lex_state_t * state, scpi_token_t * token);
typedef int (*lexfn2_t)(lex_state_t * state, scpi_token_t * token, int * cnt);

const char * typeToStr(scpi_token_type_t type) {
    switch (type) {
        case TokComma: return "TokComma";
        case TokSemicolon: return "TokSemicolon";
        case TokQuiestion: return "TokQuiestion";
        case TokNewLine: return "TokNewLine";
        case TokHexnum: return "TokHexnum";
        case TokOctnum: return "TokOctnum";
        case TokBinnum: return "TokBinnum";
        case TokProgramMnemonic: return "TokProgramMnemonic";
        case TokDecimalNumericProgramData: return "TokDecimalNumericProgramData";
        case TokDecimalNumericProgramDataWithSuffix: return "TokDecimalNumericProgramDataWithSuffix";
        case TokSuffixProgramData: return "TokSuffixProgramData";
        case TokArbitraryBlockProgramData: return "TokArbitraryBlockProgramData";
        case TokSingleQuoteProgramData: return "TokSingleQuoteProgramData";
        case TokDoubleQuoteProgramData: return "TokDoubleQuoteProgramData";
        case TokProgramExpression: return "TokProgramExpression";
        case TokCompoundProgramHeader: return "TokCompoundProgramHeader";
        case TokCommonProgramHeader: return "TokCommonProgramHeader";
        case TokCompoundQueryProgramHeader: return "TokCompoundQueryProgramHeader";
        case TokCommonQueryProgramHeader: return "TokCommonQueryProgramHeader";
        case TokWhiteSpace: return "TokWhiteSpace";
        case TokAllProgramData: return "TokAllProgramData";
        case TokIncompleteCompoundProgramHeader: return "TokIncompleteCompoundProgramHeader";
        case TokIncompleteCommonProgramHeader: return "TokIncompleteCommonProgramHeader";
        case TokInvalid: return "TokInvalid";
        default: return "TokUnknown";
    }
}

void printToken(scpi_token_t * token) {
    printf("Token:\r\n");
    printf("\t->type = %s\r\n", typeToStr(token->type));
    printf("\t->ptr = %p (\"%.*s\")\r\n", token->ptr, token->len, token->ptr);
    printf("\t->len = %d\r\n", token->len);
}


#if 0

static void TEST_TOKEN(const char * str, lexfn_t fn, int offset, int len, scpi_token_type_t tp) {
    lex_state_t state;
    scpi_token_t token;

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
    scpi_token_type_t tp = t;                        \
    lex_state_t state;                          \
    scpi_token_t token;                              \
                                                \
    state.buffer = state.pos = str;             \
    state.len = strlen(str);                    \
    fn(&state, &token);                         \
    CU_ASSERT_EQUAL(str + offset, token.ptr);   \
    CU_ASSERT_EQUAL(len, token.len);            \
    CU_ASSERT_EQUAL(tp, token.type);            \
    if (tp != token.type) printToken(&token);   \
    else                                        \
    if (len != token.len) printToken(&token);   \
} while(0)


void testWhiteSpace(void) {
    TEST_TOKEN("  \t MEAS", scpiLex_WhiteSpace, 0, 4, TokWhiteSpace);
    TEST_TOKEN("MEAS", scpiLex_WhiteSpace, 0, 0, TokUnknown);
}

void testNondecimal(void) {
    TEST_TOKEN("#H123fe5A", scpiLex_NondecimalNumericData, 2, 7, TokHexnum);
    TEST_TOKEN("#B0111010101", scpiLex_NondecimalNumericData, 2, 10, TokBinnum);
    TEST_TOKEN("#Q125725433", scpiLex_NondecimalNumericData, 2, 9, TokOctnum);
}

void testCharacterProgramData(void) {
    TEST_TOKEN("abc_213as564", scpiLex_CharacterProgramData, 0, 12, TokProgramMnemonic);
    TEST_TOKEN("abc_213as564 , ", scpiLex_CharacterProgramData, 0, 12, TokProgramMnemonic);
}

void testDecimal(void) {
    TEST_TOKEN("10", scpiLex_DecimalNumericProgramData, 0, 2, TokDecimalNumericProgramData);
    TEST_TOKEN("10 , ", scpiLex_DecimalNumericProgramData, 0, 2, TokDecimalNumericProgramData);
    TEST_TOKEN("-10.5 , ", scpiLex_DecimalNumericProgramData, 0, 5, TokDecimalNumericProgramData);
    TEST_TOKEN("+.5 , ", scpiLex_DecimalNumericProgramData, 0, 3, TokDecimalNumericProgramData);
    TEST_TOKEN("-. , ", scpiLex_DecimalNumericProgramData, 0, 0, TokUnknown);
    TEST_TOKEN("-1 e , ", scpiLex_DecimalNumericProgramData, 0, 2, TokDecimalNumericProgramData);
    TEST_TOKEN("-1 e 3, ", scpiLex_DecimalNumericProgramData, 0, 6, TokDecimalNumericProgramData);
    TEST_TOKEN("1.5E12", scpiLex_DecimalNumericProgramData, 0, 6, TokDecimalNumericProgramData);
}

void testSuffix(void) {
    TEST_TOKEN("A/V , ", scpiLex_SuffixProgramData, 0, 3, TokSuffixProgramData);
    TEST_TOKEN("mA.h", scpiLex_SuffixProgramData, 0, 4, TokSuffixProgramData);
}

void testProgramHeader(void) {
    TEST_TOKEN("*IDN? ", scpiLex_ProgramHeader, 0, 5, TokCommonQueryProgramHeader);
    TEST_TOKEN("*RST ", scpiLex_ProgramHeader, 0, 4, TokCommonProgramHeader);
    TEST_TOKEN("*?; ", scpiLex_ProgramHeader, 0, 1, TokIncompleteCommonProgramHeader);
    TEST_TOKEN(":*IDN?; ", scpiLex_ProgramHeader, 0, 1, TokIncompleteCompoundProgramHeader);
    TEST_TOKEN("MEAS:VOLT:DC? ", scpiLex_ProgramHeader, 0, 13, TokCompoundQueryProgramHeader);
    TEST_TOKEN("CONF:VOLT:DC ", scpiLex_ProgramHeader, 0, 12, TokCompoundProgramHeader);
    TEST_TOKEN(":MEAS:VOLT:DC? ", scpiLex_ProgramHeader, 0, 14, TokCompoundQueryProgramHeader);
    TEST_TOKEN(":MEAS::VOLT:DC? ", scpiLex_ProgramHeader, 0, 6, TokIncompleteCompoundProgramHeader);
    TEST_TOKEN("*IDN?", scpiLex_ProgramHeader, 0, 5, TokCommonQueryProgramHeader);
    TEST_TOKEN("*RST", scpiLex_ProgramHeader, 0, 4, TokCommonProgramHeader);
    TEST_TOKEN("CONF:VOLT:DC", scpiLex_ProgramHeader, 0, 12, TokCompoundProgramHeader);
    TEST_TOKEN("]]", scpiLex_ProgramHeader, 0, 0, TokUnknown);
}

void testArbitraryBlock(void) {
    TEST_TOKEN("#12AB", scpiLex_ArbitraryBlockProgramData, 3, 2, TokArbitraryBlockProgramData);
    TEST_TOKEN("#12AB, ", scpiLex_ArbitraryBlockProgramData, 3, 2, TokArbitraryBlockProgramData);
    TEST_TOKEN("#13AB", scpiLex_ArbitraryBlockProgramData, 0, 0, TokUnknown);
    TEST_TOKEN("#12\r\n, ", scpiLex_ArbitraryBlockProgramData, 3, 2, TokArbitraryBlockProgramData);
    TEST_TOKEN("#02AB, ", scpiLex_ArbitraryBlockProgramData, 0, 0, TokUnknown);
}

void testExpression(void) {
    TEST_TOKEN("( 1 + 2 )", scpiLex_ProgramExpression, 0, 9, TokProgramExpression);
    TEST_TOKEN("( 1 + 2 ) , ", scpiLex_ProgramExpression, 0, 9, TokProgramExpression);
    TEST_TOKEN("( 1 + 2  , ", scpiLex_ProgramExpression, 0, 0, TokUnknown);
}

void testString(void) {
    TEST_TOKEN("\"ahoj\"", scpiLex_StringProgramData, 1, 4, TokDoubleQuoteProgramData);
    TEST_TOKEN("\"ahoj\" ", scpiLex_StringProgramData, 1, 4, TokDoubleQuoteProgramData);
    TEST_TOKEN("'ahoj' ", scpiLex_StringProgramData, 1, 4, TokSingleQuoteProgramData);
    TEST_TOKEN("'ahoj ", scpiLex_StringProgramData, 0, 0, TokUnknown);
    TEST_TOKEN("'ah''oj' ", scpiLex_StringProgramData, 1, 6, TokSingleQuoteProgramData);
    TEST_TOKEN("'ah\"oj' ", scpiLex_StringProgramData, 1, 5, TokSingleQuoteProgramData);
    TEST_TOKEN("\"ah\"\"oj\" ", scpiLex_StringProgramData, 1, 6, TokDoubleQuoteProgramData);
}

void testProgramData(void) {
    TEST_TOKEN("#H123fe5A", scpiParser_parseProgramData, 2, 7, TokHexnum);
    TEST_TOKEN("  #H123fe5A ", scpiParser_parseProgramData, 4, 7, TokHexnum);
    TEST_TOKEN("#B0111010101", scpiParser_parseProgramData, 2, 10, TokBinnum);
    TEST_TOKEN("#Q125725433", scpiParser_parseProgramData, 2, 9, TokOctnum);

    TEST_TOKEN("10", scpiParser_parseProgramData, 0, 2, TokDecimalNumericProgramData);
    TEST_TOKEN("10 , ", scpiParser_parseProgramData, 0, 2, TokDecimalNumericProgramData);
    TEST_TOKEN("-10.5 , ", scpiParser_parseProgramData, 0, 5, TokDecimalNumericProgramData);
    TEST_TOKEN("+.5 , ", scpiParser_parseProgramData, 0, 3, TokDecimalNumericProgramData);
    TEST_TOKEN("-. , ", scpiParser_parseProgramData, 0, 0, TokUnknown);
    TEST_TOKEN("-1 e , ", scpiParser_parseProgramData, 0, 4, TokDecimalNumericProgramDataWithSuffix);
    TEST_TOKEN("-1 e 3, ", scpiParser_parseProgramData, 0, 6, TokDecimalNumericProgramData);
    TEST_TOKEN("1.5E12", scpiParser_parseProgramData, 0, 6, TokDecimalNumericProgramData);

    TEST_TOKEN("#12AB", scpiParser_parseProgramData, 3, 2, TokArbitraryBlockProgramData);
    TEST_TOKEN("#12AB, ", scpiParser_parseProgramData, 3, 2, TokArbitraryBlockProgramData);
    TEST_TOKEN("#13AB", scpiParser_parseProgramData, 0, 0, TokUnknown);
    TEST_TOKEN("#12\r\n, ", scpiParser_parseProgramData, 3, 2, TokArbitraryBlockProgramData);
    TEST_TOKEN("#02AB, ", scpiParser_parseProgramData, 0, 0, TokUnknown);

    TEST_TOKEN("( 1 + 2 ) , ", scpiParser_parseProgramData, 0, 9, TokProgramExpression);
    TEST_TOKEN("( 1 + 2  , ", scpiParser_parseProgramData, 0, 0, TokUnknown);

    TEST_TOKEN("\"ahoj\" ", scpiParser_parseProgramData, 1, 4, TokDoubleQuoteProgramData);
    TEST_TOKEN("'ahoj' ", scpiParser_parseProgramData, 1, 4, TokSingleQuoteProgramData);
    TEST_TOKEN("'ahoj ", scpiParser_parseProgramData, 0, 0, TokUnknown);
    TEST_TOKEN("'ah''oj' ", scpiParser_parseProgramData, 1, 6, TokSingleQuoteProgramData);
    TEST_TOKEN("'ah\"oj' ", scpiParser_parseProgramData, 1, 5, TokSingleQuoteProgramData);
    TEST_TOKEN("\"ah\"\"oj\" ", scpiParser_parseProgramData, 1, 6, TokDoubleQuoteProgramData);
    
    TEST_TOKEN("abc_213as564 , ", scpiLex_CharacterProgramData, 0, 12, TokProgramMnemonic);
    
    TEST_TOKEN("1.5E12 V", scpiParser_parseProgramData, 0, 8, TokDecimalNumericProgramDataWithSuffix);
}


#define TEST_ALL_TOKEN(s, f, o, l, t, c) do {   \
    const char * str = s;                       \
    lexfn2_t fn = f;                            \
    int offset = o;                             \
    int len = l;                                \
    scpi_token_type_t tp = t;                        \
    lex_state_t state;                          \
    scpi_token_t token;                              \
    int count;                                  \
                                                \
    state.buffer = state.pos = str;             \
    state.len = strlen(str);                    \
    fn(&state, &token, &count);                 \
    CU_ASSERT_EQUAL(str + offset, token.ptr);   \
    CU_ASSERT_EQUAL(len, token.len);            \
    CU_ASSERT_EQUAL(tp, token.type);            \
    CU_ASSERT_EQUAL(count, c);                  \
    if (tp != token.type) printToken(&token);   \
    else                                        \
    if (len != token.len) printToken(&token);   \
} while(0)


void testAllProgramData(void) {
    TEST_ALL_TOKEN("1.5E12 V", scpiParser_parseAllProgramData, 0, 8, TokAllProgramData, 1);
    TEST_ALL_TOKEN("1.5E12 V, abc_213as564, 10, #H123fe5A", scpiParser_parseAllProgramData, 0, 37, TokAllProgramData, 4);
    TEST_ALL_TOKEN("1.5E12 V, ", scpiParser_parseAllProgramData, 0, 0, TokUnknown, -1);
    TEST_ALL_TOKEN("#12\r\n, 1.5E12 V", scpiParser_parseAllProgramData, 0, 15, TokAllProgramData, 2);
    TEST_ALL_TOKEN(" ( 1 + 2 ) ,#12\r\n, 1.5E12 V", scpiParser_parseAllProgramData, 0, 27, TokAllProgramData, 3);
    TEST_ALL_TOKEN("\"ahoj\" , #12AB", scpiParser_parseAllProgramData, 0, 14, TokAllProgramData, 2);
}


#define TEST_DETECT(s, h, hl, ht, d, dc, t) do {                                \
    const char * str = s;                                                       \
    scpi_parser_state_t state;                                                  \
    int result;                                                                 \
    result = scpiParser_detectProgramMessageUnit(&state, str, strlen(str));           \
    CU_ASSERT_EQUAL(state.programHeader.ptr, str + h);                          \
    CU_ASSERT_EQUAL(state.programHeader.len, hl);                               \
    CU_ASSERT_EQUAL(state.programHeader.type, ht);                              \
    CU_ASSERT_EQUAL(state.programData.ptr, str + d);                            \
    CU_ASSERT_EQUAL(state.numberOfParameters, dc);                              \
    CU_ASSERT_EQUAL(state.termination, t);                                      \
} while(0)

void testDetectProgramMessageUnit(void) {
    TEST_DETECT("*IDN?\r\n", 0, 5, TokCommonQueryProgramHeader, 5, 0, PmutNewLine);
    TEST_DETECT(" MEAS:VOLT:DC?\r\n", 1, 13, TokCompoundQueryProgramHeader, 14, 0, PmutNewLine);
    TEST_DETECT(" MEAS:VOLT:DC? 1.2 V\r\n", 1, 13, TokCompoundQueryProgramHeader, 15, 1, PmutNewLine);
    TEST_DETECT(" CONF:VOLT:DC 1.2 V, 100mv;", 1, 12, TokCompoundProgramHeader, 14, 2, PmutSemicolon);
    TEST_DETECT(" CONF:VOLT:DC 1.2 V, 100mv", 1, 12, TokCompoundProgramHeader, 14, 2, PmutNone);
    TEST_DETECT(" CONF:VOLT:DC 1.2 V, \r\n", 1, 12, TokCompoundProgramHeader, 14, -1, PmutNewLine);
    TEST_DETECT("[\r\n", 0, 1, TokInvalid, 0, 0, PmutNone);
}

void testBoolParameter(void) {
    TEST_TOKEN(" 1", scpiParser_parseProgramData, 1, 1, TokDecimalNumericProgramData);
    TEST_TOKEN(" 0", scpiParser_parseProgramData, 1, 1, TokDecimalNumericProgramData);
    TEST_TOKEN(" ON", scpiParser_parseProgramData, 1, 2, TokProgramMnemonic);
    TEST_TOKEN("OFF ", scpiParser_parseProgramData, 0, 3, TokProgramMnemonic);
    
    // TODO: finish bool test
}

// TODO: SCPI_Parameter test

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
            || (NULL == CU_add_test(pSuite, "ProgramData", testProgramData))
            || (NULL == CU_add_test(pSuite, "AllProgramData", testAllProgramData))
            || (NULL == CU_add_test(pSuite, "DetectProgramMessageUnit", testDetectProgramMessageUnit))
            || (NULL == CU_add_test(pSuite, "BoolParameter", testBoolParameter))
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

