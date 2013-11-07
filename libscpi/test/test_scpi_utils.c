/*-
 * Copyright (c) 2013 Jan Breuer
 *                    Richard.hmm
 * Copyright (c) 2012 Jan Breuer
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

/*
 * File:   test_scpi_utils.c
 * Author: Jan Breuer
 *
 * Created on 26.11.2012, 11:22:00
 */

#include <stdio.h>
#include <stdlib.h>
#include "CUnit/Basic.h"

#include "scpi/scpi.h"
#include "../src/utils.h"

/*
 * CUnit Test Suite
 */

int init_suite(void) {
    return 0;
}

int clean_suite(void) {
    return 0;
}

void test_strnpbrk() {
    char str[] = "ahoj";

    CU_ASSERT(strnpbrk(str, 4, "a") == (str + 0));
    CU_ASSERT(strnpbrk(str, 4, "h") == (str + 1));
    CU_ASSERT(strnpbrk(str, 4, "b") == NULL);
    CU_ASSERT(strnpbrk(str, 1, "h") == NULL);
    CU_ASSERT(strnpbrk(str, 4, "xo") == (str + 2));
}

void test_longToStr() {
    char str[32];
    size_t len;

    len = longToStr(10, str, 32);
    CU_ASSERT(len == 2);
    CU_ASSERT(str[0] == '1');
    CU_ASSERT(str[1] == '0');
    CU_ASSERT(str[2] == '\0');
}

void test_doubleToStr() {
    size_t result;
    char str[50];

#define TEST_DOUBLE_TO_STR(v, r, s)                     \
    do {                                                \
        result = doubleToStr(v, str, sizeof(str));      \
        CU_ASSERT_EQUAL(result, r);                     \
        CU_ASSERT_STRING_EQUAL(str, s);                 \
    } while(0)                                          \


    TEST_DOUBLE_TO_STR(1, 1, "1");
    TEST_DOUBLE_TO_STR(-1, 2, "-1");
    TEST_DOUBLE_TO_STR(1.1, 3, "1.1");
    TEST_DOUBLE_TO_STR(-1.1, 4, "-1.1");
    TEST_DOUBLE_TO_STR(1e3, 4, "1000");
    TEST_DOUBLE_TO_STR(1e30, 5, "1e+30");
    TEST_DOUBLE_TO_STR(-1.3e30, 8, "-1.3e+30");
    TEST_DOUBLE_TO_STR(-1.3e-30, 8, "-1.3e-30");
}

void test_strToLong() {
    size_t result;
    int32_t val;

#define TEST_STR_TO_LONG(s, r, v)                       \
    do {                                                \
        result = strToLong(s, &val);                    \
        CU_ASSERT_EQUAL(val, v);                        \
        CU_ASSERT_EQUAL(result, r);                     \
    } while(0)                                          \
    
    TEST_STR_TO_LONG("", 0, 0);
    TEST_STR_TO_LONG("1", 1, 1);
    TEST_STR_TO_LONG("10", 2, 10);
    TEST_STR_TO_LONG("-50", 3, -50);
    TEST_STR_TO_LONG("100MHz", 3, 100);
    TEST_STR_TO_LONG("MHz", 0, 0);
    TEST_STR_TO_LONG("1.4", 1, 1);
    TEST_STR_TO_LONG(" 1", 2, 1);
    TEST_STR_TO_LONG(" +100", 5, 100); // space and +
    TEST_STR_TO_LONG("0xFF", 4, 255); // hexadecimal FF
    TEST_STR_TO_LONG("077", 3, 63); // octal 77
    TEST_STR_TO_LONG("018", 2, 1); // octal 1, 8 is ignored
}

