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

#include "../src/lexer_private.h"
#include "scpi/parser.h"
#include "../src/parser_private.h"

/*
 * CUnit Test Suite
 */

static int init_suite(void) {
    return 0;
}

static int clean_suite(void) {
    return 0;
}

typedef int (*lexfn_t)(lex_state_t * state, scpi_token_t * token);
typedef int (*lexfn2_t)(lex_state_t * state, scpi_token_t * token, int * cnt);

static const char * typeToStr(scpi_token_type_t type) {
    switch (type) {
        case SCPI_TOKEN_COMMA: return "TokComma";
        case SCPI_TOKEN_SEMICOLON: return "TokSemicolon";
        case SCPI_TOKEN_QUESTION: return "TokQuiestion";
        case SCPI_TOKEN_NL: return "TokNewLine";
        case SCPI_TOKEN_HEXNUM: return "TokHexnum";
        case SCPI_TOKEN_OCTNUM: return "TokOctnum";
        case SCPI_TOKEN_BINNUM: return "TokBinnum";
        case SCPI_TOKEN_PROGRAM_MNEMONIC: return "TokProgramMnemonic";
        case SCPI_TOKEN_DECIMAL_NUMERIC_PROGRAM_DATA: return "TokDecimalNumericProgramData";
        case SCPI_TOKEN_DECIMAL_NUMERIC_PROGRAM_DATA_WITH_SUFFIX: return "TokDecimalNumericProgramDataWithSuffix";
        case SCPI_TOKEN_SUFFIX_PROGRAM_DATA: return "TokSuffixProgramData";
        case SCPI_TOKEN_ARBITRARY_BLOCK_PROGRAM_DATA: return "TokArbitraryBlockProgramData";
        case SCPI_TOKEN_SINGLE_QUOTE_PROGRAM_DATA: return "TokSingleQuoteProgramData";
        case SCPI_TOKEN_DOUBLE_QUOTE_PROGRAM_DATA: return "TokDoubleQuoteProgramData";
        case SCPI_TOKEN_PROGRAM_EXPRESSION: return "TokProgramExpression";
        case SCPI_TOKEN_COMPOUND_PROGRAM_HEADER: return "TokCompoundProgramHeader";
        case SCPI_TOKEN_COMMON_PROGRAM_HEADER: return "TokCommonProgramHeader";
        case SCPI_TOKEN_COMPOUND_QUERY_PROGRAM_HEADER: return "TokCompoundQueryProgramHeader";
        case SCPI_TOKEN_COMMON_QUERY_PROGRAM_HEADER: return "TokCommonQueryProgramHeader";
        case SCPI_TOKEN_WS: return "TokWhiteSpace";
        case SCPI_TOKEN_ALL_PROGRAM_DATA: return "TokAllProgramData";
        case SCPI_TOKEN_INCOMPLETE_COMPOUND_PROGRAM_HEADER: return "TokIncompleteCompoundProgramHeader";
        case SCPI_TOKEN_INCOMPLETE_COMMON_PROGRAM_HEADER: return "TokIncompleteCommonProgramHeader";
        case SCPI_TOKEN_INVALID: return "TokInvalid";
        default: return "TokUnknown";
    }
}

