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

#include <stdio.h>

/**
 * Update register value
 * @param context
 * @param name - register name
 */
static void regUpdate(scpi_t * context, scpi_reg_name_t name) {
    SCPI_RegSet(context, name, SCPI_RegGet(context, name));
}

/**
 * Update latching event register value based on bit transitions from 0 -> 1
 * in the condition register
 * @param context
 * @param condReg - condition register name
 * @param eventReg - event register name
 */
static void regUpdateEvent(scpi_t * context, scpi_reg_val_t oldCondVal, scpi_reg_val_t newCondVal, scpi_reg_name_t eventReg) {
    SCPI_RegSet(context, eventReg, ((oldCondVal ^ newCondVal) & newCondVal) | SCPI_RegGet(context, eventReg));
}

/**
 * Update STB register according to value and its mask register
 * @param context
 * @param val value of register
 * @param mask name of mask register (enable register)
 * @param stbBits bits to clear or set in STB
 */
static void regUpdateSTB(scpi_t * context, scpi_reg_val_t val, scpi_reg_name_t mask, scpi_reg_val_t stbBits) {
    if (val & SCPI_RegGet(context, mask)) {
        SCPI_RegSetBits(context, SCPI_REG_STB, stbBits);
    } else {
        SCPI_RegClearBits(context, SCPI_REG_STB, stbBits);
    }
}

/**
 * Get register value
 * @param name - register name
 * @return register value
 */
scpi_reg_val_t SCPI_RegGet(scpi_t * context, scpi_reg_name_t name) {
    if ((name < SCPI_REG_COUNT) && context) {
        return context->registers[name];
    } else {
        return 0;
    }
}

/**
 * Wrapper function to control interface from context
 * @param context
 * @param ctrl number of controll message
 * @param value value of related register
 */
static size_t writeControl(scpi_t * context, scpi_ctrl_name_t ctrl, scpi_reg_val_t val) {
    if (context && context->interface && context->interface->control) {
        return context->interface->control(context, ctrl, val);
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
    scpi_bool_t srq = FALSE;
    scpi_reg_val_t mask;
    scpi_reg_val_t old_val;

    if ((name >= SCPI_REG_COUNT) || (context == NULL)) {
        return;
    }

    /* store old register value */
    old_val = context->registers[name];

    /* set register value */
    context->registers[name] = val;

    /** @TODO: remove recutsion */
    switch (name) {
        case SCPI_REG_STB:
            mask = SCPI_RegGet(context, SCPI_REG_SRE);
            mask &= ~STB_SRQ;
            if (val & mask) {
                val |= STB_SRQ;
                /* avoid sending SRQ if nothing has changed */
                if (old_val != val) {
                    srq = TRUE;
                }
            } else {
                val &= ~STB_SRQ;
            }
            break;
        case SCPI_REG_SRE:
            regUpdate(context, SCPI_REG_STB);
            break;
        case SCPI_REG_ESR:
            regUpdateSTB(context, val, SCPI_REG_ESE, STB_ESR);
            break;
        case SCPI_REG_ESE:
            regUpdate(context, SCPI_REG_ESR);
            break;
        case SCPI_REG_QUES:
            regUpdateSTB(context, val, SCPI_REG_QUESE, STB_QES);
            break;
        case SCPI_REG_QUESE:
            regUpdate(context, SCPI_REG_QUES);
            break;
        case SCPI_REG_QUESC:
            regUpdateEvent(context, old_val, val, SCPI_REG_QUES);
            break;
        case SCPI_REG_OPER:
            regUpdateSTB(context, val, SCPI_REG_OPERE, STB_OPS);
            break;
        case SCPI_REG_OPERE:
            regUpdate(context, SCPI_REG_OPER);
            break;
        case SCPI_REG_OPERC:
            regUpdateEvent(context, old_val, val, SCPI_REG_OPER);
            break;


        case SCPI_REG_COUNT:
            /* nothing to do */
            break;
    }

    /* set updated register value */
    context->registers[name] = val;

    if (srq) {
        writeControl(context, SCPI_CTRL_SRQ, SCPI_RegGet(context, SCPI_REG_STB));
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

/**
 * Clear event register
 * @param context
 */
void SCPI_EventClear(scpi_t * context) {
    /* TODO */
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
    if (SCPI_ParamInt32(context, &new_ESE, TRUE)) {
        SCPI_RegSet(context, SCPI_REG_ESE, (scpi_reg_val_t) new_ESE);
        return SCPI_RES_OK;
    }
    return SCPI_RES_ERR;
}

/**
 * *ESE?
 * @param context
 * @return 
 */
scpi_result_t SCPI_CoreEseQ(scpi_t * context) {
    SCPI_ResultInt32(context, SCPI_RegGet(context, SCPI_REG_ESE));
    return SCPI_RES_OK;
}

/**
 * *ESR?
 * @param context
 * @return 
 */
scpi_result_t SCPI_CoreEsrQ(scpi_t * context) {
    SCPI_ResultInt32(context, SCPI_RegGet(context, SCPI_REG_ESR));
    SCPI_RegSet(context, SCPI_REG_ESR, 0);
    return SCPI_RES_OK;
}

/**
 * *IDN?
 * 
 * field1: MANUFACTURE
 * field2: MODEL
 * field4: SUBSYSTEMS REVISIONS
 * 
 * example: MANUFACTURE,MODEL,0,01-02-01
 * @param context
 * @return 
 */
scpi_result_t SCPI_CoreIdnQ(scpi_t * context) {
    int i;
    for (i = 0; i < 4; i++) {
        if (context->idn[i]) {
            SCPI_ResultMnemonic(context, context->idn[i]);
        } else {
            SCPI_ResultMnemonic(context, "0");
        }
    }
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
    /* Operation is always completed */
    SCPI_ResultInt32(context, 1);
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
    if (SCPI_ParamInt32(context, &new_SRE, TRUE)) {
        SCPI_RegSet(context, SCPI_REG_SRE, (scpi_reg_val_t) new_SRE);
        return SCPI_RES_OK;
    }
    return SCPI_RES_ERR;
}

/**
 * *SRE?
 * @param context
 * @return 
 */
scpi_result_t SCPI_CoreSreQ(scpi_t * context) {
    SCPI_ResultInt32(context, SCPI_RegGet(context, SCPI_REG_SRE));
    return SCPI_RES_OK;
}

/**
 * *STB?
 * @param context
 * @return 
 */
scpi_result_t SCPI_CoreStbQ(scpi_t * context) {
    SCPI_ResultInt32(context, SCPI_RegGet(context, SCPI_REG_STB));
    return SCPI_RES_OK;
}

/**
 * *TST?
 * @param context
 * @return 
 */
scpi_result_t SCPI_CoreTstQ(scpi_t * context) {
    (void) context;
    SCPI_ResultInt32(context, 0);
    return SCPI_RES_OK;
}

/**
 * *WAI
 * @param context
 * @return 
 */
scpi_result_t SCPI_CoreWai(scpi_t * context) {
    (void) context;
    /* NOP */
    return SCPI_RES_OK;
}

