/*
 * SCPI Commands Header for TCP Demo
 */

#ifndef SCPI_COMMANDS_H
#define SCPI_COMMANDS_H

#include <scpi/scpi.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* External declaration of SCPI commands array */
extern const scpi_command_t scpi_commands[];

#ifdef __cplusplus
}
#endif

#endif /* SCPI_COMMANDS_H */ 