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
 * @file   scpi.h
 * @date   Thu Nov 15 10:58:45 UTC 2012
 * 
 * @brief  SCPI parser implementation
 * 
 * 
 */

#ifndef SCPI_H
#define	SCPI_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef	__cplusplus
extern "C" {
#endif

#define FALSE false
#define TRUE true
    typedef bool bool_t;
    //typedef enum { FALSE = 0, TRUE } bool_t;

    typedef struct _scpi_context_t scpi_context_t;
    typedef struct _scpi_param_list_t scpi_param_list_t;
    typedef struct _scpi_command_t scpi_command_t;
    typedef struct _scpi_buffer_t scpi_buffer_t;
    typedef struct _scpi_interface_t scpi_interface_t;
    typedef int (*scpi_command_callback_t)(scpi_context_t *);
    typedef size_t(*scpi_write_t)(scpi_context_t * context, const char * data, size_t len);
    typedef int (*scpi_error_callback_t)(scpi_context_t * context, int_fast16_t error);

    struct _scpi_param_list_t {
        const scpi_command_t * cmd;
        const char * parameters;
        size_t length;
    };

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
        scpi_command_callback_t reset;
        scpi_command_callback_t test;
    };

    struct _scpi_context_t {
        const scpi_command_t * cmdlist;
        scpi_buffer_t buffer;
        scpi_param_list_t paramlist;
        scpi_interface_t * interface;
        int_fast16_t output_count;
        int_fast16_t input_count;
        bool_t error;
    };



#define SCPI_CMD_LIST_END       {.pattern = NULL, .callback = NULL, }


    void SCPI_Init(scpi_context_t * context, scpi_command_t * command_list, scpi_buffer_t * buffer, scpi_interface_t * interface);

    int SCPI_Input(scpi_context_t * context, const char * data, size_t len);
    int SCPI_Parse(scpi_context_t * context, const char * data, size_t len);


    size_t SCPI_ResultString(scpi_context_t * context, const char * data);
    size_t SCPI_ResultInt(scpi_context_t * context, int32_t val);
    size_t SCPI_ResultDouble(scpi_context_t * context, double val);
    size_t SCPI_ResultText(scpi_context_t * context, const char * data);

    bool_t SCPI_ParamInt(scpi_context_t * context, int32_t * value, bool_t mandatory);
    bool_t SCPI_ParamDouble(scpi_context_t * context, double * value, bool_t mandatory);
    bool_t SCPI_ParamString(scpi_context_t * context, char ** value, size_t * len, bool_t mandatory);
    
    bool_t SCPI_DebugCommand(scpi_context_t * context);

    //#define SCPI_DEBUG_COMMAND(a)   scpi_debug_command(a)
#define SCPI_DEBUG_COMMAND(a)   


#ifdef	__cplusplus
}
#endif

#endif	/* SCPI_H */

