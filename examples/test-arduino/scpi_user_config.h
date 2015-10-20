#ifndef __SCPI_USER_CONFIG_H_
#define __SCPI_USER_CONFIG_H_

#ifdef	__cplusplus
extern "C" {
#endif

#define USE_COMMAND_TAGS 0
#define USE_64K_PROGMEM_FOR_CMD_LIST 1
#define USE_FULL_PROGMEM_FOR_CMD_LIST 0
#define SCPI_MAX_CMD_PATTERN_SIZE 128

// strtoull is not defined on some arduino boards
// TODO mvladic:find better way to do this
#ifndef strtoull
#define strtoull strtoul
#endif

// strtoll is not defined on some arduino boards
// TODO mvladic:find better way to do this
#ifndef strtoll
#define strtoll strtol
#endif

#ifdef	__cplusplus
}
#endif

#endif
