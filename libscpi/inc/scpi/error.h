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
 * @file   scpi_error.h
 * @date   Thu Nov 15 10:58:45 UTC 2012
 *
 * @brief  Error handling and storing routines
 *
 *
 */

#ifndef SCPI_ERROR_H
#define  SCPI_ERROR_H

#include "scpi/types.h"
#include "scpi/config.h"

#ifdef   __cplusplus
extern "C" {
#endif

    void SCPI_ErrorInit(scpi_t * context);
    void SCPI_ErrorClear(scpi_t * context);
    int16_t SCPI_ErrorPop(scpi_t * context);
    void SCPI_ErrorPush(scpi_t * context, int16_t err);
    int32_t SCPI_ErrorCount(scpi_t * context);
    const char * SCPI_ErrorTranslate(int16_t err);


/* http://en.wikipedia.org/wiki/X_Macro */
#if ((USED_SCPI_ERROR_LIST == ERR_SCPI_MINIMUM) || (USED_SCPI_ERROR_LIST == ERR_SCPI_MIN_PLUS_USER))

#define LIST_OF_ERRORS \
    X(SCPI_ERROR_INVALID_CHARACTER,          -101, "Invalid character")                            \
    X(SCPI_ERROR_SYNTAX,                     -102, "Syntax error")                                 \
    X(SCPI_ERROR_INVALID_SEPARATOR,          -103, "Invalid separator")                            \
    X(SCPI_ERROR_DATA_TYPE_ERROR,            -104, "Data type error")                              \
    X(SCPI_ERROR_PARAMETER_NOT_ALLOWED,      -108, "Parameter not allowed")                        \
    X(SCPI_ERROR_MISSING_PARAMETER,          -109, "Missing parameter")                            \
    X(SCPI_ERROR_UNDEFINED_HEADER,           -113, "Undefined header")                             \
    X(SCPI_ERROR_INVALID_SUFFIX,             -131, "Invalid suffix")                               \
    X(SCPI_ERROR_SUFFIX_NOT_ALLOWED,         -138, "Suffix not allowed")                           \
    X(SCPI_ERROR_INVALID_STRING_DATA,        -151, "Invalid string data")                          \
    X(SCPI_ERROR_INVALID_BLOCK_DATA,         -161, "Invalid block data")                           \
    X(SCPI_ERROR_EXECUTION_ERROR,            -200, "Execution error")                              \
    X(SCPI_ERROR_ILLEGAL_PARAMETER_VALUE,    -224,"Illegal parameter value")                       \
    X(SCPI_ERROR_SYSTEM_ERROR,               -310, "System error")                                 \


#elif ((USED_SCPI_ERROR_LIST == ERR_SCPI_FULL) || (USED_SCPI_ERROR_LIST == ERR_SCPI_FULL_PLUS_USER))

#define LIST_OF_ERRORS \
    X(SCPI_ERROR_COMMAND,                    -100, "Command error")                                \
    X(SCPI_ERROR_INVALID_CHARACTER,          -101, "Invalid character")                            \
    X(SCPI_ERROR_SYNTAX,                     -102, "Syntax error")                                 \
    X(SCPI_ERROR_INVALID_SEPARATOR,          -103, "Invalid separator")                            \
    X(SCPI_ERROR_DATA_TYPE_ERROR,            -104, "Data type error")                              \
    X(SCPI_ERROR_GET_NOT_ALLOWED,            -105, "GET not allowed")                              \
    X(SCPI_ERROR_PARAMETER_NOT_ALLOWED,      -108, "Parameter not allowed")                        \
    X(SCPI_ERROR_MISSING_PARAMETER,          -109, "Missing parameter")                            \
    X(SCPI_ERROR_COMMAND_HEADER,             -110, "Command header error")                         \
    X(SCPI_ERROR_HEADER_SEPARATOR,           -111, "Header separator error")                       \
    X(SCPI_ERROR_PRG_MNEMONIC_TOO_LONG,      -112, "Program mnemonic too long")                    \
    X(SCPI_ERROR_UNDEFINED_HEADER,           -113, "Undefined header")                             \
    X(SCPI_ERROR_HEADER_SUFFIX_OUTOFRANGE,   -114, "Header suffix out of range")                   \
    X(SCPI_ERROR_UNEXP_NUM_OF_PARAMETER,     -115, "Unexpected number of parameters")              \
    X(SCPI_ERROR_NUMERIC_DATA_ERROR,         -120, "Numeric data error")                           \
    X(SCPI_ERROR_INVAL_CHAR_IN_NUMBER,       -121, "Invalid character in number")                  \
    X(SCPI_ERROR_EXPONENT_TOO_LONG,          -123, "Exponent too large")                           \
    X(SCPI_ERROR_TOO_MANY_DIGITS,            -124, "Too many digits")                              \
    X(SCPI_ERROR_NUMERIC_DATA_NOT_ALLOWED,   -128, "Numeric data not allowed")                     \
    X(SCPI_ERROR_SUFFIX_ERROR,               -130, "Suffix error")                                 \
    X(SCPI_ERROR_INVALID_SUFFIX,             -131, "Invalid suffix")                               \
    X(SCPI_ERROR_SUFFIX_TOO_LONG,            -134, "Suffix too long")                              \
    X(SCPI_ERROR_SUFFIX_NOT_ALLOWED,         -138, "Suffix not allowed")                           \
    X(SCPI_ERROR_CHARACTER_DATA_ERROR,       -140, "Character data error")                         \
    X(SCPI_ERROR_INVAL_CHARACTER_DATA,       -141, "Invalid character data")                       \
    X(SCPI_ERROR_CHARACTER_DATA_TOO_LONG,    -144, "Character data too long")                      \
    X(SCPI_ERROR_CHARACTER_DATA_NOT_ALLOWED, -148, "Character data not allowed")                   \
    X(SCPI_ERROR_STRING_DATA_ERROR,          -150, "String data error")                            \
    X(SCPI_ERROR_INVALID_STRING_DATA,        -151, "Invalid string data")                          \
    X(SCPI_ERROR_STRING_DATA_NOT_ALLOWED,    -158, "String data not allowed")                      \
    X(SCPI_ERROR_BLOCK_DATA_ERROR,           -160, "Block data error")                             \
    X(SCPI_ERROR_INVALID_BLOCK_DATA,         -161, "Invalid block data")                           \
    X(SCPI_ERROR_BLOCK_DATA_NOT_ALLOWED,     -168, "Block data not allowed")                       \
    X(SCPI_ERROR_EXPRESSION_PARSING_ERROR,    -170, "Expression error")                             \
    X(SCPI_ERROR_INVAL_EXPRESSION,           -171, "Invalid expression")                           \
    X(SCPI_ERROR_EXPRESSION_DATA_NOT_ALLOWED,-178, "Expression data not allowed")                  \
    X(SCPI_ERROR_MACRO_DEFINITION_ERROR,     -180, "Macro error")                                  \
    X(SCPI_ERROR_INVAL_OUTSIDE_MACRO_DEF,    -181, "Invalid outside macro definition")             \
    X(SCPI_ERROR_INVAL_INSIDE_MACRO_DEF,     -183, "Invalid inside macro definition")              \
    X(SCPI_ERROR_MACRO_PARAMETER_ERROR,      -184, "Macro parameter error")                        \
    X(SCPI_ERROR_EXECUTION_ERROR,            -200, "Execution error")                              \
    X(SCPI_ERROR_INVAL_WHILE_IN_LOCAL,       -201, "Invalid while in local")                       \
    X(SCPI_ERROR_SETTINGS_LOST_DUE_TO_RTL,   -202, "Settings lost due to rtl")                     \
    X(SCPI_ERROR_COMMAND_PROTECTED,          -203, "Command protected TK024")                      \
    X(SCPI_ERROR_TRIGGER_ERROR,              -210, "Trigger error")                                \
    X(SCPI_ERROR_TRIGGER_IGNORED,            -211, "Trigger ignored")                              \
    X(SCPI_ERROR_ARM_IGNORED,                -212, "Arm ignored")                                  \
    X(SCPI_ERROR_INIT_IGNORED,               -213, "Init ignored")                                 \
    X(SCPI_ERROR_TRIGGER_DEADLOCK,           -214, "Trigger deadlock")                             \
    X(SCPI_ERROR_ARM_DEADLOCK,               -215, "Arm deadlock")                                 \
    X(SCPI_ERROR_PARAMETER_ERROR,            -220, "Parameter error")                              \
    X(SCPI_ERROR_SETTINGS_CONFLICT,          -221, "Settings conflict")                            \
    X(SCPI_ERROR_DATA_OUT_OF_RANGE,          -222, "Data out of range")                            \
    X(SCPI_ERROR_TOO_MUCH_DATA,              -223, "Too much data")                                \
    X(SCPI_ERROR_ILLEGAL_PARAMETER_VALUE,    -224, "Illegal parameter value")                      \
    X(SCPI_ERROR_OUT_OF_MEMORY_FOR_REQ_OP,   -225, "Out of memory")                                \
    X(SCPI_ERROR_LISTS_NOT_SAME_LENGTH,      -226, "Lists not same length")                        \
    X(SCPI_ERROR_DATA_CORRUPT,               -230, "Data corrupt or stale")                        \
    X(SCPI_ERROR_DATA_QUESTIONABLE,          -231, "Data questionable")                            \
    X(SCPI_ERROR_INVAL_VERSION,              -233, "Invalid version")                              \
    X(SCPI_ERROR_HARDWARE_ERROR,             -240, "Hardware error")                               \
    X(SCPI_ERROR_HARDWARE_MISSING,           -241, "Hardware missing")                             \
    X(SCPI_ERROR_MASS_STORAGE_ERROR,         -250, "Mass storage error")                           \
    X(SCPI_ERROR_MISSING_MASS_STORAGE,       -251, "Missing mass storage")                         \
    X(SCPI_ERROR_MISSING_MASS_MEDIA,         -252, "Missing media")                                \
    X(SCPI_ERROR_CORRUPT_MEDIA,              -253, "Corrupt media")                                \
    X(SCPI_ERROR_MEDIA_FULL,                 -254, "Media full")                                   \
    X(SCPI_ERROR_DIRECTORY_FULL,             -255, "Directory full")                               \
    X(SCPI_ERROR_FILE_NAME_NOT_FOUND,        -256, "File name not found")                          \
    X(SCPI_ERROR_FILE_NAME_ERROR,            -257, "File name error")                              \
    X(SCPI_ERROR_MEDIA_PROTECTED,            -258, "Media protected")                              \
    X(SCPI_ERROR_EXPRESSION_EXECUTING_ERROR, -260, "Expression error")                             \
    X(SCPI_ERROR_MATH_ERROR_IN_EXPRESSION,   -261, "Math error in expression")                     \
    X(SCPI_ERROR_MACRO_UNDEF_EXEC_ERROR,     -270, "Macro error")                                  \
    X(SCPI_ERROR_MACRO_SYNTAX_ERROR,         -271, "Macro syntax error")                           \
    X(SCPI_ERROR_MACRO_EXECUTION_ERROR,      -272, "Macro execution error")                        \
    X(SCPI_ERROR_ILLEGAL_MACRO_LABEL,        -273, "Illegal macro label")                          \
    X(SCPI_ERROR_IMPROPER_USED_MACRO_PARAM,  -274, "Macro parameter error")                        \
    X(SCPI_ERROR_MACRO_DEFINITION_TOO_LONG,  -275, "Macro definition too long")                    \
    X(SCPI_ERROR_MACRO_RECURSION_ERROR,      -276, "Macro recursion error")                        \
    X(SCPI_ERROR_MACRO_REDEF_NOT_ALLOWED,    -277, "Macro redefinition not allowed")               \
    X(SCPI_ERROR_MACRO_HEADER_NOT_FOUND,     -278, "Macro header not found")                       \
    X(SCPI_ERROR_PROGRAM_ERROR,              -280, "Program error")                                \
    X(SCPI_ERROR_CANNOT_CREATE_PROGRAM,      -281, "Cannot create program")                        \
    X(SCPI_ERROR_ILLEGAL_PROGRAM_NAME,       -282, "Illegal program name")                         \
    X(SCPI_ERROR_ILLEGAL_VARIABLE_NAME,      -283, "Illegal variable name")                        \
    X(SCPI_ERROR_PROGRAM_CURRENTLY_RUNNING,  -284, "Program currently running")                    \
    X(SCPI_ERROR_PROGRAM_SYNTAX_ERROR,       -285, "Program syntax error")                         \
    X(SCPI_ERROR_PROGRAM_RUNTIME_ERROR,      -286, "Program runtime error")                        \
    X(SCPI_ERROR_MEMORY_USE_ERROR,           -290, "Memory use error")                             \
    X(SCPI_ERROR_OUT_OF_MEMORY,              -291, "Out of memory")                                \
    X(SCPI_ERROR_REF_NAME_DOES_NOT_EXIST,    -292, "Referenced name does not exist")               \
    X(SCPI_ERROR_REF_NAME_ALREADY_EXISTS,    -293, "Referenced name already exists")               \
    X(SCPI_ERROR_INCOMPATIBLE_TYPE,          -294, "Incompatible type")                            \
    X(SCPI_ERROR_DEVICE_ERROR,               -300, "Device specific error")                        \
    X(SCPI_ERROR_SYSTEM_ERROR,               -310, "System error")                                 \
    X(SCPI_ERROR_MEMORY_ERROR,               -311, "Memory error")                                 \
    X(SCPI_ERROR_PUD_MEMORY_LOST,            -312, "PUD memory lost")                              \
    X(SCPI_ERROR_CALIBRATION_MEMORY_LOST,    -313, "Calibration memory lost")                      \
    X(SCPI_ERROR_SAVE_RECALL_MEMORY_LOST,    -314, "Save/recall memory lost")                      \
    X(SCPI_ERROR_CONFIGURATION_MEMORY_LOST,  -315, "Configuration memory lost")                    \
    X(SCPI_ERROR_STORAGE_FAULT,              -320, "Storage fault")                                \
    X(SCPI_ERROR_OUT_OF_DEVICE_MEMORY,       -321, "Out of memory")                                \
    X(SCPI_ERROR_SELF_TEST_FAILED,           -330, "Self-test failed")                             \
    X(SCPI_ERROR_CALIBRATION_FAILED,         -340, "Calibration failed")                           \
    X(SCPI_ERROR_QUEUE_OVERFLOW,             -350, "Queue overflow")                               \
    X(SCPI_ERROR_COMMUNICATION_ERROR,        -360, "Communication error")                          \
    X(SCPI_ERROR_PARITY_ERROR_IN_CMD_MSG,    -361, "Parity error in program message")              \
    X(SCPI_ERROR_FRAMING_ERROR_IN_CMD_MSG,   -362, "Framing error in program message")             \
    X(SCPI_ERROR_INPUT_BUFFER_OVERRUN,       -363, "Input buffer overrun")                         \
    X(SCPI_ERROR_TIME_OUT,                   -365, "Time out error")                               \
    X(SCPI_ERROR_QUERY_ERROR,                -400, "Query error")                                  \
    X(SCPI_ERROR_QUERY_INTERRUPTED,          -410, "Query INTERRUPTED")                            \
    X(SCPI_ERROR_QUERY_UNTERMINATED,         -420, "Query UNTERMINATED")                           \
    X(SCPI_ERROR_QUERY_DEADLOCKED,           -430, "Query DEADLOCKED")                             \
    X(SCPI_ERROR_QUERY_UNTERM_INDEF_RESP,    -440, "Query UNTERMINATED after indefinite response") \
    X(SCPI_ERROR_POWER_ON,                   -500, "Power on")                                     \
    X(SCPI_ERROR_USER_REQUEST,               -600, "User request")                                 \
    X(SCPI_ERROR_REQUEST_CONTROL,            -700, "Request control")                              \
    X(SCPI_ERROR_OPERATION_COMPLETE,         -800, "Operation complete")                           \

#else
#error no SCPI error list defined!
#endif

enum {
#define X(def, val, str) def = val,
LIST_OF_ERRORS
#undef X
};

#ifdef   __cplusplus
}
#endif

#endif   /* SCPI_ERROR_H */

