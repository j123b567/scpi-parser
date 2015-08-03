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
#include "scpi/config.h"

#if HAVE_STDBOOL
#include <stdbool.h>
#endif

#ifdef  __cplusplus
extern "C" {
#endif

#if !HAVE_STDBOOL
   typedef unsigned char bool;
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
        SCPI_REG_SRE, /* Service Request Enable Register */
        SCPI_REG_ESR, /* Standard Event Status Register (ESR, SESR) */
        SCPI_REG_ESE, /* Event Status Enable Register */
        SCPI_REG_OPER, /* OPERation Status Register */
        SCPI_REG_OPERE, /* OPERation Status Enable Register */
        SCPI_REG_QUES, /* QUEStionable status register */
        SCPI_REG_QUESE, /* QUEStionable status Enable Register */

        /* last definition - number of registers */
        SCPI_REG_COUNT
    };
    typedef enum _scpi_reg_name_t scpi_reg_name_t;

    enum _scpi_ctrl_name_t {
        SCPI_CTRL_SRQ = 1, /* service request */
        SCPI_CTRL_GTL, /* Go to local */
        SCPI_CTRL_SDC, /* Selected device clear */
        SCPI_CTRL_PPC, /* Parallel poll configure */
        SCPI_CTRL_GET, /* Group execute trigger */
        SCPI_CTRL_TCT, /* Take control */
        SCPI_CTRL_LLO, /* Device clear */
        SCPI_CTRL_DCL, /* Local lockout */
        SCPI_CTRL_PPU, /* Parallel poll unconfigure */
        SCPI_CTRL_SPE, /* Serial poll enable */
        SCPI_CTRL_SPD, /* Serial poll disable */
        SCPI_CTRL_MLA, /* My local address */
        SCPI_CTRL_UNL, /* Unlisten */
        SCPI_CTRL_MTA, /* My talk address */
        SCPI_CTRL_UNT, /* Untalk */
        SCPI_CTRL_MSA /* My secondary address */
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

#define SCPI_CMD_LIST_END       {NULL, NULL, 0}

    /* scpi interface */
    typedef struct _scpi_t scpi_t;
    typedef struct _scpi_interface_t scpi_interface_t;

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

    typedef size_t(*scpi_write_t)(scpi_t * context, const char * data, size_t len);
    typedef scpi_result_t(*scpi_write_control_t)(scpi_t * context, scpi_ctrl_name_t ctrl, scpi_reg_val_t val);
    typedef int (*scpi_error_callback_t)(scpi_t * context, int_fast16_t error);

    /* scpi lexer */
    enum _scpi_token_type_t {
        SCPI_TOKEN_COMMA,
        SCPI_TOKEN_SEMICOLON,
        SCPI_TOKEN_QUESTION,
        SCPI_TOKEN_NL,
        SCPI_TOKEN_HEXNUM,
        SCPI_TOKEN_OCTNUM,
        SCPI_TOKEN_BINNUM,
        SCPI_TOKEN_PROGRAM_MNEMONIC,
        SCPI_TOKEN_DECIMAL_NUMERIC_PROGRAM_DATA,
        SCPI_TOKEN_DECIMAL_NUMERIC_PROGRAM_DATA_WITH_SUFFIX,
        SCPI_TOKEN_SUFFIX_PROGRAM_DATA,
        SCPI_TOKEN_ARBITRARY_BLOCK_PROGRAM_DATA,
        SCPI_TOKEN_SINGLE_QUOTE_PROGRAM_DATA,
        SCPI_TOKEN_DOUBLE_QUOTE_PROGRAM_DATA,
        SCPI_TOKEN_PROGRAM_EXPRESSION,
        SCPI_TOKEN_COMPOUND_PROGRAM_HEADER,
        SCPI_TOKEN_INCOMPLETE_COMPOUND_PROGRAM_HEADER,
        SCPI_TOKEN_COMMON_PROGRAM_HEADER,
        SCPI_TOKEN_INCOMPLETE_COMMON_PROGRAM_HEADER,
        SCPI_TOKEN_COMPOUND_QUERY_PROGRAM_HEADER,
        SCPI_TOKEN_COMMON_QUERY_PROGRAM_HEADER,
        SCPI_TOKEN_WS,
        SCPI_TOKEN_ALL_PROGRAM_DATA,
        SCPI_TOKEN_INVALID,
        SCPI_TOKEN_UNKNOWN,
    };
    typedef enum _scpi_token_type_t scpi_token_type_t;

    struct _scpi_token_t {
        scpi_token_type_t type;
        char * ptr;
        int len;
    };
    typedef struct _scpi_token_t scpi_token_t;

    struct _lex_state_t {
        char * buffer;
        char * pos;
        int len;
    };
    typedef struct _lex_state_t lex_state_t;

    /* scpi parser */
    enum _message_termination_t {
        SCPI_MESSAGE_TERMINATION_NONE,
        SCPI_MESSAGE_TERMINATION_NL,
        SCPI_MESSAGE_TERMINATION_SEMICOLON,
    };
    typedef enum _message_termination_t message_termination_t;

    struct _scpi_parser_state_t {
        scpi_token_t programHeader;
        scpi_token_t programData;
        int numberOfParameters;
        message_termination_t termination;
    };
    typedef struct _scpi_parser_state_t scpi_parser_state_t;

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
        SCPI_NUM_NINF,
        SCPI_NUM_AUTO
    };
    typedef enum _scpi_special_number_t scpi_special_number_t;

    struct _scpi_choice_def_t {
        const char * name;
        int32_t tag;
    };
#define SCPI_CHOICE_LIST_END   {NULL, -1}
    typedef struct _scpi_choice_def_t scpi_choice_def_t;

    struct _scpi_param_list_t {
        const scpi_command_t * cmd;
        lex_state_t lex_state;
        scpi_const_buffer_t cmd_raw;
    };
    typedef struct _scpi_param_list_t scpi_param_list_t;

    struct _scpi_number_parameter_t {
        scpi_bool_t special;
        union {
            double value;
            int32_t tag;
        };
        scpi_unit_t unit;
        int8_t base;
    };
    typedef struct _scpi_number_parameter_t scpi_number_t;

    struct _scpi_data_parameter_t {
        const char * ptr;
        int32_t len;
    };
    typedef struct _scpi_data_parameter_t scpi_data_parameter_t;

    typedef scpi_token_t scpi_parameter_t;

    struct _scpi_command_t {
        const char * pattern;
        scpi_command_callback_t callback;
        int32_t tag;
    };

    struct _scpi_interface_t {
        scpi_error_callback_t error;
        scpi_write_t write;
        scpi_write_control_t control;
        scpi_command_callback_t flush;
        scpi_command_callback_t reset;
    };

    struct _scpi_t {
        const scpi_command_t * cmdlist;
        scpi_buffer_t buffer;
        scpi_param_list_t param_list;
        scpi_interface_t * interface;
        int_fast16_t output_count;
        int_fast16_t input_count;
        scpi_bool_t cmd_error;
        scpi_error_queue_t error_queue;
        scpi_reg_val_t * registers;
        const scpi_unit_def_t * units;
        void * user_context;
        scpi_parser_state_t parser_state;
        const char * idn[4];
    };

#ifdef  __cplusplus
}
#endif

#endif  /* SCPI_TYPES_H */

