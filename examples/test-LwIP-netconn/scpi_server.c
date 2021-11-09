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

/**
 * @file   scpi_server.c
 * @date   Thu Nov 15 10:58:45 UTC 2012
 *
 * @brief  TCP/IP SCPI Server
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "scpi/scpi.h"
#include "scpi-def.h"

#include "lwip/tcpip.h"
#include "FreeRTOS.h"
#include "task.h"
#include "lwip/tcp.h"
#include "lwip/inet.h"

#define SCPI_THREAD_PRIO (tskIDLE_PRIORITY + 2)

#define SCPI_MSG_TIMEOUT                0
#define SCPI_MSG_TEST                   1
#define SCPI_MSG_IO_LISTEN              2
#define SCPI_MSG_CONTROL_IO_LISTEN      3
#define SCPI_MSG_IO                     4
#define SCPI_MSG_CONTROL_IO             5
#define SCPI_MSG_SET_ESE_REQ            6
#define SCPI_MSG_SET_ERROR              7

typedef struct {
    struct netconn *io_listen;
    struct netconn *control_io_listen;
    struct netconn *io;
    struct netconn *control_io;
    xQueueHandle evtQueue;
    /* FILE * fio; */
    /* fd_set fds; */
} user_data_t;

struct _queue_event_t {
    uint8_t cmd;
    uint8_t param1;
    int16_t param2;
} __attribute__((__packed__));
typedef struct _queue_event_t queue_event_t;


user_data_t user_data = {
    .io_listen = NULL,
    .io = NULL,
    .control_io_listen = NULL,
    .control_io = NULL,
    .evtQueue = 0,
};

size_t SCPI_Write(scpi_t * context, const char * data, size_t len) {
    if (context->user_context != NULL) {
        user_data_t * u = (user_data_t *) (context->user_context);
        if (u->io) {
            return (netconn_write(u->io, data, len, NETCONN_COPY) == ERR_OK) ? len : 0;
        }
    }
    return 0;
}

scpi_result_t SCPI_Flush(scpi_t * context) {
    if (context->user_context != NULL) {
        user_data_t * u = (user_data_t *) (context->user_context);
        if (u->io) {
            /* flush not implemented */
            return SCPI_RES_OK;
        }
    }
    return SCPI_RES_OK;
}

int SCPI_Error(scpi_t * context, int_fast16_t err) {
    (void) context;    
    if (err != 0) {
        /* New error */
        SCPI_Event_ErrorIndicatorOn(context, err);
    } else {
        /* No more errors in the queue */
        SCPI_Event_ErrorIndicatorOff(context, err);
    }
    return 0;
}

scpi_result_t SCPI_Control(scpi_t * context, scpi_ctrl_name_t ctrl, scpi_reg_val_t val) {
    char b[16];

    if (SCPI_CTRL_SRQ == ctrl) {
        iprintf("**SRQ: 0x%X (%d)\r\n", val, val);
    } else {
        iprintf("**CTRL %02x: 0x%X (%d)\r\n", ctrl, val, val);
    }

    if (context->user_context != NULL) {
        user_data_t * u = (user_data_t *) (context->user_context);
        if (u->control_io) {
            snprintf(b, sizeof (b), "SRQ%d\r\n", val);
            return netconn_write(u->control_io, b, strlen(b), NETCONN_COPY) == ERR_OK ? SCPI_RES_OK : SCPI_RES_ERR;
        }
    }
    return SCPI_RES_OK;
}

scpi_result_t __attribute__((weak)) SCPI_Reset(scpi_t * context) {
    (void) context;
    iprintf("**Reset\r\n");
    return SCPI_RES_OK;
}

scpi_result_t SCPI_SystemCommTcpipControlQ(scpi_t * context) {
    SCPI_ResultInt(context, SCPI_CONTROL_PORT);
    return SCPI_RES_OK;
}

