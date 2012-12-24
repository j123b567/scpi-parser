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
 * @file   scpi_ieee488.c
 * @date   Thu Nov 15 10:58:45 UTC 2012
 * 
 * @brief  Implementation of IEEE488.2 commands and state model
 * 
 * 
 */

#include "scpi/parser.h"
#include "scpi/ieee488.h"
#include "scpi/error.h"
#include "scpi/constants.h"

/**
 * Update register value
 * @param name - register name
 */
static void SCPI_RegUpdate(scpi_t * context, scpi_reg_name_t name) {
    SCPI_RegSet(context, name, SCPI_RegGet(context, name));
}

/**
 * Get register value
 * @param name - register name
 * @return register value
 */
scpi_reg_val_t SCPI_RegGet(scpi_t * context, scpi_reg_name_t name) {
    if ((name < SCPI_REG_COUNT) && (context->registers != NULL)) {
        return context->registers[name];
    } else {
        return 0;
    }
}

/**
 * Set register value
 * @param name - register name
 * @param val - new value
 */
void SCPI_RegSet(scpi_t * context, scpi_reg_name_t name, scpi_reg_val_t val) {
    bool_t srq = FALSE;
    scpi_reg_val_t mask;

    if ((name >= SCPI_REG_COUNT) || (context->registers == NULL)) {
        return;
    }
    

    // set register value
    context->registers[name] = val;

    switch (name) {
        case SCPI_REG_STB:
            mask = SCPI_RegGet(context, SCPI_REG_SRE);
            mask &= ~STB_SRQ;
            if (val & mask) {
                val |= STB_SRQ;
                srq = TRUE;
            } else {
                val &= ~STB_SRQ;
            }
            break;
        case SCPI_REG_SRE:
            SCPI_RegUpdate(context, SCPI_REG_STB);
            break;
        case SCPI_REG_ESR:
            if (val & SCPI_RegGet(context, SCPI_REG_ESE)) {
                SCPI_RegSetBits(context, SCPI_REG_STB, STB_ESR);
            } else {
                SCPI_RegClearBits(context, SCPI_REG_STB, STB_ESR);
            }
            break;
        case SCPI_REG_ESE:
            SCPI_RegUpdate(context, SCPI_REG_ESR);
            break;
        case SCPI_REG_QUES:
            if (val & SCPI_RegGet(context, SCPI_REG_QUESE)) {
                SCPI_RegSetBits(context, SCPI_REG_STB, STB_QES);
            } else {
                SCPI_RegClearBits(context, SCPI_REG_STB, STB_QES);
            }
            break;
        case SCPI_REG_QUESE:
            SCPI_RegUpdate(context, SCPI_REG_QUES);
            break;
        case SCPI_REG_OPER:
            if (val & SCPI_RegGet(context, SCPI_REG_OPERE)) {
                SCPI_RegSetBits(context, SCPI_REG_STB, STB_OPS);
            } else {
                SCPI_RegClearBits(context, SCPI_REG_STB, STB_OPS);
            }
            break;
        case SCPI_REG_OPERE:
            SCPI_RegUpdate(context, SCPI_REG_OPER);
            break;
            
            
        case SCPI_REG_COUNT:
            // nothing to do
            break;
    }

    // set updated register value
    context->registers[name] = val;

    if (srq && context->interface && context->interface->srq) {
        context->interface->srq(context);
    }
}

/**
 * Set register bits
 * @param name - register name
 * @param bits bit mask
 */
void SCPI_RegSetBits(scpi_t * context, scpi_reg_name_t name, scpi_reg_val_t bits) {
    SCPI_RegSet(context, name, SCPI_RegGet(context, name) | bits);
}

/**
 * Clear register bits
 * @param name - register name
 * @param bits bit mask
 */
void SCPI_RegClearBits(scpi_t * context, scpi_reg_name_t name, scpi_reg_val_t bits) {
    SCPI_RegSet(context, name, SCPI_RegGet(context, name) & ~bits);
}

/* ============ */

void SCPI_EventClear(scpi_t * context) {
    // TODO
    SCPI_RegSet(context, SCPI_REG_ESR, 0);
}

