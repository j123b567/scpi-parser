/*-
 * BSD 2-Clause License
 *
 * Copyright (c) 2012-2018, Jan Breuer
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _SCPI_SERVER_H_
#define _SCPI_SERVER_H_
#ifdef __cplusplus
extern "C" {
#endif

#define SCPI_KEEP_IDLE    2000 // (ms) keepalive quiet time after last TCP packet
#define SCPI_KEEP_INTVL   1000 // (ms) keepalive repeat interval
#define SCPI_KEEP_CNT        4 // Retry count before terminating connection (SCPI_KEEP_INTVL * SCPI_KEEP_INTVL (ms)).

#define SCPI_DEVICE_PORT  5025 // scpi-raw standard port
#define SCPI_CONTROL_PORT 5026 // libscpi control port (not part of the standard)

#include <stdint.h>
#include "lwip/api.h"
#include "scpi/types.h"

void scpi_server_init(void);

void SCPI_AddError(int16_t err);
void SCPI_RequestControl(void);

// optional event handlers
void SCPI_Event_DeviceConnected(scpi_t * context, struct netconn * conn);
void SCPI_Event_DeviceDisconnected(scpi_t * context, struct netconn * conn);
void SCPI_Event_ControlConnected(scpi_t * context, struct netconn * conn);
void SCPI_Event_ControlDisconnected(scpi_t * context, struct netconn * conn);
void SCPI_Event_ErrorIndicatorOn(scpi_t * context, int_fast16_t err);
void SCPI_Event_ErrorIndicatorOff(scpi_t * context, int_fast16_t err);

#ifdef __cplusplus
}
#endif
#endif /* _SCPI_SERVER_H_ */
