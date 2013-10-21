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
 * @file   scpi_error.h
 * @date   Thu Nov 15 10:58:45 UTC 2012
 *
 * @brief  Error handling and storing routines
 *
 *
 */

#ifndef SCPI_ERROR_H
#define	SCPI_ERROR_H

#include "scpi/types.h"

#ifdef	__cplusplus
extern "C" {
#endif

    void SCPI_ErrorInit(scpi_t * context);
    void SCPI_ErrorClear(scpi_t * context);
    int16_t SCPI_ErrorPop(scpi_t * context);
    void SCPI_ErrorPush(scpi_t * context, int16_t err);
    int32_t SCPI_ErrorCount(scpi_t * context);
    const char * SCPI_ErrorTranslate(int16_t err);

/* http://en.wikipedia.org/wiki/X_Macro */
#define LIST_OF_ERRORS \
    X(SCPI_ERROR_SYNTAX,               -102, "Syntax error")                   \
    X(SCPI_ERROR_INVALID_SEPARATOR,    -103, "Invalid separator")              \
    X(SCPI_ERROR_UNDEFINED_HEADER,     -113, "Undefined header")               \
    X(SCPI_ERROR_PARAMETER_NOT_ALLOWED,-108, "Parameter not allowed")          \
    X(SCPI_ERROR_MISSING_PARAMETER,    -109, "Missing parameter")              \
    X(SCPI_ERROR_INVALID_SUFFIX,       -131, "Invalid suffix")                 \
    X(SCPI_ERROR_SUFFIX_NOT_ALLOWED,   -138, "Suffix not allowed")             \
    X(SCPI_ERROR_EXECUTION_ERROR,      -200, "Execution error")                \
    X(SCPI_ERROR_ILLEGAL_PARAMETER_VALUE,-224,"Illegal parameter value")       \


enum {
#define X(def, val, str) def = val,
LIST_OF_ERRORS
#undef X
};

#ifdef	__cplusplus
}
#endif

#endif	/* SCPI_ERROR_H */