static void printToken(scpi_token_t * token) {
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
    char * str = s;                             \
    lexfn_t fn = f;                             \
    int offset = o;                             \
    int len = l;                                \
    scpi_token_type_t tp = t;                   \
    lex_state_t state;                          \
    scpi_token_t token;                         \
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

static void testWhiteSpace(void) {
    TEST_TOKEN("  \t MEAS", scpiLex_WhiteSpace, 0, 4, SCPI_TOKEN_WS);
    TEST_TOKEN("MEAS", scpiLex_WhiteSpace, 0, 0, SCPI_TOKEN_UNKNOWN);
}

static void testNondecimal(void) {
    TEST_TOKEN("#H123fe5A", scpiLex_NondecimalNumericData, 2, 7, SCPI_TOKEN_HEXNUM);
    TEST_TOKEN("#B0111010101", scpiLex_NondecimalNumericData, 2, 10, SCPI_TOKEN_BINNUM);
    TEST_TOKEN("#Q125725433", scpiLex_NondecimalNumericData, 2, 9, SCPI_TOKEN_OCTNUM);
    TEST_TOKEN("#H123fe5A,", scpiLex_NondecimalNumericData, 2, 7, SCPI_TOKEN_HEXNUM);
    TEST_TOKEN("#B0111010101,", scpiLex_NondecimalNumericData, 2, 10, SCPI_TOKEN_BINNUM);
    TEST_TOKEN("#Q125725433,", scpiLex_NondecimalNumericData, 2, 9, SCPI_TOKEN_OCTNUM);
}

static void testCharacterProgramData(void) {
    TEST_TOKEN("abc_213as564", scpiLex_CharacterProgramData, 0, 12, SCPI_TOKEN_PROGRAM_MNEMONIC);
    TEST_TOKEN("abc_213as564 , ", scpiLex_CharacterProgramData, 0, 12, SCPI_TOKEN_PROGRAM_MNEMONIC);
}

static void testDecimal(void) {
    TEST_TOKEN("10", scpiLex_DecimalNumericProgramData, 0, 2, SCPI_TOKEN_DECIMAL_NUMERIC_PROGRAM_DATA);
    TEST_TOKEN("10 , ", scpiLex_DecimalNumericProgramData, 0, 2, SCPI_TOKEN_DECIMAL_NUMERIC_PROGRAM_DATA);
    TEST_TOKEN("-10.5 , ", scpiLex_DecimalNumericProgramData, 0, 5, SCPI_TOKEN_DECIMAL_NUMERIC_PROGRAM_DATA);
    TEST_TOKEN("+.5 , ", scpiLex_DecimalNumericProgramData, 0, 3, SCPI_TOKEN_DECIMAL_NUMERIC_PROGRAM_DATA);
    TEST_TOKEN("-. , ", scpiLex_DecimalNumericProgramData, 0, 0, SCPI_TOKEN_UNKNOWN);
    TEST_TOKEN("-1 e , ", scpiLex_DecimalNumericProgramData, 0, 2, SCPI_TOKEN_DECIMAL_NUMERIC_PROGRAM_DATA);
    TEST_TOKEN("-1 e 3, ", scpiLex_DecimalNumericProgramData, 0, 6, SCPI_TOKEN_DECIMAL_NUMERIC_PROGRAM_DATA);
    TEST_TOKEN("1.5E12", scpiLex_DecimalNumericProgramData, 0, 6, SCPI_TOKEN_DECIMAL_NUMERIC_PROGRAM_DATA);
}

static void testSuffix(void) {
    TEST_TOKEN("A/V , ", scpiLex_SuffixProgramData, 0, 3, SCPI_TOKEN_SUFFIX_PROGRAM_DATA);
    TEST_TOKEN("mA.h", scpiLex_SuffixProgramData, 0, 4, SCPI_TOKEN_SUFFIX_PROGRAM_DATA);
    TEST_TOKEN("mA3.h", scpiLex_SuffixProgramData, 0, 5, SCPI_TOKEN_SUFFIX_PROGRAM_DATA);
}

static void testProgramHeader(void) {
    TEST_TOKEN("*IDN? ", scpiLex_ProgramHeader, 0, 5, SCPI_TOKEN_COMMON_QUERY_PROGRAM_HEADER);
    TEST_TOKEN("*RST ", scpiLex_ProgramHeader, 0, 4, SCPI_TOKEN_COMMON_PROGRAM_HEADER);
    TEST_TOKEN("*?; ", scpiLex_ProgramHeader, 0, 1, SCPI_TOKEN_INCOMPLETE_COMMON_PROGRAM_HEADER);
    TEST_TOKEN(":*IDN?; ", scpiLex_ProgramHeader, 0, 1, SCPI_TOKEN_INCOMPLETE_COMPOUND_PROGRAM_HEADER);
    TEST_TOKEN(":meas", scpiLex_ProgramHeader, 0, 5, SCPI_TOKEN_COMPOUND_PROGRAM_HEADER);
    TEST_TOKEN("MEAS:VOLT:DC? ", scpiLex_ProgramHeader, 0, 13, SCPI_TOKEN_COMPOUND_QUERY_PROGRAM_HEADER);
    TEST_TOKEN("CONF:VOLT:DC ", scpiLex_ProgramHeader, 0, 12, SCPI_TOKEN_COMPOUND_PROGRAM_HEADER);
    TEST_TOKEN(":MEAS:VOLT:DC? ", scpiLex_ProgramHeader, 0, 14, SCPI_TOKEN_COMPOUND_QUERY_PROGRAM_HEADER);
    TEST_TOKEN(":MEAS::VOLT:DC? ", scpiLex_ProgramHeader, 0, 6, SCPI_TOKEN_INCOMPLETE_COMPOUND_PROGRAM_HEADER);
    TEST_TOKEN("*IDN?", scpiLex_ProgramHeader, 0, 5, SCPI_TOKEN_COMMON_QUERY_PROGRAM_HEADER);
    TEST_TOKEN("*RST", scpiLex_ProgramHeader, 0, 4, SCPI_TOKEN_COMMON_PROGRAM_HEADER);
    TEST_TOKEN("CONF:VOLT:DC", scpiLex_ProgramHeader, 0, 12, SCPI_TOKEN_COMPOUND_PROGRAM_HEADER);
    TEST_TOKEN("CONF:VOLT:DC", scpiLex_ProgramHeader, 0, 12, SCPI_TOKEN_COMPOUND_PROGRAM_HEADER);
    TEST_TOKEN("]]", scpiLex_ProgramHeader, 0, 0, SCPI_TOKEN_UNKNOWN);
    TEST_TOKEN("*", scpiLex_ProgramHeader, 0, 1, SCPI_TOKEN_INCOMPLETE_COMMON_PROGRAM_HEADER);
}

static void testArbitraryBlock(void) {
    TEST_TOKEN("#12AB", scpiLex_ArbitraryBlockProgramData, 3, 2, SCPI_TOKEN_ARBITRARY_BLOCK_PROGRAM_DATA);
    TEST_TOKEN("#12AB, ", scpiLex_ArbitraryBlockProgramData, 3, 2, SCPI_TOKEN_ARBITRARY_BLOCK_PROGRAM_DATA);
    TEST_TOKEN("#13AB", scpiLex_ArbitraryBlockProgramData, 0, 0, SCPI_TOKEN_UNKNOWN);
    TEST_TOKEN("#12\r\n, ", scpiLex_ArbitraryBlockProgramData, 3, 2, SCPI_TOKEN_ARBITRARY_BLOCK_PROGRAM_DATA);
    TEST_TOKEN("#02AB, ", scpiLex_ArbitraryBlockProgramData, 0, 0, SCPI_TOKEN_UNKNOWN);
    TEST_TOKEN("#12", scpiLex_ArbitraryBlockProgramData, 0, 0, SCPI_TOKEN_UNKNOWN);
    TEST_TOKEN("#1", scpiLex_ArbitraryBlockProgramData, 0, 0, SCPI_TOKEN_UNKNOWN);
    TEST_TOKEN("#", scpiLex_ArbitraryBlockProgramData, 0, 0, SCPI_TOKEN_UNKNOWN);
}

static void testExpression(void) {
    TEST_TOKEN("( 1 + 2 )", scpiLex_ProgramExpression, 0, 9, SCPI_TOKEN_PROGRAM_EXPRESSION);
    TEST_TOKEN("( 1 + 2 ) , ", scpiLex_ProgramExpression, 0, 9, SCPI_TOKEN_PROGRAM_EXPRESSION);
    TEST_TOKEN("( 1 + 2  , ", scpiLex_ProgramExpression, 0, 0, SCPI_TOKEN_UNKNOWN);
}

static void testString(void) {
    TEST_TOKEN("\"ahoj\"", scpiLex_StringProgramData, 0, 6, SCPI_TOKEN_DOUBLE_QUOTE_PROGRAM_DATA);
    TEST_TOKEN("\"ahoj\" ", scpiLex_StringProgramData, 0, 6, SCPI_TOKEN_DOUBLE_QUOTE_PROGRAM_DATA);
    TEST_TOKEN("'ahoj' ", scpiLex_StringProgramData, 0, 6, SCPI_TOKEN_SINGLE_QUOTE_PROGRAM_DATA);
    TEST_TOKEN("'ahoj ", scpiLex_StringProgramData, 0, 0, SCPI_TOKEN_UNKNOWN);
    TEST_TOKEN("'ah''oj' ", scpiLex_StringProgramData, 0, 8, SCPI_TOKEN_SINGLE_QUOTE_PROGRAM_DATA);
    TEST_TOKEN("'ah\"oj' ", scpiLex_StringProgramData, 0, 7, SCPI_TOKEN_SINGLE_QUOTE_PROGRAM_DATA);
    TEST_TOKEN("\"ah\"\"oj\" ", scpiLex_StringProgramData, 0, 8, SCPI_TOKEN_DOUBLE_QUOTE_PROGRAM_DATA);
    TEST_TOKEN("\"\"", scpiLex_StringProgramData, 0, 2, SCPI_TOKEN_DOUBLE_QUOTE_PROGRAM_DATA);
    TEST_TOKEN("''", scpiLex_StringProgramData, 0, 2, SCPI_TOKEN_SINGLE_QUOTE_PROGRAM_DATA);

    TEST_TOKEN("'abcd", scpiLex_StringProgramData, 0, 0, SCPI_TOKEN_UNKNOWN);
    TEST_TOKEN("\"abcd", scpiLex_StringProgramData, 0, 0, SCPI_TOKEN_UNKNOWN);
}

static void testProgramData(void) {
    TEST_TOKEN("#H123fe5A", scpiParser_parseProgramData, 2, 7, SCPI_TOKEN_HEXNUM);
    TEST_TOKEN("  #H123fe5A ", scpiParser_parseProgramData, 4, 7, SCPI_TOKEN_HEXNUM);
    TEST_TOKEN("#B0111010101", scpiParser_parseProgramData, 2, 10, SCPI_TOKEN_BINNUM);
    TEST_TOKEN("#Q125725433", scpiParser_parseProgramData, 2, 9, SCPI_TOKEN_OCTNUM);

    TEST_TOKEN("10", scpiParser_parseProgramData, 0, 2, SCPI_TOKEN_DECIMAL_NUMERIC_PROGRAM_DATA);
    TEST_TOKEN("10 , ", scpiParser_parseProgramData, 0, 2, SCPI_TOKEN_DECIMAL_NUMERIC_PROGRAM_DATA);
    TEST_TOKEN("-10.5 , ", scpiParser_parseProgramData, 0, 5, SCPI_TOKEN_DECIMAL_NUMERIC_PROGRAM_DATA);
    TEST_TOKEN("+.5 , ", scpiParser_parseProgramData, 0, 3, SCPI_TOKEN_DECIMAL_NUMERIC_PROGRAM_DATA);
    TEST_TOKEN("-. , ", scpiParser_parseProgramData, 0, 0, SCPI_TOKEN_UNKNOWN);
    TEST_TOKEN("-1 e , ", scpiParser_parseProgramData, 0, 4, SCPI_TOKEN_DECIMAL_NUMERIC_PROGRAM_DATA_WITH_SUFFIX);
    TEST_TOKEN("-1 e 3, ", scpiParser_parseProgramData, 0, 6, SCPI_TOKEN_DECIMAL_NUMERIC_PROGRAM_DATA);
    TEST_TOKEN("1.5E12", scpiParser_parseProgramData, 0, 6, SCPI_TOKEN_DECIMAL_NUMERIC_PROGRAM_DATA);

    TEST_TOKEN("#12AB", scpiParser_parseProgramData, 3, 2, SCPI_TOKEN_ARBITRARY_BLOCK_PROGRAM_DATA);
    TEST_TOKEN("#12AB, ", scpiParser_parseProgramData, 3, 2, SCPI_TOKEN_ARBITRARY_BLOCK_PROGRAM_DATA);
    TEST_TOKEN("#13AB", scpiParser_parseProgramData, 5, 0, SCPI_TOKEN_UNKNOWN);
    TEST_TOKEN("#12\r\n, ", scpiParser_parseProgramData, 3, 2, SCPI_TOKEN_ARBITRARY_BLOCK_PROGRAM_DATA);
    TEST_TOKEN("#02AB, ", scpiParser_parseProgramData, 0, 0, SCPI_TOKEN_UNKNOWN);

    TEST_TOKEN("( 1 + 2 ) , ", scpiParser_parseProgramData, 0, 9, SCPI_TOKEN_PROGRAM_EXPRESSION);
    TEST_TOKEN("( 1 + 2  , ", scpiParser_parseProgramData, 0, 0, SCPI_TOKEN_UNKNOWN);

    TEST_TOKEN("\"ahoj\" ", scpiParser_parseProgramData, 0, 6, SCPI_TOKEN_DOUBLE_QUOTE_PROGRAM_DATA);
    TEST_TOKEN("'ahoj' ", scpiParser_parseProgramData, 0, 6, SCPI_TOKEN_SINGLE_QUOTE_PROGRAM_DATA);
    TEST_TOKEN("'ahoj ", scpiParser_parseProgramData, 0, 0, SCPI_TOKEN_UNKNOWN);
    TEST_TOKEN("'ah''oj' ", scpiParser_parseProgramData, 0, 8, SCPI_TOKEN_SINGLE_QUOTE_PROGRAM_DATA);
    TEST_TOKEN("'ah\"oj' ", scpiParser_parseProgramData, 0, 7, SCPI_TOKEN_SINGLE_QUOTE_PROGRAM_DATA);
    TEST_TOKEN("\"ah\"\"oj\" ", scpiParser_parseProgramData, 0, 8, SCPI_TOKEN_DOUBLE_QUOTE_PROGRAM_DATA);
    TEST_TOKEN("\"\"", scpiParser_parseProgramData, 0, 2, SCPI_TOKEN_DOUBLE_QUOTE_PROGRAM_DATA);
    TEST_TOKEN("'test\r\n' ", scpiParser_parseProgramData, 0, 8, SCPI_TOKEN_SINGLE_QUOTE_PROGRAM_DATA);
    TEST_TOKEN("'\xFA\xF0' ", scpiParser_parseProgramData, 0, 0, SCPI_TOKEN_UNKNOWN);

    TEST_TOKEN("abc_213as564 , ", scpiLex_CharacterProgramData, 0, 12, SCPI_TOKEN_PROGRAM_MNEMONIC);

    TEST_TOKEN("1.5E12 V", scpiParser_parseProgramData, 0, 8, SCPI_TOKEN_DECIMAL_NUMERIC_PROGRAM_DATA_WITH_SUFFIX);
}


#define TEST_ALL_TOKEN(s, f, o, l, t, c) do {   \
    char * str = s;                             \
    lexfn2_t fn = f;                            \
    int offset = o;                             \
    int len = l;                                \
    scpi_token_type_t tp = t;                   \
    lex_state_t state;                          \
    scpi_token_t token;                         \
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

static void testAllProgramData(void) {
    TEST_ALL_TOKEN("1.5E12 V", scpiParser_parseAllProgramData, 0, 8, SCPI_TOKEN_ALL_PROGRAM_DATA, 1);
    TEST_ALL_TOKEN("1.5E12 V, abc_213as564, 10, #H123fe5A", scpiParser_parseAllProgramData, 0, 37, SCPI_TOKEN_ALL_PROGRAM_DATA, 4);
    TEST_ALL_TOKEN("1.5E12 V, ", scpiParser_parseAllProgramData, 0, 0, SCPI_TOKEN_UNKNOWN, -1);
    TEST_ALL_TOKEN("#12\r\n, 1.5E12 V", scpiParser_parseAllProgramData, 0, 15, SCPI_TOKEN_ALL_PROGRAM_DATA, 2);
    TEST_ALL_TOKEN(" ( 1 + 2 ) ,#12\r\n, 1.5E12 V", scpiParser_parseAllProgramData, 0, 27, SCPI_TOKEN_ALL_PROGRAM_DATA, 3);
    TEST_ALL_TOKEN("\"ahoj\" , #12AB", scpiParser_parseAllProgramData, 0, 14, SCPI_TOKEN_ALL_PROGRAM_DATA, 2);
}


#define TEST_DETECT(s, h, hl, ht, d, dc, t) do {                                \
    char * str = s;                                                             \
    scpi_parser_state_t state;                                                  \
    int result;                                                                 \
    result = scpiParser_detectProgramMessageUnit(&state, str, strlen(str));     \
    CU_ASSERT_EQUAL(state.programHeader.ptr, str + h);                          \
    CU_ASSERT_EQUAL(state.programHeader.len, hl);                               \
    CU_ASSERT_EQUAL(state.programHeader.type, ht);                              \
    CU_ASSERT_EQUAL(state.programData.ptr, str + d);                            \
    CU_ASSERT_EQUAL(state.numberOfParameters, dc);                              \
    CU_ASSERT_EQUAL(state.termination, t);                                      \
} while(0)

