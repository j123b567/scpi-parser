/*
 * TCP SCPI Demo
 * 
 * This demo shows how to use the SCPI Parser library to create
 * a TCP-based SCPI server that can handle remote connections.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <scpi/scpi.h>
#include "scpi-commands.h"

// Platform-specific includes and definitions
#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <winsock2.h>
    #include <ws2tcpip.h>
    
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

#define SCPI_INPUT_BUFFER_LENGTH 256
#define SCPI_ERROR_QUEUE_SIZE 17
#define DEFAULT_PORT 5025

/* SCPI buffer and error queue */
static char scpi_input_buffer[SCPI_INPUT_BUFFER_LENGTH];
static scpi_error_t scpi_error_queue_data[SCPI_ERROR_QUEUE_SIZE];

/* SCPI Context */
static scpi_t scpi_context;

/* Test mode flag for automated testing */
static bool test_mode = false;

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

/* SCPI Interface Implementation */
static size_t SCPI_Write(scpi_t * context, const char * data, size_t len) {
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

static scpi_result_t SCPI_Flush(scpi_t * context) {
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

static int SCPI_Error(scpi_t * context, int_fast16_t err) {
    (void) context;
    printf("**ERROR: %d, \"%s\" [%s]\r\n", 
            (int16_t) err, SCPI_ErrorTranslate(err), PLATFORM_NAME);
    return 0;
}

static scpi_result_t SCPI_Control(scpi_t * context, scpi_ctrl_name_t ctrl, scpi_reg_val_t val) {
    (void) context;

    if (SCPI_CTRL_SRQ == ctrl) {
        printf("**SRQ: 0x%X (%d) [%s]\r\n", val, val, PLATFORM_NAME);
    } else {
        printf("**CTRL %02x: 0x%X (%d) [%s]\r\n", ctrl, val, val, PLATFORM_NAME);
    }
    return SCPI_RES_OK;
}

static scpi_result_t SCPI_Reset(scpi_t * context) {
    (void) context;
    printf("**Reset [%s]\r\n", PLATFORM_NAME);
    return SCPI_RES_OK;
}

/* SCPI Interface structure */
static const scpi_interface_t scpi_interface = {
    .error = SCPI_Error,
    .write = SCPI_Write,
    .control = SCPI_Control,
    .flush = SCPI_Flush,
    .reset = SCPI_Reset,
};

/**
 * Create and configure a TCP server socket
 */
static int createServer(int port) {
    int fd;
    int rc;
    int on = 1;
    struct sockaddr_in servaddr;

    printf("Creating TCP SCPI server on port %d [%s]\n", port, PLATFORM_NAME);

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

    printf("TCP SCPI server listening on port %d [%s]\n", port, PLATFORM_NAME);
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

    timeout.tv_sec = test_mode ? 1 : 5;  // Shorter timeout in test mode
    timeout.tv_usec = 0;

    rc = select(max_fd + 1, &fds, NULL, NULL, &timeout);

    return rc;
}

/* Print help information */
static void print_help(const char* program_name) {
    printf("\n=== SCPI Parser TCP Demo ===\n\n");
    printf("Usage: %s [options]\n\n", program_name);
    printf("Options:\n");
    printf("  -p, --port <port>     Set TCP port (default: %d)\n", DEFAULT_PORT);
    printf("  --test-mode           Enable test mode (exits after startup)\n");
    printf("  -h, --help            Show this help\n");
    printf("  -v, --version         Show version information\n\n");
    
    printf("This demo creates a TCP SCPI server that accepts connections\n");
    printf("on the specified port. You can connect using telnet or any\n");
    printf("TCP client and send SCPI commands.\n\n");
    
    printf("Example connection:\n");
    printf("  telnet localhost %d\n\n", DEFAULT_PORT);
    
    printf("Example SCPI commands:\n");
    printf("  *IDN?\\r\\n                        - Get instrument identification\n");
    printf("  SOUR:VOLT 5.0\\r\\n                - Set voltage to 5.0V\n");
    printf("  SOUR:VOLT?\\r\\n                   - Query voltage setting\n");
    printf("  MEAS:VOLT?\\r\\n                   - Measure voltage\n");
    printf("  OUTP ON\\r\\n                      - Enable output\n");
}

/* Process command line arguments */
static int process_arguments(int argc, char* argv[], int* port) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            print_help(argv[0]);
            return 1; // Exit after showing help
        }
        else if (strcmp(argv[i], "--version") == 0 || strcmp(argv[i], "-v") == 0) {
            printf("SCPI Parser TCP Demo v1.0.0\n");
            printf("Built with SCPI Parser library\n");
            printf("Platform: %s\n", PLATFORM_NAME);
            return 1; // Exit after showing version
        }
        else if (strcmp(argv[i], "--test-mode") == 0) {
            test_mode = true;
            printf("Test mode enabled\n");
        }
        else if ((strcmp(argv[i], "--port") == 0 || strcmp(argv[i], "-p") == 0) && i + 1 < argc) {
            *port = atoi(argv[++i]);
            if (*port <= 0 || *port > 65535) {
                fprintf(stderr, "Error: Invalid port number %d\n", *port);
                return -1;
            }
        }
        else {
            fprintf(stderr, "Error: Unknown option %s\n", argv[i]);
            return -1;
        }
    }
    return 0; // Continue execution
}

/**
 * Main function
 */
int main(int argc, char* argv[]) {
    int port = DEFAULT_PORT;
    int rc;
    int listenfd;
    char smbuffer[10];

    /* Process command line arguments */
    rc = process_arguments(argc, argv, &port);
    if (rc != 0) {
        return (rc > 0) ? 0 : 1;
    }

    printf("Starting TCP SCPI Demo Server [%s]\n", PLATFORM_NAME);

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
            NULL, /* No units */
            "DEMO", "TCP_SCPI_PARSER", "TCP Demo", "v1.0",
            scpi_input_buffer, SCPI_INPUT_BUFFER_LENGTH,
            scpi_error_queue_data, SCPI_ERROR_QUEUE_SIZE);

    // Create server socket
    listenfd = createServer(port);
    if (listenfd < 0) {
        socket_cleanup();
        return 1;
    }

    if (test_mode) {
        printf("Test mode: Server started successfully, exiting.\n");
        close(listenfd);
        socket_cleanup();
        return 0;
    }

    printf("TCP SCPI Server ready. Waiting for connections... [%s]\n", PLATFORM_NAME);
    printf("Connect using: telnet localhost %d\n", port);

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
    
    return 0;
} 