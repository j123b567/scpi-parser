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
#include "../src/utils_private.h"

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

    len = longToStr(10, str, 32, 10);
    CU_ASSERT(len == 2);
    CU_ASSERT_STRING_EQUAL(str, "10");
    CU_ASSERT(str[len] == '\0');

    len = longToStr(10, str, 32, 2);
    CU_ASSERT(len == 4);
    CU_ASSERT(str[0] == '1');
    CU_ASSERT(str[1] == '0');
    CU_ASSERT(str[2] == '1');
    CU_ASSERT(str[3] == '0');
    CU_ASSERT(str[4] == '\0');

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

#define TEST_STR_TO_LONG(s, r, v, b)                    \
    do {                                                \
        result = strToLong(s, &val, b);                 \
        CU_ASSERT_EQUAL(val, v);                        \
        CU_ASSERT_EQUAL(result, r);                     \
    } while(0)                                          \
    
    TEST_STR_TO_LONG("", 0, 0, 10);
    TEST_STR_TO_LONG("1", 1, 1, 10);
    TEST_STR_TO_LONG("10", 2, 10, 10);
    TEST_STR_TO_LONG("-50", 3, -50, 10);
    TEST_STR_TO_LONG("100MHz", 3, 100, 10);
    TEST_STR_TO_LONG("MHz", 0, 0, 10);
    TEST_STR_TO_LONG("1.4", 1, 1, 10);
    TEST_STR_TO_LONG(" 1", 2, 1, 10);
    TEST_STR_TO_LONG(" +100", 5, 100, 10); // space and +
    TEST_STR_TO_LONG("FF", 2, 255, 16); // hexadecimal FF
    TEST_STR_TO_LONG("77", 2, 63, 8); // octal 77
    TEST_STR_TO_LONG("18", 1, 1, 8); // octal 1, 8 is ignored
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

void test_compareStrAndNum() {

    CU_ASSERT_TRUE(compareStrAndNum("abcd", 1, "afgh", 1));
    CU_ASSERT_TRUE(compareStrAndNum("ABCD", 4, "abcd", 4));
    CU_ASSERT_TRUE(compareStrAndNum("AbCd", 3, "AbCE", 3));
    CU_ASSERT_TRUE(compareStrAndNum("ABCD", 1, "a", 1));

    CU_ASSERT_FALSE(compareStrAndNum("abcd", 1, "efgh", 1));
    CU_ASSERT_FALSE(compareStrAndNum("ABCD", 4, "abcd", 3));

    CU_ASSERT_TRUE(compareStrAndNum("abcd", 4, "abcd1", 5));
    CU_ASSERT_TRUE(compareStrAndNum("abcd", 4, "abcd123", 7));
    CU_ASSERT_FALSE(compareStrAndNum("abcd", 4, "abcd12A", 7));
    CU_ASSERT_FALSE(compareStrAndNum("abcd", 4, "abcdB12", 7));
    CU_ASSERT_FALSE(compareStrAndNum("abdd", 4, "abcd132", 7));

}

void test_matchPattern() {
    scpi_bool_t result;

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
    scpi_bool_t result;

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

    TEST_MATCH_COMMAND("ABCdef#", "abc", TRUE); // test numeric parameter
    TEST_MATCH_COMMAND("ABCdef#", "abc1324", TRUE); // test numeric parameter
    TEST_MATCH_COMMAND("ABCdef#", "abcDef1324", TRUE); // test numeric parameter
    TEST_MATCH_COMMAND("ABCdef#", "abcDef124b", FALSE); // test numeric parameter

    TEST_MATCH_COMMAND("OUTPut#:MODulation#:FM#", "abc", FALSE); // test numeric parameter
    TEST_MATCH_COMMAND("OUTPut#:MODulation#:FM#", "outp1:mod10:fm", TRUE); // test numeric parameter
    TEST_MATCH_COMMAND("OUTPut#:MODulation#:FM#", "output1:mod10:fm", TRUE); // test numeric parameter
    TEST_MATCH_COMMAND("OUTPut#:MODulation#:FM#", "outp1:modulation:fm5", TRUE); // test numeric parameter
    TEST_MATCH_COMMAND("OUTPut#:MODulation#:FM#", "output:mod:fm", TRUE); // test numeric parameter
    TEST_MATCH_COMMAND("OUTPut#:MODulation#:FM#", "outp1:mod10a:fm", FALSE); // test numeric parameter
    TEST_MATCH_COMMAND("OUTPut#[:MODulation#]:FM#", "outp1:fm", TRUE); // test numeric parameter
    TEST_MATCH_COMMAND("OUTPut#[:MODulation#]:FM#", "outp1:mod10:fm", TRUE); // test numeric parameter
    TEST_MATCH_COMMAND("OUTPut#[:MODulation#]:FM#", "outp1:fm2", TRUE); // test numeric parameter
    TEST_MATCH_COMMAND("OUTPut#[:MODulation#]:FM#", "output:fm", TRUE); // test numeric parameter    
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
            || (NULL == CU_add_test(pSuite, "compareStrAndNum", test_compareStrAndNum))
            || (NULL == CU_add_test(pSuite, "matchPattern", test_matchPattern))
            || (NULL == CU_add_test(pSuite, "matchCommand", test_matchCommand))
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
