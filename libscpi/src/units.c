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
 * @file   scpi_units.c
 * @date   Thu Nov 15 10:58:45 UTC 2012
 * 
 * @brief  SCPI units
 * 
 * 
 */

#include <string.h>
#include "scpi/parser.h"
#include "scpi/units.h"
#include "scpi/utils_private.h"
#include "scpi/error.h"


/*
 * multipliers IEEE 488.2-1992 tab 7-2
 * 1E18         EX
 * 1E15         PE
 * 1E12         T
 * 1E9          G
 * 1E6          MA (use M for OHM and HZ)
 * 1E3          K
 * 1E-3         M (disaalowed for OHM and HZ)
 * 1E-6         U
 * 1E-9         N
 * 1E-12        P
 * 1E-15        F
 * 1E-18        A
 */

/*
 * units definition IEEE 488.2-1992 tab 7-1
 */
const scpi_unit_def_t scpi_units_def[] = {
    /* voltage */
    {/* name */ "UV",   /* unit */ SCPI_UNIT_VOLT,      /* mult */ 1e-6},
    {/* name */ "MV",   /* unit */ SCPI_UNIT_VOLT,      /* mult */ 1e-3},
    {/* name */ "V",    /* unit */ SCPI_UNIT_VOLT,      /* mult */ 1},
    {/* name */ "KV",   /* unit */ SCPI_UNIT_VOLT,      /* mult */ 1e3},

    /* current */
    {/* name */ "UA",   /* unit */ SCPI_UNIT_AMPER,     /* mult */ 1e-6},
    {/* name */ "MA",   /* unit */ SCPI_UNIT_AMPER,     /* mult */ 1e-3},
    {/* name */ "A",    /* unit */ SCPI_UNIT_AMPER,     /* mult */ 1},
    {/* name */ "KA",   /* unit */ SCPI_UNIT_AMPER,     /* mult */ 1e3},

    /* resistance */
    {/* name */ "OHM",  /* unit */ SCPI_UNIT_OHM,       /* mult */ 1},
    {/* name */ "KOHM", /* unit */ SCPI_UNIT_OHM,       /* mult */ 1e3},
    {/* name */ "MOHM", /* unit */ SCPI_UNIT_OHM,       /* mult */ 1e6},

    /* frequency */
    {/* name */ "HZ",   /* unit */ SCPI_UNIT_HERTZ,     /* mult */ 1},
    {/* name */ "KHZ",  /* unit */ SCPI_UNIT_HERTZ,     /* mult */ 1e3},
    {/* name */ "MHZ",  /* unit */ SCPI_UNIT_HERTZ,     /* mult */ 1e6},
    {/* name */ "GHZ",  /* unit */ SCPI_UNIT_HERTZ,     /* mult */ 1e9},

    /* temperature */
    {/* name */ "CEL",  /* unit */ SCPI_UNIT_CELSIUS,   /* mult */ 1},

    /* time */
    {/* name */ "PS",   /* unit */ SCPI_UNIT_SECONDS,   /* mult */ 1e-12},
    {/* name */ "NS",   /* unit */ SCPI_UNIT_SECONDS,   /* mult */ 1e-9},
    {/* name */ "US",   /* unit */ SCPI_UNIT_SECONDS,   /* mult */ 1e-6},
    {/* name */ "MS",   /* unit */ SCPI_UNIT_SECONDS,   /* mult */ 1e-3},
    {/* name */ "S",    /* unit */ SCPI_UNIT_SECONDS,   /* mult */ 1},
    {/* name */ "MIN",  /* unit */ SCPI_UNIT_SECONDS,   /* mult */ 60},
    {/* name */ "HR",   /* unit */ SCPI_UNIT_SECONDS,   /* mult */ 3600},

    SCPI_UNITS_LIST_END,
};

/*
 * Special number values definition
 */
const scpi_special_number_def_t scpi_special_numbers_def[] = {
    {/* name */ "MINimum",      /* type */ SCPI_NUM_MIN},
    {/* name */ "MAXimum",      /* type */ SCPI_NUM_MAX},
    {/* name */ "DEFault",      /* type */ SCPI_NUM_DEF},
    {/* name */ "UP",           /* type */ SCPI_NUM_UP},
    {/* name */ "DOWN",         /* type */ SCPI_NUM_DOWN},
    {/* name */ "NAN",          /* type */ SCPI_NUM_NAN},
    {/* name */ "INFinity",     /* type */ SCPI_NUM_INF},
    {/* name */ "NINF",         /* type */ SCPI_NUM_NINF},
    SCPI_SPECIAL_NUMBERS_LIST_END,
};

/**
 * Match string constant to one of special number values
 * @param specs specifications of special numbers (patterns)
 * @param str string to be recognised
 * @param len length of string
 * @param value resultin value
 * @return TRUE if str matches one of specs patterns
 */
