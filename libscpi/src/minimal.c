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
 * @file   scpi_minimal.c
 * @date   Thu Nov 15 10:58:45 UTC 2012
 * 
 * @brief  SCPI minimal implementation
 * 
 * 
 */


#include "scpi/parser.h"
#include "scpi/minimal.h"
#include "scpi/constants.h"
#include "scpi/error.h"
#include "scpi/ieee488.h"

/**
 * Command stub function
 * @param context
 * @return 
 */
scpi_result_t SCPI_Stub(scpi_t * context) {
    (void) context;
    return SCPI_RES_OK;
}

/**
 * Query command stub function
 * @param context
 * @return 
 */
scpi_result_t SCPI_StubQ(scpi_t * context) {
    SCPI_ResultInt(context, 0);
    return SCPI_RES_OK;
}

/**
 * SYSTem:VERSion?
 * @param context
 * @return 
 */
scpi_result_t SCPI_SystemVersionQ(scpi_t * context) {
    SCPI_ResultMnemonic(context, SCPI_STD_VERSION_REVISION);
    return SCPI_RES_OK;
}

/**
 * SYSTem:ERRor[:NEXT]?
 * @param context
 * @return 
 */
scpi_result_t SCPI_SystemErrorNextQ(scpi_t * context) {
    int16_t err = SCPI_ErrorPop(context);

    SCPI_ResultInt(context, err);
    SCPI_ResultText(context, SCPI_ErrorTranslate(err));

    return SCPI_RES_OK;
}

/**
 * SYSTem:ERRor:COUNt?
 * @param context
 * @return 
 */
scpi_result_t SCPI_SystemErrorCountQ(scpi_t * context) {
    SCPI_ResultInt(context, SCPI_ErrorCount(context));

    return SCPI_RES_OK;
}

/**
 * STATus:QUEStionable[:EVENt]?
 * @param context
 * @return 
 */
scpi_result_t SCPI_StatusQuestionableEventQ(scpi_t * context) {
    /* return value */
    SCPI_ResultInt(context, SCPI_RegGet(context, SCPI_REG_QUES));

    /* clear register */
    SCPI_RegSet(context, SCPI_REG_QUES, 0);

    return SCPI_RES_OK;
}

/**
 * STATus:QUEStionable:ENABle?
 * @param context
 * @return 
 */
scpi_result_t SCPI_StatusQuestionableEnableQ(scpi_t * context) {
    /* return value */
    SCPI_ResultInt(context, SCPI_RegGet(context, SCPI_REG_QUESE));

    return SCPI_RES_OK;
}

/**
 * STATus:QUEStionable:ENABle
 * @param context
 * @return 
 */
scpi_result_t SCPI_StatusQuestionableEnable(scpi_t * context) {
    int32_t new_QUESE;
    if (SCPI_ParamInt(context, &new_QUESE, TRUE)) {
        SCPI_RegSet(context, SCPI_REG_QUESE, (scpi_reg_val_t)new_QUESE);
    }
    return SCPI_RES_OK;
}

/**
 * STATus:PRESet
 * @param context
 * @return 
 */
scpi_result_t SCPI_StatusPreset(scpi_t * context) {
    /* clear STATUS:... */
    SCPI_RegSet(context, SCPI_REG_QUES, 0);
    return SCPI_RES_OK;
}
