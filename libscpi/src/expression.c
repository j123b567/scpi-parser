/*-
 * Copyright (c) 2012-2015 Jan Breuer,
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
 * @file   expression.c
 *
 * @brief  Expressions handling
 *
 *
 */

#include "scpi/expression.h"
#include "scpi/error.h"
#include "scpi/parser.h"

#include "lexer_private.h"

static scpi_expr_result_t numericRange(lex_state_t * state, scpi_bool_t * isRange, scpi_token_t * valueFrom, scpi_token_t * valueTo)
{
    if (scpiLex_DecimalNumericProgramData(state, valueFrom)) {
        if (scpiLex_Colon(state, valueTo)) {
            *isRange = TRUE;
            if (scpiLex_DecimalNumericProgramData(state, valueTo)) {
                return SCPI_EXPR_OK;
            } else {
                return SCPI_EXPR_ERROR;
            }
        } else {
            *isRange = FALSE;
            return SCPI_EXPR_OK;
        }
    }

    return SCPI_EXPR_NO_MORE;
}

scpi_expr_result_t SCPI_ExprNumericListEntry(scpi_t * context, scpi_parameter_t * param, int index, scpi_bool_t * isRange, scpi_parameter_t * valueFrom, scpi_parameter_t * valueTo)
{
    lex_state_t lex;
    int i;
    scpi_expr_result_t res;

    if (!isRange || !valueFrom || !valueTo || !param) {
        SCPI_ErrorPush(context, SCPI_ERROR_SYSTEM_ERROR);
        return SCPI_EXPR_ERROR;
    }

    if (param->type != SCPI_TOKEN_PROGRAM_EXPRESSION) {
        SCPI_ErrorPush(context, SCPI_ERROR_DATA_TYPE_ERROR);
        return SCPI_EXPR_ERROR;
    }

    lex.buffer = param->ptr + 1;
    lex.pos = lex.buffer;
    lex.len = param->len - 2;

    for (i = 0; i <= index; i++) {
        res = numericRange(&lex, isRange, valueFrom, valueTo);
        if (res != SCPI_EXPR_OK) {
            break;
        }
        if (i != index) {
            if (!scpiLex_Comma(&lex, valueFrom)) {
                res = SCPI_EXPR_ERROR;
                break;
            }
        }
    }

    if (res == SCPI_EXPR_ERROR) {
        SCPI_ErrorPush(context, SCPI_ERROR_EXPRESSION_PARSING_ERROR);
    }
    return res;
}

scpi_expr_result_t SCPI_ExprNumericListEntryInt(scpi_t * context, scpi_parameter_t * param, int index, scpi_bool_t * isRange, int32_t * valueFrom, int32_t * valueTo)
{
    scpi_expr_result_t res;
    scpi_bool_t range = FALSE;
    scpi_parameter_t paramFrom;
    scpi_parameter_t paramTo;

    res = SCPI_ExprNumericListEntry(context, param, index, &range, &paramFrom, &paramTo);
    if (res == SCPI_EXPR_OK) {
        SCPI_ParamToInt(context, &paramFrom, valueFrom);
        if (range) {
            SCPI_ParamToInt(context, &paramTo, valueFrom);
        }
    }

    return res;
}

scpi_expr_result_t SCPI_ExprNumericListEntryDouble(scpi_t * context, scpi_parameter_t * param, int index, scpi_bool_t * isRange, double * valueFrom, double * valueTo)
{
    scpi_expr_result_t res;
    scpi_bool_t range = FALSE;
    scpi_parameter_t paramFrom;
    scpi_parameter_t paramTo;

    res = SCPI_ExprNumericListEntry(context, param, index, &range, &paramFrom, &paramTo);
    if (res == SCPI_EXPR_OK) {
        SCPI_ParamToDouble(context, &paramFrom, valueFrom);
        if (range) {
            SCPI_ParamToDouble(context, &paramTo, valueFrom);
        }
    }

    return res;
}
