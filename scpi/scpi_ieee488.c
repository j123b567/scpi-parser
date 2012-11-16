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

#include "scpi.h"
#include "scpi_ieee488.h"
#include "scpi_error.h"
#include "scpi_constants.h"
#include <string.h>
#include <stdint.h>

/* register array */
static scpi_reg_val_t regs[SCPI_REG_COUNT];

/**
 * Update register value
 * @param name - register name
 */
static void SCPI_RegUpdate(scpi_reg_name_t name) {
    SCPI_RegSet(name, SCPI_RegGet(name));
}

/**
 * Get register value
 * @param name - register name
 * @return register value
 */
scpi_reg_val_t SCPI_RegGet(scpi_reg_name_t name) {
    if (name < SCPI_REG_COUNT) {
        return regs[name];
    } else {
        return 0;
    }
}

/**
 * Set register value
 * @param name - register name
 * @param val - new value
 */
void SCPI_RegSet(scpi_reg_name_t name, scpi_reg_val_t val) {
    if (name >= SCPI_REG_COUNT) {
        return;
    }

    // set register value
    regs[name] = val;

    switch (name) {
        case SCPI_REG_STB:
            if (val & (SCPI_RegGet(SCPI_REG_SRE) &~STB_SRQ)) {
                val |= STB_SRQ;
            } else {
                val &= ~STB_SRQ;
            }
            break;
        case SCPI_REG_SRE:
            SCPI_RegUpdate(SCPI_REG_STB);
            break;
        case SCPI_REG_ESR:
            if (val & SCPI_RegGet(SCPI_REG_ESE)) {
                SCPI_RegSetBits(SCPI_REG_STB, STB_ESR);
            } else {
                SCPI_RegClearBits(SCPI_REG_STB, STB_ESR);
            }
            break;
        case SCPI_REG_ESE:
            SCPI_RegUpdate(SCPI_REG_ESR);
            break;
        case SCPI_REG_QUES:
            if (val & SCPI_RegGet(SCPI_REG_QUESE)) {
                SCPI_RegSetBits(SCPI_REG_STB, STB_QES);
            } else {
                SCPI_RegClearBits(SCPI_REG_STB, STB_QES);
            }
            break;
        case SCPI_REG_QUESE:
            SCPI_RegUpdate(SCPI_REG_QUES);
            break;
        case SCPI_REG_OPER:
            if (val & SCPI_RegGet(SCPI_REG_OPERE)) {
                SCPI_RegSetBits(SCPI_REG_STB, STB_OPS);
            } else {
                SCPI_RegClearBits(SCPI_REG_STB, STB_OPS);
            }
            break;
        case SCPI_REG_OPERE:
            SCPI_RegUpdate(SCPI_REG_OPER);
            break;
            
            
        case SCPI_REG_COUNT:
            // nothing to do
            break;
    }

    // set updated register value
    regs[name] = val;

}

/**
 * Set register bits
 * @param name - register name
 * @param bits bit mask
 */
void SCPI_RegSetBits(scpi_reg_name_t name, scpi_reg_val_t bits) {
    SCPI_RegSet(name, SCPI_RegGet(name) | bits);
}

/**
 * Clear register bits
 * @param name - register name
 * @param bits bit mask
 */
void SCPI_RegClearBits(scpi_reg_name_t name, scpi_reg_val_t bits) {
    SCPI_RegSet(name, SCPI_RegGet(name) & ~bits);
}

/* ============ */

void SCPI_EventClear(void) {
    // TODO
    SCPI_RegSet(SCPI_REG_ESR, 0);
}

/**
 * *CLS
 * @param context
 * @return 
 */
int SCPI_CoreCls(scpi_context_t * context) {
    (void) context;
    SCPI_EventClear();
    SCPI_ErrorClear(context);
    SCPI_RegSet(SCPI_REG_OPER, 0);
    SCPI_RegSet(SCPI_REG_QUES, 0);
    return 0;
}

/**
 * *ESE
 * @param context
 * @return 
 */
int SCPI_CoreEse(scpi_context_t * context) {
    int32_t new_ESE;
    if (SCPI_ParamInt(context, &new_ESE, TRUE)) {
        SCPI_RegSet(SCPI_REG_ESE, new_ESE);
    }
    return 0;
}

/**
 * *ESE?
 * @param context
 * @return 
 */
int SCPI_CoreEseQ(scpi_context_t * context) {
    (void) context;
    SCPI_ResultInt(context, SCPI_RegGet(SCPI_REG_ESE));
    return 0;
}

/**
 * *ESR?
 * @param context
 * @return 
 */
int SCPI_CoreEsrQ(scpi_context_t * context) {
    (void) context;
    SCPI_ResultInt(context, SCPI_RegGet(SCPI_REG_ESR));
    SCPI_RegSet(SCPI_REG_ESR, 0);
    return 0;
}

/**
 * *IDN?
 * @param context
 * @return 
 */
int SCPI_CoreIdnQ(scpi_context_t * context) {
    (void) context;
    SCPI_ResultString(context, SCPI_MANUFACTURE);
    SCPI_ResultString(context, SCPI_DEV_NAME);
    SCPI_ResultString(context, SCPI_DEV_VERSION);
    return 0;
}

/**
 * *OPC
 * @param context
 * @return 
 */
int SCPI_CoreOpc(scpi_context_t * context) {
    (void) context;
    SCPI_RegSetBits(SCPI_REG_ESR, ESR_OPC);
    return 0;
}

/**
 * *OPC?
 * @param context
 * @return 
 */
int SCPI_CoreOpcQ(scpi_context_t * context) {
    (void) context;
    // Operation is always completed
    SCPI_ResultInt(context, 1);
    return 0;
}

/**
 * *RST
 * @param context
 * @return 
 */
int SCPI_CoreRst(scpi_context_t * context) {
    if (context && context->interface && context->interface->reset) {
        return context->interface->reset(context);
    }
    return 0;
}

/**
 * *SRE
 * @param context
 * @return 
 */
int SCPI_CoreSre(scpi_context_t * context) {
    int32_t new_SRE;
    if (SCPI_ParamInt(context, &new_SRE, TRUE)) {
        SCPI_RegSet(SCPI_REG_SRE, new_SRE);
    }
    return 0;
}

/**
 * *SRE?
 * @param context
 * @return 
 */
int SCPI_CoreSreQ(scpi_context_t * context) {
    (void) context;
    SCPI_ResultInt(context, SCPI_RegGet(SCPI_REG_SRE));
    return 0;
}

/**
 * *STB?
 * @param context
 * @return 
 */
int SCPI_CoreStbQ(scpi_context_t * context) {
    (void) context;
    SCPI_ResultInt(context, SCPI_RegGet(SCPI_REG_STB));
    return 0;
}

/**
 * *TST?
 * @param context
 * @return 
 */
int SCPI_CoreTstQ(scpi_context_t * context) {
    (void) context;
    int result = 0;
    if (context && context->interface && context->interface->test) {
        result = context->interface->test(context);
    }    
    SCPI_ResultInt(context, result);
    return 0;
}

/**
 * *WAI
 * @param context
 * @return 
 */
int SCPI_CoreWai(scpi_context_t * context) {
    (void) context;
    // NOP
    return 0;
}