static scpi_bool_t translateSpecialNumber(const scpi_special_number_def_t * specs, const char * str, size_t len, scpi_number_t * value) {
    int i;

    value->value = 0.0;
    value->unit = SCPI_UNIT_NONE;
    value->type = SCPI_NUM_NUMBER;

    if (specs == NULL) {
        return FALSE;
    }

    for (i = 0; specs[i].name != NULL; i++) {
        if (matchPattern(specs[i].name, strlen(specs[i].name), str, len)) {
            value->type = specs[i].type;
            return TRUE;
        }
    }

    return FALSE;
}

/**
 * Convert special number type to its string representation
 * @param specs specifications of special numbers (patterns)
 * @param type type of special number
 * @return String representing special number or NULL
 */
static const char * translateSpecialNumberInverse(const scpi_special_number_def_t * specs, scpi_special_number_t type) {
    int i;

    if (specs == NULL) {
        return NULL;
    }
    
    for (i = 0; specs[i].name != NULL; i++) {
        if (specs[i].type == type) {
            return specs[i].name;
        }
    }

    return NULL;
}

/**
 * Convert string describing unit to its representation
 * @param units units patterns
 * @param unit text representation of unknown unit
 * @param len length of text representation
 * @return pointer of related unit definition or NULL
 */
static const scpi_unit_def_t * translateUnit(const scpi_unit_def_t * units, const char * unit, size_t len) {
    int i;
    
    if (units == NULL) {
        return NULL;
    }
    
    for (i = 0; units[i].name != NULL; i++) {
        if (compareStr(unit, len, units[i].name, strlen(units[i].name))) {
            return &units[i];
        }
    }

    return NULL;
}

/**
 * Convert unit definition to string
 * @param units units definitions (patterns)
 * @param unit type of unit
 * @return string representation of unit
 */
static const char * translateUnitInverse(const scpi_unit_def_t * units, const scpi_unit_t unit) {
    int i;
    
    if (units == NULL) {
        return NULL;
    }
    
    for (i = 0; units[i].name != NULL; i++) {
        if ((units[i].unit == unit) && (units[i].mult == 1)) {
            return units[i].name;
        }
    }

    return NULL;
}

/**
 * Transform number to base units
 * @param context
 * @param unit text representation of unit
 * @param len length of text representation
 * @param value preparsed numeric value
 * @return TRUE if value parameter was converted to base units
 */
static scpi_bool_t transformNumber(scpi_t * context, const char * unit, size_t len, scpi_number_t * value) {
    size_t s;
    const scpi_unit_def_t * unitDef;
    s = skipWhitespace(unit, len);

    if (s == len) {
        value->unit = SCPI_UNIT_NONE;
        return TRUE;
    }

    unitDef = translateUnit(context->units, unit + s, len - s);

    if (unitDef == NULL) {
        SCPI_ErrorPush(context, SCPI_ERROR_INVALID_SUFFIX);
        return FALSE;
    }

    value->value *= unitDef->mult;
    value->unit = unitDef->unit;

    return TRUE;
}

/**
 * Parse parameter as number, number with unit or special value (min, max, default, ...)
 * @param context
 * @param value return value
 * @param mandatory if the parameter is mandatory
 * @return 
 */
scpi_bool_t SCPI_ParamNumber(scpi_t * context, scpi_number_t * value, scpi_bool_t mandatory) {
    scpi_bool_t result;
    const char * param;
    size_t len;
    size_t numlen;

    /* read parameter and shift to the next one */
    result = SCPI_ParamString(context, &param, &len, mandatory);

    /* value not initializes */
    if (!value) {
        return FALSE;
    }

    value->type = SCPI_NUM_DEF;

    /* if parameter was not found, return TRUE or FALSE according
     * to fact that parameter was mandatory or not */
    if (!result) {
        return mandatory ? FALSE : TRUE;
    }

    /* convert string to special number type */
    if (translateSpecialNumber(context->special_numbers, param, len, value)) {
        /* found special type */
        return TRUE;
    }

    /* convert text from double - no special type */
    numlen = strToDouble(param, &value->value);

    /* transform units of value */
    if (numlen <= len) {
        return transformNumber(context, param + numlen, len - numlen, value);
    }
    return FALSE;

}

/**
 * Convert scpi_number_t to string
 * @param context
 * @param value number value
 * @param str target string
 * @param len max length of string
 * @return number of chars written to string
 */
size_t SCPI_NumberToStr(scpi_t * context, scpi_number_t * value, char * str, size_t len) {
    const char * type;
    const char * unit;
    size_t result;

    if (!value || !str) {
        return 0;
    }

    type = translateSpecialNumberInverse(context->special_numbers, value->type);

    if (type) {
        strncpy(str, type, len);
        return min(strlen(type), len);
    }

    result = doubleToStr(value->value, str, len);

    unit = translateUnitInverse(context->units, value->unit);

    if (unit) {
        strncat(str, " ", len);
        strncat(str, unit, len);
        result += strlen(unit) + 1;
    }

    return result;
}
