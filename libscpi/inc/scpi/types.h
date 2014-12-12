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

/**
 * @file   scpi_types.h
 * @date   Thu Nov 15 10:58:45 UTC 2012
 * 
 * @brief  SCPI data types
 * 
 * 
 */

#ifndef SCPI_TYPES_H
#define SCPI_TYPES_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef  __cplusplus
extern "C" {
#endif

#ifndef FALSE
    #define FALSE 0
#endif
#ifndef TRUE
    #define TRUE (!FALSE)
#endif

    /* basic data types */
    typedef bool scpi_bool_t;
    /* typedef enum { FALSE = 0, TRUE } scpi_bool_t; */

    /* IEEE 488.2 registers */
    enum _scpi_reg_name_t {
        SCPI_REG_STB = 0, /* Status Byte */
        SCPI_REG_SRE,     /* Service Request Enable Register */
        SCPI_REG_ESR,     /* Standard Event Status Register (ESR, SESR) */
        SCPI_REG_ESE,     /* Event Status Enable Register */
        SCPI_REG_OPER,    /* OPERation Status Register */
        SCPI_REG_OPERE,   /* OPERation Status Enable Register */
        SCPI_REG_QUES,    /* QUEStionable status register */
        SCPI_REG_QUESE,   /* QUEStionable status Enable Register */

        /* last definition - number of registers */
        SCPI_REG_COUNT
    };
    typedef enum _scpi_reg_name_t scpi_reg_name_t;

    enum _scpi_ctrl_name_t {
        SCPI_CTRL_SRQ = 1, /* service request */
        SCPI_CTRL_GTL,     /* Go to local */
        SCPI_CTRL_SDC,     /* Selected device clear */
        SCPI_CTRL_PPC,     /* Parallel poll configure */
        SCPI_CTRL_GET,     /* Group execute trigger */
        SCPI_CTRL_TCT,     /* Take control */
        SCPI_CTRL_LLO,     /* Device clear */
        SCPI_CTRL_DCL,     /* Local lockout */
        SCPI_CTRL_PPU,     /* Parallel poll unconfigure */
        SCPI_CTRL_SPE,     /* Serial poll enable */
        SCPI_CTRL_SPD,     /* Serial poll disable */
        SCPI_CTRL_MLA,     /* My local address */
        SCPI_CTRL_UNL,     /* Unlisten */
        SCPI_CTRL_MTA,     /* My talk address */
        SCPI_CTRL_UNT,     /* Untalk */
        SCPI_CTRL_MSA      /* My secondary address */
    };
    typedef enum _scpi_ctrl_name_t scpi_ctrl_name_t;

    typedef uint16_t scpi_reg_val_t;

    /* scpi commands */
    enum _scpi_result_t {
        SCPI_RES_OK = 1,
        SCPI_RES_ERR = -1
    };
    typedef enum _scpi_result_t scpi_result_t;

    typedef struct _scpi_command_t scpi_command_t;

    struct _scpi_buffer_t {
        size_t length;
        size_t position;
        char * data;
    };
    typedef struct _scpi_buffer_t scpi_buffer_t;
    
    struct _scpi_const_buffer_t {
        size_t length;
        size_t position;
        const char * data;
    };
    typedef struct _scpi_const_buffer_t scpi_const_buffer_t;    

    struct _scpi_param_list_t {
        const scpi_command_t * cmd;
        const char * parameters;
        size_t length;
        scpi_const_buffer_t cmd_raw;
    };
    #define SCPI_CMD_LIST_END       {NULL, NULL, }
    typedef struct _scpi_param_list_t scpi_param_list_t;

    /* scpi interface */
    typedef struct _scpi_t scpi_t;
    typedef struct _scpi_interface_t scpi_interface_t;

    typedef size_t(*scpi_write_t)(scpi_t * context, const char * data, size_t len);
    typedef scpi_result_t(*scpi_write_control_t)(scpi_t * context, scpi_ctrl_name_t ctrl, scpi_reg_val_t val);
    typedef int (*scpi_error_callback_t)(scpi_t * context, int_fast16_t error);

    typedef scpi_result_t(*scpi_command_callback_t)(scpi_t *);

    /* scpi error queue */
    typedef void * scpi_error_queue_t;

    /* scpi units */
    enum _scpi_unit_t {
        SCPI_UNIT_NONE,
        SCPI_UNIT_VOLT,
        SCPI_UNIT_AMPER,
        SCPI_UNIT_OHM,
        SCPI_UNIT_HERTZ,
        SCPI_UNIT_CELSIUS,
        SCPI_UNIT_SECONDS,
        SCPI_UNIT_DISTANCE
    };
    typedef enum _scpi_unit_t scpi_unit_t;

    struct _scpi_unit_def_t {
        const char * name;
        scpi_unit_t unit;
        double mult;
    };
    #define SCPI_UNITS_LIST_END       {NULL, SCPI_UNIT_NONE, 0}
    typedef struct _scpi_unit_def_t scpi_unit_def_t;

    enum _scpi_special_number_t {
        SCPI_NUM_NUMBER,
        SCPI_NUM_MIN,
        SCPI_NUM_MAX,
        SCPI_NUM_DEF,
        SCPI_NUM_UP,
        SCPI_NUM_DOWN,
        SCPI_NUM_NAN,
        SCPI_NUM_INF,
        SCPI_NUM_NINF
    };
    typedef enum _scpi_special_number_t scpi_special_number_t;

    struct _scpi_special_number_def_t {
        const char * name;
        scpi_special_number_t type;
    };
    #define SCPI_SPECIAL_NUMBERS_LIST_END   {NULL, SCPI_NUM_NUMBER}    
    typedef struct _scpi_special_number_def_t scpi_special_number_def_t;

    struct _scpi_number_t {
        double value;
        scpi_unit_t unit;
        scpi_special_number_t type;
    };
    typedef struct _scpi_number_t scpi_number_t;

    struct _scpi_command_t {
        const char * pattern;
        scpi_command_callback_t callback;
    };

    struct _scpi_interface_t {
        scpi_error_callback_t error;
        scpi_write_t write;
        scpi_write_control_t control;
        scpi_command_callback_t flush;
        scpi_command_callback_t reset;
        scpi_command_callback_t test;
    };

    struct _scpi_t {
        const scpi_command_t * cmdlist;
        scpi_buffer_t buffer;
        scpi_param_list_t paramlist;
        scpi_interface_t * interface;
        int_fast16_t output_count;
        int_fast16_t input_count;
        scpi_bool_t cmd_error;
        scpi_error_queue_t error_queue;
        scpi_reg_val_t * registers;
        const scpi_unit_def_t * units;
        const scpi_special_number_def_t * special_numbers;
        void * user_context;
        const char * idn[4];
    };

#ifdef  __cplusplus
}
#endif

#endif  /* SCPI_TYPES_H */

