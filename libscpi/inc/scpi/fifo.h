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
 * @file   scpi_fifo.h
 * @date   Thu Nov 15 10:58:45 UTC 2012
 * 
 * @brief  basic FIFO implementation
 * 
 * 
 */

#ifndef SCPI_FIFO_H
#define	SCPI_FIFO_H

#include "scpi/types.h"

#ifdef	__cplusplus
extern "C" {
#endif


#define FIFO_SIZE 16

    struct _fifo_t {
        int16_t wr;
        int16_t rd;
        int16_t size;
        int16_t data[FIFO_SIZE];
    };
    typedef struct _fifo_t fifo_t;

    void fifo_init(fifo_t * fifo);
    void fifo_clear(fifo_t * fifo);
    scpi_bool_t fifo_add(fifo_t * fifo, int16_t value);
    scpi_bool_t fifo_remove(fifo_t * fifo, int16_t * value);
    scpi_bool_t fifo_count(fifo_t * fifo, int16_t * value);

#ifdef	__cplusplus
}
#endif

#endif	/* SCPI_FIFO_H */
