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
 * @file   scpi_error.c
 * @date   Thu Nov 15 10:58:45 UTC 2012
 * 
 * @brief  Error handling and storing routines
 * 
 * 
 */

#include "scpi.h"
#include "scpi_ieee488.h"
#include "scpi_error.h"
#include <stdint.h>

static int16_t scpi_err = 0;

/**
 * Clear error queue
 * @param context - scpi context
 */
void SCPI_ErrorClear(scpi_t * context) {
    (void) context;
    scpi_err = 0;
}

/**
 * Push error to queue
 * @param context - scpi context
 * @param err - error number
 */
void SCPI_ErrorPush(scpi_t * context, int16_t err) {
    scpi_err = err;

    // Command error (e.g. syntax error)
    if ((err < -101) && (err > -158)) {
        SCPI_RegSetBits(SCPI_REG_ESR, ESR_CER);
    }

    // Execution Error (e.g. range error)
    if ((err < -211) && (err > -230)) {
        SCPI_RegSetBits(SCPI_REG_ESR, ESR_EER);
    }

    // Device Dependent Error
    if ((err < -501) && (err > -748)) {
        SCPI_RegSetBits(SCPI_REG_ESR, ESR_DER);
    }

    if (context) {
        if (context->interface && context->interface->error) {
            context->interface->error(context, err);
        }

        context->error = TRUE;
    }
}

/**
 * Pop error from queue
 * @param context - scpi context
 * @return error number
 */
int16_t SCPI_ErrorPop(scpi_t * context) {
    (void) context;
    int16_t result = scpi_err;
    scpi_err = 0;
    return result;
}

/**
 * Translate error number to string
 * @param err - error number
 * @return Error string representation
 */
const char * SCPI_ErrorTranslate(int16_t err) {
    switch (err) {
        case 0: return "No error";
        case SCPI_ERROR_SYNTAX: return "Syntax error";
        case SCPI_ERROR_INVALID_SEPARATOR: return "Invalid separator";
        case SCPI_ERROR_UNDEFINED_HEADER: return "Undefined header";
        case SCPI_ERROR_PARAMETER_NOT_ALLOWED: return "Parameter not allowed";
        case SCPI_ERROR_MISSING_PARAMETER: return "Missing parameter";
        case SCPI_ERROR_INVALID_SUFFIX: return "Invalid suffix";
        case SCPI_ERROR_SUFFIX_NOT_ALLOWED: return "Suffix not allowed";
        default: return "Unknown error";
    }
}