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
 * @file   main-cross.c
 * @date   Thu Nov 15 10:58:45 UTC 2012
 *
 * @brief  Cross-Platform TCP/IP SCPI Server
 *         Works on both Windows and Unix-like systems
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "scpi/scpi.h"
#include "../common/scpi-def.h"

// Platform-specific includes and definitions
#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    
    // Windows compatibility definitions
    #define close(s) closesocket(s)
    typedef int socklen_t;
    #define SHUT_RDWR SD_BOTH
    #ifndef EWOULDBLOCK
        #define EWOULDBLOCK WSAEWOULDBLOCK
    #endif
    
    // Platform name for logging
    #define PLATFORM_NAME "Windows"
    
#else
    #include <netinet/in.h>
    #include <sys/select.h>
    #include <sys/socket.h>
    #include <sys/ioctl.h>
    #include <errno.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <netinet/tcp.h>
    
    // Platform name for logging
    #define PLATFORM_NAME "Unix"
    
#endif

// Cross-platform socket functions
static int socket_startup(void);
static void socket_cleanup(void);
static int socket_set_nonblocking(int socket);
static int socket_get_error(void);
static const char* socket_error_string(int error_code);

/**
 * Initialize socket subsystem (Windows requires this)
 */
static int socket_startup(void) {
#ifdef _WIN32
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        fprintf(stderr, "WSAStartup failed: %d\n", result);
        return -1;
    }
    printf("Socket subsystem initialized on %s\n", PLATFORM_NAME);
#else
    printf("Socket subsystem ready on %s\n", PLATFORM_NAME);
#endif
    return 0;
}

/**
 * Cleanup socket subsystem
 */
static void socket_cleanup(void) {
#ifdef _WIN32
    WSACleanup();
    printf("Socket subsystem cleaned up on %s\n", PLATFORM_NAME);
#else
    printf("Socket subsystem shutdown on %s\n", PLATFORM_NAME);
#endif
}

/**
 * Set socket to non-blocking mode
 */
static int socket_set_nonblocking(int socket) {
#ifdef _WIN32
    u_long nonBlocking = 1;
    return ioctlsocket(socket, FIONBIO, &nonBlocking);
#else
    int on = 1;
    return ioctl(socket, FIONBIO, (char *) &on);
#endif
}

/**
 * Get last socket error code
 */
static int socket_get_error(void) {
#ifdef _WIN32
    return WSAGetLastError();
#else
    return errno;
#endif
}

/**
 * Get human-readable error string
 */
static const char* socket_error_string(int error_code) {
#ifdef _WIN32
    static char error_buffer[256];
    FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                   NULL, error_code, 0, error_buffer, sizeof(error_buffer), NULL);
    return error_buffer;
#else
    return strerror(error_code);
#endif
}

// SCPI Interface Implementation
size_t SCPI_Write(scpi_t * context, const char * data, size_t len) {
    if (context->user_context != NULL) {
        int fd = *(int *) (context->user_context);

#ifndef _WIN32
        // Unix-specific TCP_CORK optimization
        int state = 1;
        setsockopt(fd, IPPROTO_TCP, TCP_CORK, &state, sizeof(state));
#endif

        // Cross-platform socket send
#ifdef _WIN32
        return send(fd, data, (int)len, 0);
#else
        return write(fd, data, len);
#endif
    }
    return 0;
}

scpi_result_t SCPI_Flush(scpi_t * context) {
    if (context->user_context != NULL) {
        int fd = *(int *) (context->user_context);

#ifndef _WIN32
        // Unix-specific TCP_CORK disable to flush
        int state = 0;
        setsockopt(fd, IPPROTO_TCP, TCP_CORK, &state, sizeof(state));
#endif
    }

    return SCPI_RES_OK;
}

int SCPI_Error(scpi_t * context, int_fast16_t err) {
    (void) context;
    fprintf(stderr, "**ERROR: %d, \"%s\" [%s]\r\n", 
            (int16_t) err, SCPI_ErrorTranslate(err), PLATFORM_NAME);
    return 0;
}

scpi_result_t SCPI_Control(scpi_t * context, scpi_ctrl_name_t ctrl, scpi_reg_val_t val) {
    (void) context;

    if (SCPI_CTRL_SRQ == ctrl) {
        fprintf(stderr, "**SRQ: 0x%X (%d) [%s]\r\n", val, val, PLATFORM_NAME);
    } else {
        fprintf(stderr, "**CTRL %02x: 0x%X (%d) [%s]\r\n", ctrl, val, val, PLATFORM_NAME);
    }
    return SCPI_RES_OK;
}

scpi_result_t SCPI_Reset(scpi_t * context) {
    (void) context;
    fprintf(stderr, "**Reset [%s]\r\n", PLATFORM_NAME);
    return SCPI_RES_OK;
}

scpi_result_t SCPI_SystemCommTcpipControlQ(scpi_t * context) {
    (void) context;
    return SCPI_RES_ERR;
}

/**
 * Create and configure a TCP server socket
 */