static void setEseReq(void) {
    SCPI_RegSetBits(&scpi_context, SCPI_REG_ESR, ESR_REQ);
}

static void setError(int16_t err) {
    SCPI_ErrorPush(&scpi_context, err);
}

void SCPI_RequestControl(void) {
    BaseType_t xReturned;
    queue_event_t msg;
    msg.cmd = SCPI_MSG_SET_ESE_REQ;

    /* Avoid sending evtQueue message if ESR_REQ is already set
    if((SCPI_RegGet(&scpi_context, SCPI_REG_ESR) & ESR_REQ) == 0) {
        xQueueSend(user_data.evtQueue, &msg, 1000);
    }
     */

    xReturned = xQueueSend(user_data.evtQueue, &msg, 1000);
    /* @fixme: replace by real error handling code */
    LWIP_ASSERT("SCPI_RequestControl failed", xReturned == pdPASS);
}

void SCPI_AddError(int16_t err) {
    BaseType_t xReturned;
    queue_event_t msg;
    msg.cmd = SCPI_MSG_SET_ERROR;
    msg.param2 = err;

    xReturned = xQueueSend(user_data.evtQueue, &msg, 1000);
    /* @fixme: replace by real error handling code */
    LWIP_ASSERT("SCPI_AddError failed", xReturned == pdPASS);
}

void scpi_netconn_callback(struct netconn * conn, enum netconn_evt evt, u16_t len) {
    BaseType_t xReturned;
    queue_event_t msg;
    (void) len;


    if (evt == NETCONN_EVT_RCVPLUS) {
        msg.cmd = SCPI_MSG_TEST;
        if (conn == user_data.io) {
            msg.cmd = SCPI_MSG_IO;
        } else if (conn == user_data.io_listen) {
            msg.cmd = SCPI_MSG_IO_LISTEN;
        } else if (conn == user_data.control_io) {
            msg.cmd = SCPI_MSG_CONTROL_IO;
        } else if (conn == user_data.control_io_listen) {
            msg.cmd = SCPI_MSG_CONTROL_IO_LISTEN;
        }
        xReturned = xQueueSend(user_data.evtQueue, &msg, 1000);
        /* @fixme: replace by real error handling code */
        LWIP_ASSERT("scpi_netconn_callback failed", xReturned == pdPASS);
    }
}

static struct netconn * createServer(int port) {
    struct netconn * conn;
    err_t err;

    conn = netconn_new_with_callback(NETCONN_TCP, scpi_netconn_callback);
    if (conn == NULL) {
        return NULL;
    }

    err = netconn_bind(conn, NULL, port);
    if (err != ERR_OK) {
        netconn_delete(conn);
        return NULL;
    }


    netconn_listen(conn);

    return conn;
}

static void waitServer(user_data_t * user_data, queue_event_t * evt) {
    /* 5s timeout */
    if (xQueueReceive(user_data->evtQueue, evt, 5000 * portTICK_RATE_MS) != pdPASS) {
        evt->cmd = SCPI_MSG_TIMEOUT;
    }
}

static int processIoListen(scpi_t * context, user_data_t * user_data) {
    struct netconn *newconn;

    if (netconn_accept(user_data->io_listen, &newconn) == ERR_OK) {
        if (user_data->io) {
            /* Close unwanted connection */
            netconn_close(newconn);
            netconn_delete(newconn);
        } else {
            /* connection established */
            SCPI_Event_DeviceConnected(context, newconn);
            ip_set_option(newconn->pcb.tcp, SOF_KEEPALIVE);
            newconn->pcb.tcp->keep_idle   = SCPI_KEEP_IDLE;  // Override TCP_KEEPIDLE_DEFAULT  for this connection.
            newconn->pcb.tcp->keep_intvl  = SCPI_KEEP_INTVL; // Override TCP_KEEPINTVL_DEFAULT for this connection.
            newconn->pcb.tcp->keep_cnt    = SCPI_KEEP_CNT;   // Override TCP_KEEPCNT_DEFAULT   for this connection.
            user_data->io = newconn;
        }
    }

    return 0;
}