void test_strToDouble() {
    double val;
    size_t result;

#define TEST_STR_TO_DOUBLE(s, r, v)                     \
    do {                                                \
        result = strToDouble(s, &val);                  \
        CU_ASSERT_EQUAL(result, r);                     \
        CU_ASSERT_DOUBLE_EQUAL(v, val, 0.000001);       \
    } while(0);                                         \
    
    TEST_STR_TO_DOUBLE("", 0, 0.0);

    TEST_STR_TO_DOUBLE(" 1", 2, 1.0);

    TEST_STR_TO_DOUBLE("1", 1, 1.0);
    TEST_STR_TO_DOUBLE("10", 2, 10.0);
    TEST_STR_TO_DOUBLE("10MHz", 2, 10.0);
    TEST_STR_TO_DOUBLE("MHz", 0, 0.0);
    TEST_STR_TO_DOUBLE("1E", 1, 1.0);
    TEST_STR_TO_DOUBLE("1E3", 3, 1000.0);

    TEST_STR_TO_DOUBLE("1.2", 3, 1.2);
    TEST_STR_TO_DOUBLE("10.2", 4, 10.2);
    TEST_STR_TO_DOUBLE("10.2MHz", 4, 10.2);
    TEST_STR_TO_DOUBLE("MHz", 0, 0.0);
    TEST_STR_TO_DOUBLE("1.2E", 3, 1.2);
    TEST_STR_TO_DOUBLE("1.2E3", 5, 1200.0);

    TEST_STR_TO_DOUBLE("-1.2", 4, -1.2);

}

void test_compareStr() {

    CU_ASSERT_TRUE(compareStr("abcd", 1, "afgh", 1));
    CU_ASSERT_TRUE(compareStr("ABCD", 4, "abcd", 4));
    CU_ASSERT_TRUE(compareStr("AbCd", 3, "AbCE", 3));
    CU_ASSERT_TRUE(compareStr("ABCD", 1, "a", 1));

    CU_ASSERT_FALSE(compareStr("abcd", 1, "efgh", 1));
    CU_ASSERT_FALSE(compareStr("ABCD", 4, "abcd", 3));
}

void test_locateText() {

    const char * v;
    const char * b;
    size_t l;
    int result;


#define TEST_LOCATE_TEXT(s, ex_res, ex_off, ex_len)      \
    do {                                                \
        v = (s);                                        \
        b = NULL;                                       \
        l = 0;                                          \
        result = locateText(v, strlen(v), &b, &l);       \
        CU_ASSERT(result == ex_res);                    \
        if (result == TRUE) {                           \
                CU_ASSERT(b == (v + ex_off));           \
                CU_ASSERT(l == ex_len);                 \
        } else {                                        \
                CU_ASSERT(b == NULL);                   \
                CU_ASSERT(l == 0);                      \
        }                                               \
    } while(0)                                          \


    TEST_LOCATE_TEXT("", TRUE, 0, 0);
    TEST_LOCATE_TEXT(",", TRUE, 0, 0);
    TEST_LOCATE_TEXT("\"\",", TRUE, 1, 0);
    TEST_LOCATE_TEXT("   ", TRUE, 3, 0);
    TEST_LOCATE_TEXT("a", TRUE, 0, 1);
    TEST_LOCATE_TEXT("ab", TRUE, 0, 2);
    TEST_LOCATE_TEXT("abc", TRUE, 0, 3);
    TEST_LOCATE_TEXT(" abc", TRUE, 1, 3);
    TEST_LOCATE_TEXT(" abc def", TRUE, 1, 7);
    TEST_LOCATE_TEXT(" abc def ", TRUE, 1, 7);
    TEST_LOCATE_TEXT("\"\"", TRUE, 1, 0);
    TEST_LOCATE_TEXT("\"a\"", TRUE, 1, 1);
    TEST_LOCATE_TEXT(" \"a\" ", TRUE, 2, 1);
    TEST_LOCATE_TEXT(" \"a\"  ", TRUE, 2, 1);
    TEST_LOCATE_TEXT(" \"a\"  ,", TRUE, 2, 1);
    TEST_LOCATE_TEXT(" \"a,b\"", TRUE, 2, 3);
    TEST_LOCATE_TEXT(" \"a,b\"     ,", TRUE, 2, 3);
    TEST_LOCATE_TEXT(" a b    ", TRUE, 1, 3);
    TEST_LOCATE_TEXT(" a b   ,", TRUE, 1, 3);
    TEST_LOCATE_TEXT(" \"a \" ", TRUE, 2, 2);
    TEST_LOCATE_TEXT(" \"a  ", FALSE, 0, 0);
    TEST_LOCATE_TEXT(" \"a\" a, a ", FALSE, 0, 0);
    TEST_LOCATE_TEXT(" \"a\" , a ", TRUE, 2, 1);
}

