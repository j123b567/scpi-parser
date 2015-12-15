/*
 * File:   test_fifo.c
 * Author: jaybee
 *
 * Created on 27.11.2012, 16:01:38
 */

#include <stdio.h>
#include <stdlib.h>
#include "CUnit/Basic.h"

#include "../src/fifo_private.h"

/*
 * CUnit Test Suite
 */

static int init_suite(void) {
    return 0;
}

static int clean_suite(void) {
    return 0;
}

static void testFifo() {
    scpi_fifo_t fifo;
    int16_t fifo_data[5];
    fifo_init(&fifo, fifo_data, 5);
    int16_t value;

#define TEST_FIFO_COUNT(n)                      \
    do {                                        \
        fifo_count(&fifo, &value);              \
        CU_ASSERT_EQUAL(value, n);              \
    } while(0)                                  \


    TEST_FIFO_COUNT(0);
    CU_ASSERT_TRUE(fifo_is_empty(&fifo));
    CU_ASSERT_FALSE(fifo_is_full(&fifo));

    CU_ASSERT_TRUE(fifo_add(&fifo, 1));
    TEST_FIFO_COUNT(1);
    CU_ASSERT_FALSE(fifo_is_empty(&fifo));
    CU_ASSERT_FALSE(fifo_is_full(&fifo));

    CU_ASSERT_TRUE(fifo_add(&fifo, 2));
    TEST_FIFO_COUNT(2);
    CU_ASSERT_FALSE(fifo_is_empty(&fifo));
    CU_ASSERT_FALSE(fifo_is_full(&fifo));

    CU_ASSERT_TRUE(fifo_add(&fifo, 3));
    TEST_FIFO_COUNT(3);
    CU_ASSERT_FALSE(fifo_is_empty(&fifo));
    CU_ASSERT_FALSE(fifo_is_full(&fifo));

    CU_ASSERT_TRUE(fifo_add(&fifo, 4));
    TEST_FIFO_COUNT(4);
    CU_ASSERT_FALSE(fifo_is_empty(&fifo));
    CU_ASSERT_TRUE(fifo_is_full(&fifo));

    CU_ASSERT_EQUAL(fifo.data[0], 1);
    CU_ASSERT_EQUAL(fifo.data[1], 2);
    CU_ASSERT_EQUAL(fifo.data[2], 3);
    CU_ASSERT_EQUAL(fifo.data[3], 4);


    CU_ASSERT_FALSE(fifo_add(&fifo, 5));
    TEST_FIFO_COUNT(4);
    CU_ASSERT_FALSE(fifo_is_empty(&fifo));
    CU_ASSERT_TRUE(fifo_is_full(&fifo));

    CU_ASSERT_EQUAL(fifo.data[0], 1);
    CU_ASSERT_EQUAL(fifo.data[1], 2);
    CU_ASSERT_EQUAL(fifo.data[2], 3);
    CU_ASSERT_EQUAL(fifo.data[3], 4);

    CU_ASSERT_TRUE(fifo_remove_last(&fifo, &value));
    CU_ASSERT_EQUAL(value, 4);
    TEST_FIFO_COUNT(3);
    CU_ASSERT_FALSE(fifo_is_empty(&fifo));
    CU_ASSERT_FALSE(fifo_is_full(&fifo));

    CU_ASSERT_TRUE(fifo_add(&fifo, 6));
    TEST_FIFO_COUNT(4);
    CU_ASSERT_FALSE(fifo_is_empty(&fifo));
    CU_ASSERT_TRUE(fifo_is_full(&fifo));

    CU_ASSERT_EQUAL(fifo.data[0], 1);
    CU_ASSERT_EQUAL(fifo.data[1], 2);
    CU_ASSERT_EQUAL(fifo.data[2], 3);
    CU_ASSERT_EQUAL(fifo.data[3], 6);

    CU_ASSERT_TRUE(fifo_remove(&fifo, &value));
    CU_ASSERT_EQUAL(value, 1);
    TEST_FIFO_COUNT(3);
    CU_ASSERT_FALSE(fifo_is_empty(&fifo));
    CU_ASSERT_FALSE(fifo_is_full(&fifo));

    CU_ASSERT_TRUE(fifo_add(&fifo, 7));
    TEST_FIFO_COUNT(4);

    CU_ASSERT_TRUE(fifo_remove(&fifo, &value));
    CU_ASSERT_EQUAL(value, 2);
    TEST_FIFO_COUNT(3);

    CU_ASSERT_TRUE(fifo_remove(&fifo, &value));
    CU_ASSERT_EQUAL(value, 3);
    TEST_FIFO_COUNT(2);

    CU_ASSERT_TRUE(fifo_add(&fifo, 10));
    TEST_FIFO_COUNT(3);

    CU_ASSERT_TRUE(fifo_add(&fifo, 11));
    TEST_FIFO_COUNT(4);

    CU_ASSERT_TRUE(fifo_remove(&fifo, &value));
    CU_ASSERT_EQUAL(value, 6);
    TEST_FIFO_COUNT(3);

    CU_ASSERT_TRUE(fifo_remove(&fifo, &value));
    CU_ASSERT_EQUAL(value, 7);
    TEST_FIFO_COUNT(2);

    CU_ASSERT_TRUE(fifo_remove_last(&fifo, &value));
    CU_ASSERT_EQUAL(value, 11);
    TEST_FIFO_COUNT(1);

    CU_ASSERT_TRUE(fifo_remove(&fifo, &value));
    CU_ASSERT_EQUAL(value, 10);
    TEST_FIFO_COUNT(0);

    CU_ASSERT_FALSE(fifo_remove(&fifo, &value));
    TEST_FIFO_COUNT(0);

    CU_ASSERT_FALSE(fifo_remove_last(&fifo, NULL));
}

int main() {
    unsigned int result;
    CU_pSuite pSuite = NULL;

    /* Initialize the CUnit test registry */
    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

    /* Add a suite to the registry */
    pSuite = CU_add_suite("FIFO", init_suite, clean_suite);
    if (NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* Add the tests to the suite */
    if ((NULL == CU_add_test(pSuite, "test fifo", testFifo))) {
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