static int processSrqIoListen(scpi_t * context, user_data_t * user_data) {
    struct netconn *newconn;

    if (netconn_accept(user_data->control_io_listen, &newconn) == ERR_OK) {
        if (user_data->control_io) {
            netconn_close(newconn);
            netconn_delete(newconn);
        } else {
            /* control connection established */
            SCPI_Event_ControlConnected(context, newconn);
            ip_set_option(newconn->pcb.tcp, SOF_KEEPALIVE);
            newconn->pcb.tcp->keep_idle   = SCPI_KEEP_IDLE;  // Override TCP_KEEPIDLE_DEFAULT  for this connection.
            newconn->pcb.tcp->keep_intvl  = SCPI_KEEP_INTVL; // Override TCP_KEEPINTVL_DEFAULT for this connection.
            newconn->pcb.tcp->keep_cnt    = SCPI_KEEP_CNT;   // Override TCP_KEEPCNT_DEFAULT   for this connection.
            user_data->control_io = newconn;
        }
    }

    return 0;
}

static void closeIo(scpi_t * context, user_data_t * user_data) {
    /* connection closed */
    SCPI_Event_DeviceDisconnected(context, user_data->io);
    netconn_close(user_data->io);
    netconn_delete(user_data->io);
    user_data->io = NULL;
}

static void closeSrqIo(scpi_t * context, user_data_t * user_data) {
    /* control connection closed */
    SCPI_Event_ControlDisconnected(context, user_data->io);
    netconn_close(user_data->control_io);
    netconn_delete(user_data->control_io);
    user_data->control_io = NULL;
}

static int processIo(user_data_t * user_data) {
    struct netbuf *inbuf;
    char* buf;
    u16_t buflen;

    if (netconn_recv(user_data->io, &inbuf) != ERR_OK) {
        goto fail1;
    }
    if (netconn_err(user_data->io) != ERR_OK) {
        goto fail2;
    }

    netbuf_data(inbuf, (void**) &buf, &buflen);

    if (buflen > 0) {
        SCPI_Input(&scpi_context, buf, buflen);
    } else {
        /* goto fail2; */
    }

    netbuf_delete(inbuf);

    return 0;

fail2:
    netbuf_delete(inbuf);
fail1:
    closeIo(user_data);

    return 0;
}

static int processSrqIo(user_data_t * user_data) {
    struct netbuf *inbuf;
    char* buf;
    u16_t buflen;

    if (netconn_recv(user_data->control_io, &inbuf) != ERR_OK) {
        goto fail1;
    }
    if (netconn_err(user_data->control_io) != ERR_OK) {
        goto fail2;
    }

    netbuf_data(inbuf, (void**) &buf, &buflen);

    if (buflen > 0) {
        /* TODO process control */
    } else {
        /* goto fail2; */
    }

    netbuf_delete(inbuf);

    return 0;

fail2:
    netbuf_delete(inbuf);
fail1:
    closeSrqIo(user_data);

    return 0;
}

/*
 *
 */