static void testDetectProgramMessageUnit(void) {
    TEST_DETECT("*IDN?\r\n", 0, 5, SCPI_TOKEN_COMMON_QUERY_PROGRAM_HEADER, 5, 0, SCPI_MESSAGE_TERMINATION_NL);
    TEST_DETECT(" MEAS:VOLT:DC?\r\n", 1, 13, SCPI_TOKEN_COMPOUND_QUERY_PROGRAM_HEADER, 14, 0, SCPI_MESSAGE_TERMINATION_NL);
    TEST_DETECT(" MEAS:VOLT:DC? 1.2 V\r\n", 1, 13, SCPI_TOKEN_COMPOUND_QUERY_PROGRAM_HEADER, 15, 1, SCPI_MESSAGE_TERMINATION_NL);
    TEST_DETECT(" CONF:VOLT:DC 1.2 V, 100mv;", 1, 12, SCPI_TOKEN_COMPOUND_PROGRAM_HEADER, 14, 2, SCPI_MESSAGE_TERMINATION_SEMICOLON);
    TEST_DETECT(" CONF:VOLT:DC 1.2 V, 100mv", 1, 12, SCPI_TOKEN_COMPOUND_PROGRAM_HEADER, 14, 2, SCPI_MESSAGE_TERMINATION_NONE);
    TEST_DETECT(" CONF:VOLT:DC 1.2 V, \r\n", 1, 12, SCPI_TOKEN_COMPOUND_PROGRAM_HEADER, 14, -1, SCPI_MESSAGE_TERMINATION_NL);
    TEST_DETECT("[\r\n", 0, 1, SCPI_TOKEN_INVALID, 0, 0, SCPI_MESSAGE_TERMINATION_NONE);
}

static void testBoolParameter(void) {
    TEST_TOKEN(" 1", scpiParser_parseProgramData, 1, 1, SCPI_TOKEN_DECIMAL_NUMERIC_PROGRAM_DATA);
    TEST_TOKEN(" 0", scpiParser_parseProgramData, 1, 1, SCPI_TOKEN_DECIMAL_NUMERIC_PROGRAM_DATA);
    TEST_TOKEN(" ON", scpiParser_parseProgramData, 1, 2, SCPI_TOKEN_PROGRAM_MNEMONIC);
    TEST_TOKEN("OFF ", scpiParser_parseProgramData, 0, 3, SCPI_TOKEN_PROGRAM_MNEMONIC);
}

int main() {
    unsigned int result;
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
    result = CU_get_number_of_tests_failed();
    CU_cleanup_registry();
    return result ? result : CU_get_error();
}