/**
 * *CLS - This command clears all status data structures in a device. 
 *        For a device which minimally complies with SCPI. (SCPI std 4.1.3.2)
 * @param context
 * @return 
 */
scpi_result_t SCPI_CoreCls(scpi_t * context) {
    SCPI_EventClear(context);
    SCPI_ErrorClear(context);
    SCPI_RegSet(context, SCPI_REG_OPER, 0);
    SCPI_RegSet(context, SCPI_REG_QUES, 0);
    return SCPI_RES_OK;
}

/**
 * *ESE
 * @param context
 * @return 
 */
scpi_result_t SCPI_CoreEse(scpi_t * context) {
    int32_t new_ESE;
    if (SCPI_ParamInt(context, &new_ESE, TRUE)) {
        SCPI_RegSet(context, SCPI_REG_ESE, new_ESE);
    }
    return SCPI_RES_OK;
}

/**
 * *ESE?
 * @param context
 * @return 
 */
scpi_result_t SCPI_CoreEseQ(scpi_t * context) {
    SCPI_ResultInt(context, SCPI_RegGet(context, SCPI_REG_ESE));
    return SCPI_RES_OK;
}

/**
 * *ESR?
 * @param context
 * @return 
 */
scpi_result_t SCPI_CoreEsrQ(scpi_t * context) {
    SCPI_ResultInt(context, SCPI_RegGet(context, SCPI_REG_ESR));
    SCPI_RegSet(context, SCPI_REG_ESR, 0);
    return SCPI_RES_OK;
}

/**
 * *IDN?
 * @param context
 * @return 
 */
scpi_result_t SCPI_CoreIdnQ(scpi_t * context) {
    SCPI_ResultString(context, SCPI_MANUFACTURE);
    SCPI_ResultString(context, SCPI_DEV_NAME);
    SCPI_ResultString(context, SCPI_DEV_VERSION);
    return SCPI_RES_OK;
}

/**
 * *OPC
 * @param context
 * @return 
 */
scpi_result_t SCPI_CoreOpc(scpi_t * context) {
    SCPI_RegSetBits(context, SCPI_REG_ESR, ESR_OPC);
    return SCPI_RES_OK;
}

/**
 * *OPC?
 * @param context
 * @return 
 */
scpi_result_t SCPI_CoreOpcQ(scpi_t * context) {
    // Operation is always completed
    SCPI_ResultInt(context, 1);
    return SCPI_RES_OK;
}

/**
 * *RST
 * @param context
 * @return 
 */
scpi_result_t SCPI_CoreRst(scpi_t * context) {
    if (context && context->interface && context->interface->reset) {
        return context->interface->reset(context);
    }
    return SCPI_RES_OK;
}

/**
 * *SRE
 * @param context
 * @return 
 */
scpi_result_t SCPI_CoreSre(scpi_t * context) {
    int32_t new_SRE;
    if (SCPI_ParamInt(context, &new_SRE, TRUE)) {
        SCPI_RegSet(context, SCPI_REG_SRE, new_SRE);
    }
    return SCPI_RES_OK;
}

/**
 * *SRE?
 * @param context
 * @return 
 */
scpi_result_t SCPI_CoreSreQ(scpi_t * context) {
    SCPI_ResultInt(context, SCPI_RegGet(context, SCPI_REG_SRE));
    return SCPI_RES_OK;
}

/**
 * *STB?
 * @param context
 * @return 
 */
scpi_result_t SCPI_CoreStbQ(scpi_t * context) {
    SCPI_ResultInt(context, SCPI_RegGet(context, SCPI_REG_STB));
    return SCPI_RES_OK;
}

/**
 * *TST?
 * @param context
 * @return 
 */
scpi_result_t SCPI_CoreTstQ(scpi_t * context) {
    int result = 0;
    if (context && context->interface && context->interface->test) {
        result = context->interface->test(context);
    }    
    SCPI_ResultInt(context, result);
    return SCPI_RES_OK;
}

/**
 * *WAI
 * @param context
 * @return 
 */
scpi_result_t SCPI_CoreWai(scpi_t * context) {
    (void) context;
    // NOP
    return SCPI_RES_OK;
}