static void scpi_server_thread(void *arg) {
    queue_event_t evt;

    (void) arg;

    user_data.evtQueue = xQueueCreate(10, sizeof (queue_event_t));
    LWIP_ASSERT("user_data.evtQueue != NULL", user_data.evtQueue != NULL);

    /* user_context will be pointer to socket */
    SCPI_Init(&scpi_context,
            scpi_commands,
            &scpi_interface,
            scpi_units_def,
            SCPI_IDN1, SCPI_IDN2, SCPI_IDN3, SCPI_IDN4,
            scpi_input_buffer, SCPI_INPUT_BUFFER_LENGTH,
            scpi_error_queue_data, SCPI_ERROR_QUEUE_SIZE);

    scpi_context.user_context = &user_data;

    user_data.io_listen = createServer(SCPI_DEVICE_PORT);
    LWIP_ASSERT("user_data.io_listen != NULL", user_data.io_listen != NULL);

    user_data.control_io_listen = createServer(SCPI_CONTROL_PORT);
    LWIP_ASSERT("user_data.control_io_listen != NULL", user_data.control_io_listen != NULL);

    while (1) {
        waitServer(&user_data, &evt);

        if (evt.cmd == SCPI_MSG_TIMEOUT) { /* timeout */
            SCPI_Input(&scpi_context, NULL, 0);
        }

        if ((user_data.io_listen != NULL) && (evt.cmd == SCPI_MSG_IO_LISTEN)) {
            processIoListen(&scpi_context, &user_data);
        }

        if ((user_data.control_io_listen != NULL) && (evt.cmd == SCPI_MSG_CONTROL_IO_LISTEN)) {
            processSrqIoListen(&scpi_context, &user_data);
        }

        if ((user_data.io != NULL) && (evt.cmd == SCPI_MSG_IO)) {
            processIo(&scpi_context, &user_data);
        }

        if ((user_data.control_io != NULL) && (evt.cmd == SCPI_MSG_CONTROL_IO)) {
            processSrqIo(&scpi_context, &user_data);
        }

        if (evt.cmd == SCPI_MSG_SET_ESE_REQ) {
            setEseReq();
        }

        if (evt.cmd == SCPI_MSG_SET_ERROR) {
            setError(evt.param2);
        }

    }

    vTaskDelete(NULL);
}

void scpi_server_init(void) {
    TaskHandle_t xHandle = NULL;
    BaseType_t xReturned;
    xReturned = xTaskCreate(scpi_server_thread, "SCPI", DEFAULT_THREAD_STACKSIZE, NULL, SCPI_THREAD_PRIO, &xHandle);
    LWIP_ASSERT("scpi_server_init failed", xReturned == pdPASS);
}

/* Called by processIoListen() for additional reporting. Override on demand. */
void __attribute__((weak)) SCPI_Event_DeviceConnected(scpi_t * context, struct netconn * conn) {
    iprintf("***Connection established %s\r\n", inet_ntoa(newconn->pcb.ip->remote_ip));
    /* Remote or Eth LED ON */
}

/* Called by closeIO() for additional reporting. Override on demand. */
void __attribute__((weak)) SCPI_Event_DeviceDisconnected(scpi_t * context, struct netconn * conn) {
    iprintf("***Connection closed\r\n");
    /* Remote or Eth LED OFF */
}

/* Called by processIoListen() for additional reporting. Override on demand. */
void __attribute__((weak)) SCPI_Event_ControlConnected(scpi_t * context, struct netconn * conn) {
    iprintf("***Control Connection established %s\r\n", inet_ntoa(newconn->pcb.ip->remote_ip));
}

/* Called by closeIO() for additional reporting. Override on demand. */
void __attribute__((weak)) SCPI_Event_ControlDisconnected(scpi_t * context, struct netconn * conn) {
    iprintf("***Control Connection closed\r\n");
}

/* Called by SCPI_Error() for reporting. Override on demand. */
void __attribute__((weak)) SCPI_Event_ErrorIndicatorOn(scpi_t * context, int_fast16_t err) {
    iprintf("**ERROR: %ld, \"%s\"\r\n", (int32_t) err, SCPI_ErrorTranslate(err));
    /* New error : BEEP, Error LED ON */
}

/* Called by SCPI_Error() for reporting. Override on demand. */
void __attribute__((weak)) SCPI_Event_ErrorIndicatorOff(scpi_t * context, int_fast16_t err) {
    iprintf("**ERROR: %ld, \"%s\"\r\n", (int32_t) err, SCPI_ErrorTranslate(err));
    /* No more errors in the queue : Error LED OFF */
}
