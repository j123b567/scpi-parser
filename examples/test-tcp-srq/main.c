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
 * @file   main.c
 * @date   Thu Nov 15 10:58:45 UTC 2012
 *
 * @brief  TCP/IP SCPI Server
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "scpi/scpi.h"
#include "../common/scpi-def.h"

#define CONTROL_PORT 5026

typedef struct {
    int io;
    int io_listen;
    int control_io;
    int control_io_listen;
    FILE * fio;
    fd_set fds;
} user_data_t;

size_t SCPI_Write(scpi_t * context, const char * data, size_t len) {
    if (context->user_context != NULL) {
        user_data_t * u = (user_data_t *) (context->user_context);
        if (u->fio) {
            return fwrite(data, 1, len, u->fio);
        }
    }
    return 0;
}

scpi_result_t SCPI_Flush(scpi_t * context) {
    if (context->user_context != NULL) {
        user_data_t * u = (user_data_t *) (context->user_context);
        if (u->fio) {
            return fflush(u->fio) == 0 ? SCPI_RES_OK : SCPI_RES_ERR;
        }
    }
    return SCPI_RES_OK;
}

int SCPI_Error(scpi_t * context, int_fast16_t err) {
    (void) context;
    /* BEEP */
    fprintf(stderr, "**ERROR: %d, \"%s\"\r\n", (int16_t) err, SCPI_ErrorTranslate(err));
    return 0;
}

scpi_result_t SCPI_Control(scpi_t * context, scpi_ctrl_name_t ctrl, scpi_reg_val_t val) {
    char b[16];

    if (SCPI_CTRL_SRQ == ctrl) {
        fprintf(stderr, "**SRQ: 0x%X (%d)\r\n", val, val);
    } else {
        fprintf(stderr, "**CTRL %02x: 0x%X (%d)\r\n", ctrl, val, val);
    }

    if (context->user_context != NULL) {
        user_data_t * u = (user_data_t *) (context->user_context);
        if (u->control_io >= 0) {
            sprintf(b, "SRQ%d\r\n", val);
            return write(u->control_io, b, strlen(b)) > 0 ? SCPI_RES_OK : SCPI_RES_ERR;
        }
    }
    return SCPI_RES_OK;
}

scpi_result_t SCPI_Reset(scpi_t * context) {
    (void) context;

    fprintf(stderr, "**Reset\r\n");
    return SCPI_RES_OK;
}

scpi_result_t SCPI_SystemCommTcpipControlQ(scpi_t * context) {
    SCPI_ResultInt32(context, CONTROL_PORT);
    return SCPI_RES_OK;
}

static int createServer(int port) {
    int fd;
    int rc;
    int on = 1;
    struct sockaddr_in servaddr;

    /* Configure TCP Server */
    memset(&servaddr, 0, sizeof (servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    /* Create socket */
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket() failed");
        exit(-1);
    }

    /* Set address reuse enable */
    rc = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof (on));
    if (rc < 0) {
        perror("setsockopt() failed");
        close(fd);
        exit(-1);
    }

    /* Set non blocking */
    rc = ioctl(fd, FIONBIO, (char *) &on);
    if (rc < 0) {
        perror("ioctl() failed");
        close(fd);
        exit(-1);
    }

    /* Bind to socket */
    rc = bind(fd, (struct sockaddr *) &servaddr, sizeof (servaddr));
    if (rc < 0) {
        perror("bind() failed");
        close(fd);
        exit(-1);
    }

    /* Listen on socket */
    listen(fd, 1);
    if (rc < 0) {
        perror("listen() failed");
        close(fd);
        exit(-1);
    }

    return fd;
}

static int waitServer(user_data_t * user_data) {
    struct timeval timeout;
    int rc;

    FD_ZERO(&user_data->fds);

    if (user_data->io >= 0) {
        FD_SET(user_data->io, &user_data->fds);
    }

    if (user_data->io_listen >= 0) {
        FD_SET(user_data->io_listen, &user_data->fds);
    }

    if (user_data->control_io >= 0) {
        FD_SET(user_data->control_io, &user_data->fds);
    }

    if (user_data->control_io_listen >= 0) {
        FD_SET(user_data->control_io_listen, &user_data->fds);
    }

    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    rc = select(FD_SETSIZE, &user_data->fds, NULL, NULL, &timeout);

    return rc;
}