void test_locateStr() {

    const char * v;
    const char * b;
    size_t l;
    int result;


#define TEST_LOCATE_STR(s, ex_res, ex_off, ex_len)      \
    do {                                                \
        v = (s);                                        \
        b = NULL;                                       \
        l = 0;                                          \
        result = locateStr(v, strlen(v), &b, &l);       \
        CU_ASSERT(result == ex_res);                    \
        if (result == TRUE) {                           \
                CU_ASSERT(b == (s + ex_off));           \
                CU_ASSERT(l == ex_len);                 \
        } else {                                        \
                CU_ASSERT(b == NULL);                   \
                CU_ASSERT(l == 0);                      \
        }                                               \
    } while(0)                                          \
    
    TEST_LOCATE_STR("", TRUE, 0, 0);
    TEST_LOCATE_STR(",", TRUE, 0, 0);
    TEST_LOCATE_STR("   ", TRUE, 3, 0);
    TEST_LOCATE_STR("a", TRUE, 0, 1);
    TEST_LOCATE_STR("ab", TRUE, 0, 2);
    TEST_LOCATE_STR("abc", TRUE, 0, 3);
    TEST_LOCATE_STR(" abc", TRUE, 1, 3);
    TEST_LOCATE_STR(" abc def", TRUE, 1, 7);
    TEST_LOCATE_STR(" abc def ", TRUE, 1, 7);
    TEST_LOCATE_STR("\"\"", TRUE, 0, 2);
    TEST_LOCATE_STR("\"a\"", TRUE, 0, 3);
    TEST_LOCATE_STR(" \"a\" ", TRUE, 1, 3);
    TEST_LOCATE_STR(" \"a\"  ", TRUE, 1, 3);
    TEST_LOCATE_STR(" \"a\"  ,", TRUE, 1, 3);
    TEST_LOCATE_STR(" \"a,b\"", TRUE, 1, 2);
    TEST_LOCATE_STR(" \"a,b\"     ,", TRUE, 1, 2);
    TEST_LOCATE_STR(" a b    ", TRUE, 1, 3);
    TEST_LOCATE_STR(" a b   ,", TRUE, 1, 3);
    TEST_LOCATE_STR(" \"a \" ", TRUE, 1, 4);
    TEST_LOCATE_STR(" \"a  ", TRUE, 1, 2);
    TEST_LOCATE_STR(" \"a\" a, a ", TRUE, 1, 5);
    TEST_LOCATE_STR(" \"a\" , a ", TRUE, 1, 3);
}

void test_matchPattern() {
    bool_t result;
    
#define TEST_MATCH_PATTERN(p, s, r)                             \
    do {                                                        \
        result = matchPattern(p, strlen(p), s, strlen(s));      \
        CU_ASSERT_EQUAL(result, r);                             \
    } while(0)                                                  \

    TEST_MATCH_PATTERN("A", "a", TRUE);
    TEST_MATCH_PATTERN("Ab", "a", TRUE);
    TEST_MATCH_PATTERN("Ab", "ab", TRUE);
    TEST_MATCH_PATTERN("Ab", "aB", TRUE);
    TEST_MATCH_PATTERN("AB", "a", FALSE);
}

