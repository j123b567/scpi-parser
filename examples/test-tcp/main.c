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
#include <netinet/tcp.h>

#include "scpi/scpi.h"
#include "../common/scpi-def.h"

size_t SCPI_Write(scpi_t * context, const char * data, size_t len) {
    if (context->user_context != NULL) {
        int fd = *(int *) (context->user_context);

        int state = 1;
        setsockopt(fd, IPPROTO_TCP, TCP_CORK, &state, sizeof(state));

        return write(fd, data, len);
    }
    return 0;
}

scpi_result_t SCPI_Flush(scpi_t * context) {
    if (context->user_context != NULL) {
        int fd = *(int *) (context->user_context);

        int state = 0;
        setsockopt(fd, IPPROTO_TCP, TCP_CORK, &state, sizeof(state));
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
    (void) context;

    if (SCPI_CTRL_SRQ == ctrl) {
        fprintf(stderr, "**SRQ: 0x%X (%d)\r\n", val, val);
    } else {
        fprintf(stderr, "**CTRL %02x: 0x%X (%d)\r\n", ctrl, val, val);
    }
    return SCPI_RES_OK;
}

scpi_result_t SCPI_Reset(scpi_t * context) {
    (void) context;

    fprintf(stderr, "**Reset\r\n");
    return SCPI_RES_OK;
}

scpi_result_t SCPI_SystemCommTcpipControlQ(scpi_t * context) {
    (void) context;

    return SCPI_RES_ERR;
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

static int waitServer(int fd) {
    fd_set fds;
    struct timeval timeout;
    int rc;
    int max_fd;

    FD_ZERO(&fds);
    max_fd = fd;
    FD_SET(fd, &fds);

    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    rc = select(max_fd + 1, &fds, NULL, NULL, &timeout);

    return rc;
}

/*
 *
 */
int main(int argc, char** argv) {
    (void) argc;
    (void) argv;
    int rc;

    int listenfd;
    char smbuffer[10];

    /* user_context will be pointer to socket */
    scpi_context.user_context = NULL;

    SCPI_Init(&scpi_context,
            scpi_commands,
            &scpi_interface,
            scpi_units_def,
            SCPI_IDN1, SCPI_IDN2, SCPI_IDN3, SCPI_IDN4,
            scpi_input_buffer, SCPI_INPUT_BUFFER_LENGTH,
            scpi_error_queue_data, SCPI_ERROR_QUEUE_SIZE);

    listenfd = createServer(5025);

    while (1) {
        int clifd, flag = 1;
        struct sockaddr_in cliaddr;
        socklen_t clilen;

        clilen = sizeof (cliaddr);
        clifd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen);

        if (clifd < 0) continue;

	setsockopt(clifd, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(int));

        printf("Connection established %s\r\n", inet_ntoa(cliaddr.sin_addr));

        scpi_context.user_context = &clifd;

        while (1) {
            rc = waitServer(clifd);
            if (rc < 0) { /* failed */
                perror("  recv() failed");
                break;
            }
            if (rc == 0) { /* timeout */
                SCPI_Input(&scpi_context, NULL, 0);
            }
            if (rc > 0) { /* something to read */
                rc = recv(clifd, smbuffer, sizeof (smbuffer), 0);
                if (rc < 0) {
                    if (errno != EWOULDBLOCK) {
                        perror("  recv() failed");
                        break;
                    }
                } else if (rc == 0) {
                    printf("Connection closed\r\n");
                    break;
                } else {
                    SCPI_Input(&scpi_context, smbuffer, rc);
                }
            }
        }

        close(clifd);
    }

    return (EXIT_SUCCESS);
}