static void processIoListen(user_data_t * user_data) {
    struct sockaddr_in cliaddr;
    socklen_t clilen;
    clilen = sizeof (cliaddr);

    user_data->io = accept(user_data->io_listen, (struct sockaddr *) &cliaddr, &clilen);
    user_data->fio = fdopen(user_data->io, "r+");

    printf("Connection established %s\r\n", inet_ntoa(cliaddr.sin_addr));
}

static void processSrqIoListen(user_data_t * user_data) {
    struct sockaddr_in cliaddr;
    socklen_t clilen;
    clilen = sizeof (cliaddr);

    user_data->control_io = accept(user_data->control_io_listen, (struct sockaddr *) &cliaddr, &clilen);
    printf("Control Connection established %s\r\n", inet_ntoa(cliaddr.sin_addr));
}

static void closeIo(user_data_t * user_data) {
    fclose(user_data->fio);
    user_data->fio = NULL;
    user_data->io = -1;
}

static void closeSrqIo(user_data_t * user_data) {
    close(user_data->control_io);
    user_data->control_io = -1;
}

static void processIo(user_data_t * user_data) {
    int rc;
    char smbuffer[10];
    rc = recv(user_data->io, smbuffer, sizeof (smbuffer), 0);
    if (rc < 0) {
        if (errno != EWOULDBLOCK) {
            closeIo(user_data);
            perror("  recv() failed");
        }
    } else if (rc == 0) {
        closeIo(user_data);
        printf("Connection closed\r\n");
    } else {
        SCPI_Input(&scpi_context, smbuffer, rc);
    }
}

static void processSrqIo(user_data_t * user_data) {
    int rc;
    char smbuffer[10];
    rc = recv(user_data->control_io, smbuffer, sizeof (smbuffer), 0);
    if (rc < 0) {
        if (errno != EWOULDBLOCK) {
            closeSrqIo(user_data);
            perror("  recv() failed");
        }
    } else if (rc == 0) {
        closeSrqIo(user_data);
        printf("Control Connection closed\r\n");
    } else {
        /* nothing to do */
    }
}

/*
 *
 */
int main(int argc, char** argv) {
    (void) argc;
    (void) argv;
    int rc;

#ifdef __cplusplus
    user_data_t user_data = {
        /*.io_listen =*/ -1,
        /*.io =*/ -1,
        /*.control_io_listen =*/ -1,
        /*.control_io =*/ -1,
        /*.fio =*/ NULL,
        /*.fds =*/ 0,
    };
#else
    user_data_t user_data = {
        .io_listen = -1,
        .io = -1,
        .control_io_listen = -1,
        .control_io = -1,
        .fio = NULL,
    };
#endif

    /* user_context will be pointer to socket */
    SCPI_Init(&scpi_context,
            scpi_commands,
            &scpi_interface,
            scpi_units_def,
            SCPI_IDN1, SCPI_IDN2, SCPI_IDN3, SCPI_IDN4,
            scpi_input_buffer, SCPI_INPUT_BUFFER_LENGTH,
            scpi_error_queue_data, SCPI_ERROR_QUEUE_SIZE);
    scpi_context.user_context = &user_data;

    user_data.io_listen = createServer(5025);
    user_data.control_io_listen = createServer(CONTROL_PORT);

    while (1) {
        rc = waitServer(&user_data);

        if (rc < 0) { /* failed */
            perror("select failed");
            exit(-1);
        }

        if (rc == 0) { /* timeout */
            SCPI_Input(&scpi_context, NULL, 0);
        }

        if ((user_data.io_listen >= 0) && FD_ISSET(user_data.io_listen, &user_data.fds)) {
            processIoListen(&user_data);
        }

        if ((user_data.control_io_listen >= 0) && FD_ISSET(user_data.control_io_listen, &user_data.fds)) {
            processSrqIoListen(&user_data);
        }

        if ((user_data.io >= 0) && FD_ISSET(user_data.io, &user_data.fds)) {
            processIo(&user_data);
        }

        if ((user_data.control_io >= 0) && FD_ISSET(user_data.control_io, &user_data.fds)) {
            processSrqIo(&user_data);
        }

    }

    return (EXIT_SUCCESS);
}