void test_matchCommand() {
    bool_t result;
    
    #define TEST_MATCH_COMMAND(p, s, r)                         \
    do {                                                        \
        result = matchCommand(p, s, strlen(s));                 \
        CU_ASSERT_EQUAL(result, r);                             \
    } while(0)                                                  \

    TEST_MATCH_COMMAND("A", "a", TRUE);
    TEST_MATCH_COMMAND("Ab", "a", TRUE);
    TEST_MATCH_COMMAND("Ab", "ab", TRUE);
    TEST_MATCH_COMMAND("Ab", "aB", TRUE);
    TEST_MATCH_COMMAND("AB", "a", FALSE);
    TEST_MATCH_COMMAND("ABc:AACddd", "ab:aac", TRUE);
    TEST_MATCH_COMMAND("ABc:AACddd", "abc:aac", TRUE);
    TEST_MATCH_COMMAND("ABc:AACddd", "abc:aacddd", TRUE);
    TEST_MATCH_COMMAND("ABc:AACddd", "abc:aacdd", FALSE);
    TEST_MATCH_COMMAND("ABc:AACddd", "a:aac", FALSE);
    TEST_MATCH_COMMAND("ABc:AACddd", ":ab:aac", TRUE);
    TEST_MATCH_COMMAND("ABc:AACddd", ":abc:aac", TRUE);
    TEST_MATCH_COMMAND("ABc:AACddd", ":abc:aacddd", TRUE);
    TEST_MATCH_COMMAND("ABc:AACddd", ":abc:aacdd", FALSE);
    TEST_MATCH_COMMAND("ABc:AACddd", ":a:aac", FALSE);
    TEST_MATCH_COMMAND("?", "?", TRUE);
    TEST_MATCH_COMMAND("A?", "A?", TRUE);
    TEST_MATCH_COMMAND("A", "A?", FALSE);
    TEST_MATCH_COMMAND("A?", "A", FALSE);
    TEST_MATCH_COMMAND("[:ABc]:AACddd", ":ab:aac", TRUE); // test optional keyword
    TEST_MATCH_COMMAND("[:ABc]:AACddd", "aac", TRUE); // test optional keyword
    TEST_MATCH_COMMAND("[:ABc]:AACddd", "aac?", FALSE); // test optional keyword
    TEST_MATCH_COMMAND("[:ABc]:AACddd?", ":ab:aac?", TRUE); // test optional keyword
    TEST_MATCH_COMMAND("[:ABc]:AACddd?", "aac?", TRUE); // test optional keyword
    TEST_MATCH_COMMAND("[:ABc]:AACddd?", "aac", FALSE); // test optional keyword
    TEST_MATCH_COMMAND("ABc[:BCd]:CDe", "ab:bc:cd", TRUE); // test optional keyword
    TEST_MATCH_COMMAND("ABc[:BCd]:CDe", "ab:cd", TRUE); // test optional keyword
    TEST_MATCH_COMMAND("ABc[:BCd]:CDe", "ab:cd?", FALSE); // test optional keyword
    TEST_MATCH_COMMAND("ABc[:BCd]:CDe?", "ab:bc:cd?", TRUE); // test optional keyword
    TEST_MATCH_COMMAND("ABc[:BCd]:CDe?", "ab:cd?", TRUE); // test optional keyword
    TEST_MATCH_COMMAND("ABc[:BCd]:CDe?", "ab:cd", FALSE); // test optional keyword
    TEST_MATCH_COMMAND("ABc:BCd[:CDe]", "ab:bc:cd", TRUE); // test optional keyword
    TEST_MATCH_COMMAND("ABc:BCd[:CDe]", "ab:bc", TRUE); // test optional keyword
    TEST_MATCH_COMMAND("ABc:BCd[:CDe]", "ab:bc?", FALSE); // test optional keyword
    TEST_MATCH_COMMAND("ABc:BCd[:CDe]?", "ab:bc:cd?", TRUE); // test optional keyword
    TEST_MATCH_COMMAND("ABc:BCd[:CDe]?", "ab:bc?", TRUE); // test optional keyword
    TEST_MATCH_COMMAND("ABc:BCd[:CDe]?", "ab:bc", FALSE); // test optional keyword

    TEST_MATCH_COMMAND("[:ABc]:BCd[:CDe]", "ab:bc:cd", TRUE); // test optional keyword
    TEST_MATCH_COMMAND("[:ABc]:BCd[:CDe]", "ab:bc", TRUE); // test optional keyword
    TEST_MATCH_COMMAND("[:ABc]:BCd[:CDe]", "bc:cd", TRUE); // test optional keyword
    TEST_MATCH_COMMAND("[:ABc]:BCd[:CDe]", "ab:bc?", FALSE); // test optional keyword
    TEST_MATCH_COMMAND("[:ABc]:BCd[:CDe]", "bc:cd?", FALSE); // test optional keyword
    TEST_MATCH_COMMAND("[:ABc]:BCd[:CDe]?", "ab:bc:cd?", TRUE); // test optional keyword
    TEST_MATCH_COMMAND("[:ABc]:BCd[:CDe]?", "ab:bc?", TRUE); // test optional keyword
    TEST_MATCH_COMMAND("[:ABc]:BCd[:CDe]?", "bc:cd?", TRUE); // test optional keyword
    TEST_MATCH_COMMAND("[:ABc]:BCd[:CDe]?", "ab:bc", FALSE); // test optional keyword
    TEST_MATCH_COMMAND("[:ABc]:BCd[:CDe]?", "bc:cd", FALSE); // test optional keyword

    TEST_MATCH_COMMAND("ABc[:BCd][:CDe]", "ab:bc:cd", TRUE); // test optional keyword
    TEST_MATCH_COMMAND("ABc[:BCd][:CDe]", "ab:bc", TRUE); // test optional keyword
    TEST_MATCH_COMMAND("ABc[:BCd][:CDe]", "ab:cd", TRUE); // test optional keyword
    TEST_MATCH_COMMAND("ABc[:BCd][:CDe]", "ab:bc?", FALSE); // test optional keyword
    TEST_MATCH_COMMAND("ABc[:BCd][:CDe]", "ab:cd?", FALSE); // test optional keyword
    TEST_MATCH_COMMAND("ABc[:BCd][:CDe]?", "ab:bc:cd?", TRUE); // test optional keyword
    TEST_MATCH_COMMAND("ABc[:BCd][:CDe]?", "ab:bc?", TRUE); // test optional keyword
    TEST_MATCH_COMMAND("ABc[:BCd][:CDe]?", "ab:cd?", TRUE); // test optional keyword
    TEST_MATCH_COMMAND("ABc[:BCd][:CDe]?", "ab:bc", FALSE); // test optional keyword
    TEST_MATCH_COMMAND("ABc[:BCd][:CDe]?", "ab:cd", FALSE); // test optional keyword

    TEST_MATCH_COMMAND("ABc[:BCd][:CDe][:DEf]", "ab:bc:cd:de", TRUE); // test optional keyword
    TEST_MATCH_COMMAND("ABc[:BCd][:CDe][:DEf]", "ab:bc:cd", TRUE); // test optional keyword
    TEST_MATCH_COMMAND("ABc[:BCd][:CDe][:DEf]", "ab:bc:de", TRUE); // test optional keyword
    TEST_MATCH_COMMAND("ABc[:BCd][:CDe][:DEf]", "ab:cd:de", TRUE); // test optional keyword
    TEST_MATCH_COMMAND("ABc[:BCd][:CDe][:DEf]", "ab:bc", TRUE); // test optional keyword
    TEST_MATCH_COMMAND("ABc[:BCd][:CDe][:DEf]", "ab:cd", TRUE); // test optional keyword
    TEST_MATCH_COMMAND("ABc[:BCd][:CDe][:DEf]", "ab:de", TRUE); // test optional keyword
    TEST_MATCH_COMMAND("ABc[:BCd][:CDe][:DEf]", "ab", TRUE); // test optional keyword
    TEST_MATCH_COMMAND("ABc[:BCd][:CDe][:DEf]", "ab:bc:cd?", FALSE); // test optional keyword
    TEST_MATCH_COMMAND("ABc[:BCd][:CDe][:DEf]", "ab:bc:de?", FALSE); // test optional keyword
    TEST_MATCH_COMMAND("ABc[:BCd][:CDe][:DEf]", "ab:cd:de?", FALSE); // test optional keyword
    TEST_MATCH_COMMAND("ABc[:BCd][:CDe][:DEf]", "ab:bc?", FALSE); // test optional keyword
    TEST_MATCH_COMMAND("ABc[:BCd][:CDe][:DEf]", "ab:cd?", FALSE); // test optional keyword
    TEST_MATCH_COMMAND("ABc[:BCd][:CDe][:DEf]", "ab:de?", FALSE); // test optional keyword
    TEST_MATCH_COMMAND("ABc[:BCd][:CDe][:DEf]", "ab?", FALSE); // test optional keyword

    TEST_MATCH_COMMAND("ABc[:BCd][:CDe][:DEf]?", "ab:bc:cd:de?", TRUE); // test optional keyword
    TEST_MATCH_COMMAND("ABc[:BCd][:CDe][:DEf]?", "ab:bc:cd?", TRUE); // test optional keyword
    TEST_MATCH_COMMAND("ABc[:BCd][:CDe][:DEf]?", "ab:bc:de?", TRUE); // test optional keyword
    TEST_MATCH_COMMAND("ABc[:BCd][:CDe][:DEf]?", "ab:cd:de?", TRUE); // test optional keyword
    TEST_MATCH_COMMAND("ABc[:BCd][:CDe][:DEf]?", "ab:bc?", TRUE); // test optional keyword
    TEST_MATCH_COMMAND("ABc[:BCd][:CDe][:DEf]?", "ab:cd?", TRUE); // test optional keyword
    TEST_MATCH_COMMAND("ABc[:BCd][:CDe][:DEf]?", "ab:de?", TRUE); // test optional keyword
    TEST_MATCH_COMMAND("ABc[:BCd][:CDe][:DEf]?", "ab?", TRUE); // test optional keyword
    TEST_MATCH_COMMAND("ABc[:BCd][:CDe][:DEf]?", "ab:bc:cd", FALSE); // test optional keyword
    TEST_MATCH_COMMAND("ABc[:BCd][:CDe][:DEf]?", "ab:bc:de", FALSE); // test optional keyword
    TEST_MATCH_COMMAND("ABc[:BCd][:CDe][:DEf]?", "ab:cd:de", FALSE); // test optional keyword
    TEST_MATCH_COMMAND("ABc[:BCd][:CDe][:DEf]?", "ab:bc", FALSE); // test optional keyword
    TEST_MATCH_COMMAND("ABc[:BCd][:CDe][:DEf]?", "ab:cd", FALSE); // test optional keyword
    TEST_MATCH_COMMAND("ABc[:BCd][:CDe][:DEf]?", "ab:de", FALSE); // test optional keyword
    TEST_MATCH_COMMAND("ABc[:BCd][:CDe][:DEf]?", "ab", FALSE); // test optional keyword   
    TEST_MATCH_COMMAND("*IDN?", "idn", FALSE); // common command
    TEST_MATCH_COMMAND("*IDN?", "idn?", FALSE); // common command
    TEST_MATCH_COMMAND("*IDN?", "*idn", FALSE); // common command
    TEST_MATCH_COMMAND("*IDN?", "*idn?", TRUE); // common command
    TEST_MATCH_COMMAND("*IDN?", ":idn", FALSE); // common command
    TEST_MATCH_COMMAND("*IDN?", ":idn?", FALSE); // common command
    TEST_MATCH_COMMAND("*IDN?", ":*idn", FALSE); // common command
    TEST_MATCH_COMMAND("*IDN?", ":*idn?", FALSE); // common command
}

