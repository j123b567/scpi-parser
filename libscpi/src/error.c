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

#include <stdint.h>

#include "scpi/parser.h"
#include "scpi/ieee488.h"
#include "scpi/error.h"
#include "scpi/fifo.h"

/* basic FIFO */
static fifo_t local_error_queue;



void SCPI_ErrorInit(scpi_t * context) {
    /*
     * // FreeRTOS
     * context->error_queue = (scpi_error_queue_t)xQueueCreate(100, sizeof(int16_t));
     */

    /* basic FIFO */
    context->error_queue = (scpi_error_queue_t)&local_error_queue;
    fifo_init((fifo_t *)context->error_queue);
}

/**
 * Clear error queue
 * @param context - scpi context
 */
void SCPI_ErrorClear(scpi_t * context) {
    /*
     * // FreeRTOS
     * xQueueReset((xQueueHandle)context->error_queue);
     */

    /* basic FIFO */
    fifo_clear((fifo_t *)context->error_queue);
}

/**
 * Pop error from queue
 * @param context - scpi context
 * @return error number
 */
int16_t SCPI_ErrorPop(scpi_t * context) {
    int16_t result = 0;

    /*
     * // FreeRTOS
     * if (pdFALSE == xQueueReceive((xQueueHandle)context->error_queue, &result, 0)) {
     *   result = 0;
     * }
     */

    /* basic FIFO */
    fifo_remove((fifo_t *)context->error_queue, &result);

    return result;
}

/**
 * Return number of errors/events in the queue
 * @param context
 * @return 
 */
int32_t SCPI_ErrorCount(scpi_t * context) {
    int16_t result = 0;

    /*
     * // FreeRTOS
     * result = uxQueueMessagesWaiting((xQueueHandle)context->error_queue);
     */

    /* basic FIFO */
    fifo_count((fifo_t *)context->error_queue, &result);

    return result;
}

static void SCPI_ErrorAddInternal(scpi_t * context, int16_t err) {
    /*
     * // FreeRTOS
     * xQueueSend((xQueueHandle)context->error_queue, &err, 0);
     */

    /* basic FIFO */
    fifo_add((fifo_t *)context->error_queue, err);
}

struct error_reg {
    int16_t from;
    int16_t to;
    scpi_reg_val_t bit;
};

#define ERROR_DEFS_N	8

static const struct error_reg errs[ERROR_DEFS_N] = {
    {-100, -199, ESR_CER}, /* Command error (e.g. syntax error) ch 21.8.9    */
    {-200, -299, ESR_EER}, /* Execution Error (e.g. range error) ch 21.8.10  */
    {-300, -399, ESR_DER}, /* Device specific error -300, -399 ch 21.8.11    */
    {-400, -499, ESR_QER}, /* Query error -400, -499 ch 21.8.12              */
    {-500, -599, ESR_PON}, /* Power on event -500, -599 ch 21.8.13           */
    {-600, -699, ESR_URQ}, /* User Request Event -600, -699 ch 21.8.14       */
    {-700, -799, ESR_REQ}, /* Request Control Event -700, -799 ch 21.8.15    */
    {-800, -899, ESR_OPC}, /* Operation Complete Event -800, -899 ch 21.8.16 */
};

/**
 * Push error to queue
 * @param context - scpi context
 * @param err - error number
 */
void SCPI_ErrorPush(scpi_t * context, int16_t err) {

    int i;

    SCPI_ErrorAddInternal(context, err);

    for(i = 0; i < ERROR_DEFS_N; i++) {
        if ((err <= errs[i].from) && (err >= errs[i].to)) {
            SCPI_RegSetBits(context, SCPI_REG_ESR, errs[i].bit);
        }
    }

    if (context) {
        if (context->interface && context->interface->error) {
            context->interface->error(context, err);
        }

        context->cmd_error = TRUE;
    }
}

/**
 * Translate error number to string
 * @param err - error number
 * @return Error string representation
 */
const char * SCPI_ErrorTranslate(int16_t err) {
    switch (err) {
        case 0: return "No error";
        #define X(def, val, str) case def: return str;
        LIST_OF_ERRORS
        #undef X        
        default: return "Unknown error";
    }
}


