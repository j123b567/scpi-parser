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


#include "scpi.h"
#include "scpi_minimal.h"
#include "scpi_constants.h"
#include "scpi_error.h"
#include "scpi_ieee488.h"

/**
 * Command stub function
 * @param context
 * @return 
 */
int SCPI_Stub(scpi_context_t * context) {
    (void) context;
    return 0;
}

/**
 * Query command stub function
 * @param context
 * @return 
 */
int SCPI_StubQ(scpi_context_t * context) {
    (void) context;
    SCPI_ResultString(context, "");
    return 0;
}

/**
 * SYSTem:VERSion?
 * @param context
 * @return 
 */
int SCPI_SystemVersionQ(scpi_context_t * context) {
    (void) context;
    SCPI_ResultString(context, SCPI_DEV_VERSION);
    return 0;
}

/**
 * SYSTem:ERRor[:NEXT]?
 * @param context
 * @return 
 */
int SCPI_SystemErrorNextQ(scpi_context_t * context) {
    (void) context;

    int16_t err = SCPI_ErrorPop(context);

    SCPI_ResultInt(context, err);
    SCPI_ResultText(context, SCPI_ErrorTranslate(err));

    return 0;
}

/**
 * STATus:QUEStionable[:EVENt]?
 * @param context
 * @return 
 */
int SCPI_StatusQuestionableEventQ(scpi_context_t * context) {
    (void) context;

    // return value
    SCPI_ResultInt(context, SCPI_RegGet(SCPI_REG_QUES));

    // clear register
    SCPI_RegSet(SCPI_REG_QUES, 0);

    return 0;
}

/**
 * STATus:QUEStionable:ENABle?
 * @param context
 * @return 
 */
int SCPI_StatusQuestionableEnableQ(scpi_context_t * context) {
    (void) context;

    // return value
    SCPI_ResultInt(context, SCPI_RegGet(SCPI_REG_QUESE));

    return 0;
}

/**
 * STATus:QUEStionable:ENABle
 * @param context
 * @return 
 */
int SCPI_StatusQuestionableEnable(scpi_context_t * context) {
    int32_t new_QUESE;
    if (SCPI_ParamInt(context, &new_QUESE, TRUE)) {
        SCPI_RegSet(SCPI_REG_QUESE, new_QUESE);
    }
    return 0;
}

/**
 * STATus:PRESet
 * @param context
 * @return 
 */
int SCPI_StatusPreset(scpi_context_t * context) {
    (void) context;
    // clear STATUS:...
    SCPI_RegSet(SCPI_REG_QUES, 0);
    return 0;
}
