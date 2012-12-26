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
 * @file   scpi_types.h
 * @date   Thu Nov 15 10:58:45 UTC 2012
 * 
 * @brief  SCPI data types
 * 
 * 
 */

#ifndef SCPI_TYPES_H
#define	SCPI_TYPES_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef	__cplusplus
extern "C" {
#endif

#define FALSE false
#define TRUE true

    /* basic data types */
    typedef bool bool_t;
    //typedef enum { FALSE = 0, TRUE } bool_t;

    /* scpi interface */
    typedef struct _scpi_t scpi_t;
    typedef struct _scpi_interface_t scpi_interface_t;
    typedef struct _scpi_buffer_t scpi_buffer_t;
    typedef size_t(*scpi_write_t)(scpi_t * context, const char * data, size_t len);
    typedef int (*scpi_error_callback_t)(scpi_t * context, int_fast16_t error);

    /* scpi commands */
    typedef enum _scpi_result_t scpi_result_t;
    typedef struct _scpi_param_list_t scpi_param_list_t;
    typedef struct _scpi_command_t scpi_command_t;
    typedef scpi_result_t(*scpi_command_callback_t)(scpi_t *);

    /* scpi error queue */
    typedef void * scpi_error_queue_t;

    /* scpi units */
    typedef enum _scpi_unit_t scpi_unit_t;
    typedef struct _scpi_unit_def_t scpi_unit_def_t;
    typedef enum _scpi_special_number_t scpi_special_number_t;
    typedef struct _scpi_special_number_def_t scpi_special_number_def_t;
    typedef struct _scpi_number_t scpi_number_t;

    /* IEEE 488.2 registers */
    typedef enum _scpi_reg_name_t scpi_reg_name_t;
    typedef uint16_t scpi_reg_val_t;
    
    struct _scpi_param_list_t {
        const scpi_command_t * cmd;
        const char * parameters;
        size_t length;
    };


#define SCPI_CMD_LIST_END       {.pattern = NULL, .callback = NULL, }

    struct _scpi_command_t {
        const char * pattern;
        scpi_command_callback_t callback;
    };

    struct _scpi_buffer_t {
        size_t length;
        size_t position;
        char * data;
    };

    struct _scpi_interface_t {
        scpi_error_callback_t error;
        scpi_write_t write;
        scpi_command_callback_t flush;
        scpi_command_callback_t reset;
        scpi_command_callback_t test;
        scpi_command_callback_t srq;
    };

    struct _scpi_t {
        const scpi_command_t * cmdlist;
        scpi_buffer_t buffer;
        scpi_param_list_t paramlist;
        scpi_interface_t * interface;
        int_fast16_t output_count;
        int_fast16_t input_count;
        bool_t cmd_error;
        scpi_error_queue_t error_queue;
        scpi_reg_val_t * registers;
        const scpi_unit_def_t * units;
        const scpi_special_number_def_t * special_numbers;
        void * user_context;
    };

    enum _scpi_unit_t {
        SCPI_UNIT_NONE,
        SCPI_UNIT_VOLT,
        SCPI_UNIT_AMPER,
        SCPI_UNIT_OHM,
        SCPI_UNIT_HERTZ,
        SCPI_UNIT_CELSIUS,
        SCPI_UNIT_SECONDS,
    };


#define SCPI_UNITS_LIST_END       {.name = NULL, .unit = SCPI_UNIT_NONE, .mult = 0}

    struct _scpi_unit_def_t {
        const char * name;
        scpi_unit_t unit;
        double mult;
    };

    enum _scpi_special_number_t {
        SCPI_NUM_NUMBER,
        SCPI_NUM_MIN,
        SCPI_NUM_MAX,
        SCPI_NUM_DEF,
        SCPI_NUM_UP,
        SCPI_NUM_DOWN,
        SCPI_NUM_NAN,
        SCPI_NUM_INF,
        SCPI_NUM_NINF,
    };

#define SCPI_SPECIAL_NUMBERS_LIST_END   {.name = NULL, .type = SCPI_NUM_NUMBER}    

    struct _scpi_special_number_def_t {
        const char * name;
        scpi_special_number_t type;
    };

    struct _scpi_number_t {
        double value;
        scpi_unit_t unit;
        scpi_special_number_t type;
    };

    enum _scpi_result_t {
        SCPI_RES_OK = 1,
        SCPI_RES_ERR = -1,
    };


    enum _scpi_reg_name_t {
        SCPI_REG_STB = 0, // Status Byte
        SCPI_REG_SRE, // Service Request Enable Register
        SCPI_REG_ESR, // Standard Event Status Register (ESR, SESR)
        SCPI_REG_ESE, // Event Status Enable Register
        SCPI_REG_OPER, // OPERation Status Register
        SCPI_REG_OPERE, // OPERation Status Enable Register
        SCPI_REG_QUES, // QUEStionable status register
        SCPI_REG_QUESE, // QUEStionable status Enable Register

        /* last definition - number of registers */
        SCPI_REG_COUNT,
    };


#ifdef	__cplusplus
}
#endif

#endif	/* SCPI_TYPES_H */