void test_composeCompoundCommand(void) {
    
#define TEST_COMPOSE_COMMAND(b, c1_len, c2_pos, c2_len, c2_final, r)    \
    {                                                                   \
        char buffer[100];                                               \
        char * cmd_prev = buffer;                                       \
        char * cmd = buffer + c2_pos;                                   \
        size_t len_prev = c1_len;                                       \
        size_t len = c2_len;                                            \
        bool_t res;                                                     \
                                                                        \
        strcpy(buffer, b);                                              \
        res = composeCompoundCommand(cmd_prev, len_prev, &cmd, &len);   \
        CU_ASSERT_EQUAL(res, r);                                        \
        CU_ASSERT_EQUAL(len, strlen(c2_final));                         \
        CU_ASSERT_STRING_EQUAL(cmd, c2_final);                          \
    }\

    TEST_COMPOSE_COMMAND("A:B;C", 3, 4, 1, "A:C", TRUE);
    TEST_COMPOSE_COMMAND("A:B;DD", 3, 4, 2, "A:DD", TRUE);
    TEST_COMPOSE_COMMAND("A:B", 0, 0, 3, "A:B", TRUE);
    TEST_COMPOSE_COMMAND("*IDN? ; ABC", 5, 8, 3, "ABC", TRUE);
    TEST_COMPOSE_COMMAND("A:B;*IDN?", 3, 4, 5, "*IDN?", TRUE);
    TEST_COMPOSE_COMMAND("A:B;:C", 3, 4, 2, ":C", TRUE);
    TEST_COMPOSE_COMMAND("B;C", 1, 2, 1, "C", TRUE);
    TEST_COMPOSE_COMMAND("A:B;C:D", 3, 4, 3, "A:C:D", TRUE);
    TEST_COMPOSE_COMMAND(":A:B;C", 4, 5, 1, ":A:C", TRUE);
    TEST_COMPOSE_COMMAND(":A:B;:C", 4, 5, 2, ":C", TRUE);
    TEST_COMPOSE_COMMAND(":A;C", 2, 3, 1, ":C", TRUE);

    bool_t composeCompoundCommand(char * ptr_prev, size_t len_prev, char ** pptr, size_t * plen);

}

int main() {
    CU_pSuite pSuite = NULL;

    /* Initialize the CUnit test registry */
    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

    /* Add a suite to the registry */
    pSuite = CU_add_suite("SCPI Utils", init_suite, clean_suite);
    if (NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* Add the tests to the suite */
    if (0
            || (NULL == CU_add_test(pSuite, "strnpbrk", test_strnpbrk))
            || (NULL == CU_add_test(pSuite, "longToStr", test_longToStr))
            || (NULL == CU_add_test(pSuite, "doubleToStr", test_doubleToStr))
            || (NULL == CU_add_test(pSuite, "strToLong", test_strToLong))
            || (NULL == CU_add_test(pSuite, "strToDouble", test_strToDouble))
            || (NULL == CU_add_test(pSuite, "compareStr", test_compareStr))
            || (NULL == CU_add_test(pSuite, "locateText", test_locateText))
            || (NULL == CU_add_test(pSuite, "locateStr", test_locateStr))
            || (NULL == CU_add_test(pSuite, "matchPattern", test_matchPattern))
            || (NULL == CU_add_test(pSuite, "matchCommand", test_matchCommand))
            || (NULL == CU_add_test(pSuite, "composeCompoundCommand", test_composeCompoundCommand))
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
