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
 * @file   scpi_units.h
 * @date   Thu Nov 15 10:58:45 UTC 2012
 * 
 * @brief  SCPI Units
 * 
 * 
 */

#ifndef SCPI_UNITS_H
#define	SCPI_UNITS_H

#include "scpi.h"

#ifdef	__cplusplus
extern "C" {
#endif

    enum _scpi_unit_t {
        SCPI_UNIT_NONE,
        SCPI_UNIT_VOLT,
        SCPI_UNIT_AMPER,
        SCPI_UNIT_OHM,
        SCPI_UNIT_HERTZ,
        SCPI_UNIT_CELSIUS,
        SCPI_UNIT_SECONDS,
    };
    typedef enum _scpi_unit_t scpi_unit_t;

    struct _scpi_unit_def_t {
        const char * name;
        scpi_unit_t unit;
        double mult;
    };
    typedef struct _scpi_unit_def_t scpi_unit_def_t;

#define SCPI_UNITS_LIST_END       {.name = NULL, .unit = SCPI_UNIT_NONE, .mult = 0}

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
    typedef enum _scpi_special_number_t scpi_special_number_t;

    struct _scpi_special_number_def_t {
        const char * name;
        scpi_special_number_t type;
    };
    typedef struct _scpi_special_number_def_t scpi_special_number_def_t;

    struct _scpi_number_t {
        double value;
        scpi_unit_t unit;
        scpi_special_number_t type;
    };
    typedef struct _scpi_number_t scpi_number_t;


#define SCPI_SPECIAL_NUMBERS_LIST_END   {.name = NULL, .type = SCPI_NUM_NUMBER}


    bool_t SCPI_ParamNumber(scpi_t * context, scpi_number_t * value, bool_t mandatory);
    size_t SCPI_NumberToStr(scpi_t * context, scpi_number_t * value, char * str, size_t len);

#ifdef	__cplusplus
}
#endif

#endif	/* SCPI_UNITS_H */