static int createServer(int port) {
    int fd;
    int rc;
    int on = 1;
    struct sockaddr_in servaddr;

    printf("Creating server on port %d [%s]\n", port, PLATFORM_NAME);

    /* Configure TCP Server */
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons((unsigned short)port);

    /* Create socket */
    fd = (int)socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        int error = socket_get_error();
        fprintf(stderr, "socket() failed: %s (%d) [%s]\n", 
                socket_error_string(error), error, PLATFORM_NAME);
        return -1;
    }

    /* Set address reuse enable */
    rc = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(on));
    if (rc < 0) {
        int error = socket_get_error();
        fprintf(stderr, "setsockopt(SO_REUSEADDR) failed: %s (%d) [%s]\n", 
                socket_error_string(error), error, PLATFORM_NAME);
        close(fd);
        return -1;
    }

    /* Set non blocking */
    rc = socket_set_nonblocking(fd);
    if (rc < 0) {
        int error = socket_get_error();
        fprintf(stderr, "set_nonblocking() failed: %s (%d) [%s]\n", 
                socket_error_string(error), error, PLATFORM_NAME);
        close(fd);
        return -1;
    }

    /* Bind to socket */
    rc = bind(fd, (struct sockaddr *) &servaddr, sizeof(servaddr));
    if (rc < 0) {
        int error = socket_get_error();
        fprintf(stderr, "bind() failed: %s (%d) [%s]\n", 
                socket_error_string(error), error, PLATFORM_NAME);
        close(fd);
        return -1;
    }

    /* Listen on socket */
    rc = listen(fd, 1);
    if (rc < 0) {
        int error = socket_get_error();
        fprintf(stderr, "listen() failed: %s (%d) [%s]\n", 
                socket_error_string(error), error, PLATFORM_NAME);
        close(fd);
        return -1;
    }

    printf("Server listening on port %d [%s]\n", port, PLATFORM_NAME);
    return fd;
}

/**
 * Wait for activity on a socket using select()
 */
static int waitServer(int fd) {
    fd_set fds;
    struct timeval timeout;
    int rc;
    int max_fd;

    FD_ZERO(&fds);
    max_fd = fd;
    FD_SET((unsigned int)fd, &fds);

    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    rc = select(max_fd + 1, &fds, NULL, NULL, &timeout);

    return rc;
}

/**
 * Main server function
 */
int main(int argc, char** argv) {
    (void) argc;
    (void) argv;
    int rc;
    int listenfd;
    char smbuffer[10];

    printf("Starting Cross-Platform SCPI TCP Server [%s]\n", PLATFORM_NAME);

    // Initialize socket subsystem
    if (socket_startup() < 0) {
        return 1;
    }

    /* user_context will be pointer to socket */
    scpi_context.user_context = NULL;

    // Initialize SCPI
    SCPI_Init(&scpi_context,
            scpi_commands,
            &scpi_interface,
            scpi_units_def,
            SCPI_IDN1, SCPI_IDN2, SCPI_IDN3, SCPI_IDN4,
            scpi_input_buffer, SCPI_INPUT_BUFFER_LENGTH,
            scpi_error_queue_data, SCPI_ERROR_QUEUE_SIZE);

    // Create server socket
    listenfd = createServer(5025);
    if (listenfd < 0) {
        socket_cleanup();
        return 1;
    }

    printf("SCPI Server ready. Waiting for connections... [%s]\n", PLATFORM_NAME);

    // Main server loop
    while (1) {
        int clifd, flag = 1;
        struct sockaddr_in cliaddr;
        socklen_t clilen;

        clilen = sizeof(cliaddr);
        clifd = (int)accept(listenfd, (struct sockaddr *) &cliaddr, &clilen);

        if (clifd < 0) continue;

        // Disable Nagle's algorithm for better interactive response
        setsockopt(clifd, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(int));

        printf("Connection established from %s [%s]\r\n", 
               inet_ntoa(cliaddr.sin_addr), PLATFORM_NAME);

        scpi_context.user_context = &clifd;

        // Client communication loop
        while (1) {
            rc = waitServer(clifd);
            if (rc < 0) { /* failed */
                int error = socket_get_error();
                fprintf(stderr, "select() failed: %s (%d) [%s]\n", 
                        socket_error_string(error), error, PLATFORM_NAME);
                break;
            }
            if (rc == 0) { /* timeout */
                SCPI_Input(&scpi_context, NULL, 0);
            }
            if (rc > 0) { /* something to read */
                rc = recv(clifd, smbuffer, sizeof(smbuffer), 0);
                if (rc < 0) {
                    int error = socket_get_error();
#ifdef _WIN32
                    if (error != WSAEWOULDBLOCK) {
#else
                    if (error != EWOULDBLOCK) {
#endif
                        fprintf(stderr, "recv() failed: %s (%d) [%s]\n", 
                                socket_error_string(error), error, PLATFORM_NAME);
                        break;
                    }
                } else if (rc == 0) {
                    printf("Connection closed by client [%s]\r\n", PLATFORM_NAME);
                    break;
                } else {
                    SCPI_Input(&scpi_context, smbuffer, rc);
                }
            }
        }

        close(clifd);
        printf("Client disconnected [%s]\n", PLATFORM_NAME);
    }

    // Cleanup (never reached in this implementation, but good practice)
    close(listenfd);
    socket_cleanup();
    
    return (EXIT_SUCCESS);
}
