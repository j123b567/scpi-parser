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
 * @file   scpi_parser.c
 * @date   Thu Nov 15 10:58:45 UTC 2012
 *
 * @brief  SCPI parser implementation
 *
 *
 */

#include <ctype.h>
#include <string.h>

#include "scpi/config.h"
#include "scpi/parser.h"
#include "parser_private.h"
#include "lexer_private.h"
#include "scpi/error.h"
#include "scpi/constants.h"
#include "scpi/utils.h"

/**
 * Write data to SCPI output
 * @param context
 * @param data
 * @param len - lenght of data to be written
 * @return number of bytes written
 */
static size_t writeData(scpi_t * context, const char * data, size_t len) {
    return context->interface->write(context, data, len);
}

/**
 * Flush data to SCPI output
 * @param context
 * @return
 */
static int flushData(scpi_t * context) {
    if (context && context->interface && context->interface->flush) {
        return context->interface->flush(context);
    } else {
        return SCPI_RES_OK;
    }
}

/**
 * Write result delimiter to output
 * @param context
 * @return number of bytes written
 */
static size_t writeDelimiter(scpi_t * context) {
    if (context->output_count > 0) {
        return writeData(context, ",", 1);
    } else {
        return 0;
    }
}

/**
 * Conditionaly write "New Line"
 * @param context
 * @return number of characters written
 */
static size_t writeNewLine(scpi_t * context) {
    if (context->output_count > 0) {
        size_t len;
#ifndef SCPI_LINE_ENDING
#error no termination character defined
#endif
        len = writeData(context, SCPI_LINE_ENDING, strlen(SCPI_LINE_ENDING));
        flushData(context);
        return len;
    } else {
        return 0;
    }
}

/**
 * Conditionaly write ";"
 * @param context
 * @return number of characters written
 */
static size_t writeSemicolon(scpi_t * context) {
    if (context->output_count > 0) {
        return writeData(context, ";", 1);
    } else {
        return 0;
    }
}

/**
 * Process command
 * @param context
 */
static void processCommand(scpi_t * context) {
    const scpi_command_t * cmd = context->param_list.cmd;
    lex_state_t * state = &context->param_list.lex_state;

    /* conditionaly write ; */
    writeSemicolon(context);

    context->cmd_error = FALSE;
    context->output_count = 0;
    context->input_count = 0;

    /* if callback exists - call command callback */
    if (cmd->callback != NULL) {
        if ((cmd->callback(context) != SCPI_RES_OK) && !context->cmd_error) {
            SCPI_ErrorPush(context, SCPI_ERROR_EXECUTION_ERROR);
        }
    }

    /* set error if command callback did not read all parameters */
    if (state->pos < (state->buffer + state->len) && !context->cmd_error) {
        SCPI_ErrorPush(context, SCPI_ERROR_PARAMETER_NOT_ALLOWED);
    }
}

/**
 * Cycle all patterns and search matching pattern. Execute command callback.
 * @param context
 * @result TRUE if context->paramlist is filled with correct values
 */
static scpi_bool_t findCommandHeader(scpi_t * context, const char * header, int len) {
    int32_t i;
    const scpi_command_t * cmd;

    for (i = 0; context->cmdlist[i].pattern != NULL; i++) {
        cmd = &context->cmdlist[i];
        if (matchCommand(cmd->pattern, header, len, NULL, 0)) {
            context->param_list.cmd = cmd;
            return TRUE;
        }
    }
    return FALSE;
}

/**
 * Parse one command line
 * @param context
 * @param data - complete command line
 * @param len - command line length
 * @return 1 if the last evaluated command was found
 */
int SCPI_Parse(scpi_t * context, char * data, int len) {
    int result = 0;
    scpi_parser_state_t * state;
    int r;
    scpi_token_t cmd_prev = {SCPI_TOKEN_UNKNOWN, NULL, 0};

    if (context == NULL) {
        return -1;
    }

    state = &context->parser_state;
    context->output_count = 0;

    while (1) {
        result = 0;

        r = scpiParser_detectProgramMessageUnit(state, data, len);

        if (state->programHeader.type == SCPI_TOKEN_INVALID) {
            SCPI_ErrorPush(context, SCPI_ERROR_INVALID_CHARACTER);
        } else if (state->programHeader.len > 0) {

            composeCompoundCommand(&cmd_prev, &state->programHeader);

            if (findCommandHeader(context, state->programHeader.ptr, state->programHeader.len)) {

                context->param_list.lex_state.buffer = state->programData.ptr;
                context->param_list.lex_state.pos = context->param_list.lex_state.buffer;
                context->param_list.lex_state.len = state->programData.len;
                context->param_list.cmd_raw.data = state->programHeader.ptr;
                context->param_list.cmd_raw.position = 0;
                context->param_list.cmd_raw.length = state->programHeader.len;

                processCommand(context);

                result = 1;
                cmd_prev = state->programHeader;
            } else {
                SCPI_ErrorPush(context, SCPI_ERROR_UNDEFINED_HEADER);
            }
        }

        if (r < len) {
            data += r;
            len -= r;
        } else {
            break;
        }

    }

    /* conditionaly write new line */
    writeNewLine(context);

    return result;
}

/**
 * Initialize SCPI context structure
 * @param context
 * @param command_list
 * @param buffer
 * @param interface
 */
void SCPI_Init(scpi_t * context) {
    if (context->idn[0] == NULL) {
        context->idn[0] = SCPI_DEFAULT_1_MANUFACTURE;
    }
    if (context->idn[1] == NULL) {
        context->idn[1] = SCPI_DEFAULT_2_MODEL;
    }
    if (context->idn[2] == NULL) {
        context->idn[2] = SCPI_DEFAULT_3;
    }
    if (context->idn[3] == NULL) {
        context->idn[3] = SCPI_DEFAULT_4_REVISION;
    }

    context->buffer.position = 0;
    SCPI_ErrorInit(context);
}

/**
 * Interface to the application. Adds data to system buffer and try to search
 * command line termination. If the termination is found or if len=0, command
 * parser is called.
 *
 * @param context
 * @param data - data to process
 * @param len - length of data
 * @return
 */
int SCPI_Input(scpi_t * context, const char * data, int len) {
    int result = 0;
    size_t totcmdlen = 0;
    int cmdlen = 0;

    if (len == 0) {
        context->buffer.data[context->buffer.position] = 0;
        result = SCPI_Parse(context, context->buffer.data, context->buffer.position);
        context->buffer.position = 0;
    } else {
        int buffer_free;

        buffer_free = context->buffer.length - context->buffer.position;
        if (len > (buffer_free - 1)) {
            return -1;
        }
        memcpy(&context->buffer.data[context->buffer.position], data, len);
        context->buffer.position += len;
        context->buffer.data[context->buffer.position] = 0;


        while (1) {
            cmdlen = scpiParser_detectProgramMessageUnit(&context->parser_state, context->buffer.data + totcmdlen, context->buffer.position - totcmdlen);
            totcmdlen += cmdlen;

            if (context->parser_state.termination == SCPI_MESSAGE_TERMINATION_NL) {
                result = SCPI_Parse(context, context->buffer.data, totcmdlen);
                memmove(context->buffer.data, context->buffer.data + totcmdlen, context->buffer.position - totcmdlen);
                context->buffer.position -= totcmdlen;
                totcmdlen = 0;
            } else {
                if (context->parser_state.programHeader.type == SCPI_TOKEN_UNKNOWN) break;
                if (totcmdlen >= context->buffer.position) break;
            }
        }
    }

    return result;
}

/* writing results */

/**
 * Write raw string result to the output
 * @param context
 * @param data
 * @return
 */
size_t SCPI_ResultCharacters(scpi_t * context, const char * data, size_t len) {
    size_t result = 0;
    result += writeDelimiter(context);
    result += writeData(context, data, len);
    context->output_count++;
    return result;
}

/**
 * Write integer value to the result
 * @param context
 * @param val
 * @return
 */
size_t SCPI_ResultInt(scpi_t * context, int32_t val) {
    return SCPI_ResultIntBase(context, val, 10);
}

/**
 * Return prefix of nondecimal base
 * @param base
 * @return
 */
static const char * getBasePrefix(int8_t base) {
    switch (base) {
        case 2: return "#B";
        case 8: return "#Q";
        case 16: return "#H";
        default: return NULL;
    }
}

/**
 * Write integer value in specific base to the result
 * @param context
 * @param val
 * @param base
 * @return
 */
size_t SCPI_ResultIntBase(scpi_t * context, int32_t val, int8_t base) {
    char buffer[33];
    const char * basePrefix;
    size_t result = 0;
    size_t len;

    len = SCPI_LongToStr(val, buffer, sizeof (buffer), base);
    basePrefix = getBasePrefix(base);

    result += writeDelimiter(context);
    if (basePrefix != NULL) {
        result += writeData(context, basePrefix, 2);
    }
    result += writeData(context, buffer, len);
    context->output_count++;
    return result;
}

/**
 * Write double walue to the result
 * @param context
 * @param val
 * @return
 */
size_t SCPI_ResultDouble(scpi_t * context, double val) {
    char buffer[32];
    size_t result = 0;
    size_t len = SCPI_DoubleToStr(val, buffer, sizeof (buffer));
    result += writeDelimiter(context);
    result += writeData(context, buffer, len);
    context->output_count++;
    return result;

}

/**
 * Write string withn " to the result
 * @param context
 * @param data
 * @return
 */
size_t SCPI_ResultText(scpi_t * context, const char * data) {
    size_t result = 0;
    result += writeDelimiter(context);
    result += writeData(context, "\"", 1);
    // TODO: convert " to ""
    result += writeData(context, data, strlen(data));
    result += writeData(context, "\"", 1);
    context->output_count++;
    return result;
}

/**
 * Write arbitrary block program data to the result
 * @param context
 * @param data
 * @param len
 * @return
 */
size_t SCPI_ResultArbitraryBlock(scpi_t * context, const char * data, size_t len) {
    size_t result = 0;
    char block_header[12];
    size_t header_len;
    block_header[0] = '#';
    SCPI_LongToStr(len, block_header + 2, 10, 10);

    header_len = strlen(block_header + 2);
    block_header[1] = (char)(header_len + '0');

    result += writeData(context, block_header, header_len + 2);
    result += writeData(context, data, len);

    context->output_count++;
    return result;
}

/**
 * Write boolean value to the result
 * @param context
 * @param val
 * @return
 */
size_t SCPI_ResultBool(scpi_t * context, scpi_bool_t val) {
    return SCPI_ResultIntBase(context, val ? 1 : 0, 10);
}

/* parsing parameters */

/**
 * Invalidate token
 * @param token
 * @param ptr
 */
static void invalidateToken(scpi_token_t * token, char * ptr) {
    token->len = 0;
    token->ptr = ptr;
    token->type = SCPI_TOKEN_UNKNOWN;
}

/**
 * Get one parameter from command line
 * @param context
 * @param parameter
 * @param mandatory
 * @return
 */
scpi_bool_t SCPI_Parameter(scpi_t * context, scpi_parameter_t * parameter, scpi_bool_t mandatory) {
    lex_state_t * state;

    if (!parameter) {
        SCPI_ErrorPush(context, SCPI_ERROR_SYSTEM_ERROR);
        return FALSE;
    }

    invalidateToken(parameter, NULL);

    state = &context->param_list.lex_state;

    if (state->pos >= (state->buffer + state->len)) {
        if (mandatory) {
            SCPI_ErrorPush(context, SCPI_ERROR_MISSING_PARAMETER);
        } else {
            parameter->type = SCPI_TOKEN_PROGRAM_MNEMONIC; // TODO: select something different
        }
        return FALSE;
    }
    if (context->input_count != 0) {
        scpiLex_Comma(state, parameter);
        if (parameter->type != SCPI_TOKEN_COMMA) {
            invalidateToken(parameter, NULL);
            SCPI_ErrorPush(context, SCPI_ERROR_INVALID_SEPARATOR);
            return FALSE;
        }
    }

    context->input_count++;

    scpiParser_parseProgramData(&context->param_list.lex_state, parameter);

    switch (parameter->type) {
        case SCPI_TOKEN_HEXNUM:
        case SCPI_TOKEN_OCTNUM:
        case SCPI_TOKEN_BINNUM:
        case SCPI_TOKEN_PROGRAM_MNEMONIC:
        case SCPI_TOKEN_DECIMAL_NUMERIC_PROGRAM_DATA:
        case SCPI_TOKEN_DECIMAL_NUMERIC_PROGRAM_DATA_WITH_SUFFIX:
        case SCPI_TOKEN_ARBITRARY_BLOCK_PROGRAM_DATA:
        case SCPI_TOKEN_SINGLE_QUOTE_PROGRAM_DATA:
        case SCPI_TOKEN_DOUBLE_QUOTE_PROGRAM_DATA:
        case SCPI_TOKEN_PROGRAM_EXPRESSION:
            return TRUE;
        default:
            invalidateToken(parameter, NULL);
            SCPI_ErrorPush(context, SCPI_ERROR_INVALID_STRING_DATA);
            return FALSE;
    }
}

/**
 * Detect if parameter is number
 * @param parameter
 * @param suffixAllowed
 * @return
 */
scpi_bool_t SCPI_ParamIsNumber(scpi_parameter_t * parameter, scpi_bool_t suffixAllowed) {
    switch (parameter->type) {
        case SCPI_TOKEN_HEXNUM:
        case SCPI_TOKEN_OCTNUM:
        case SCPI_TOKEN_BINNUM:
        case SCPI_TOKEN_DECIMAL_NUMERIC_PROGRAM_DATA:
            return TRUE;
        case SCPI_TOKEN_DECIMAL_NUMERIC_PROGRAM_DATA_WITH_SUFFIX:
            return suffixAllowed;
        default:
            return FALSE;
    }
}

/**
 * Convert parameter to integer
 * @param context
 * @param parameter
 * @param value result
 * @return TRUE if succesful
 */
scpi_bool_t SCPI_ParamToInt(scpi_t * context, scpi_parameter_t * parameter, int32_t * value) {

    if (!value) {
        SCPI_ErrorPush(context, SCPI_ERROR_SYSTEM_ERROR);
        return FALSE;
    }

    switch (parameter->type) {
        case SCPI_TOKEN_HEXNUM:
            return strToLong(parameter->ptr, value, 16) > 0 ? TRUE : FALSE;
        case SCPI_TOKEN_OCTNUM:
            return strToLong(parameter->ptr, value, 8) > 0 ? TRUE : FALSE;
        case SCPI_TOKEN_BINNUM:
            return strToLong(parameter->ptr, value, 2) > 0 ? TRUE : FALSE;
        case SCPI_TOKEN_DECIMAL_NUMERIC_PROGRAM_DATA:
        case SCPI_TOKEN_DECIMAL_NUMERIC_PROGRAM_DATA_WITH_SUFFIX:
            return strToLong(parameter->ptr, value, 10) > 0 ? TRUE : FALSE;
    }
    return FALSE;
}

/**
 * Convert parameter to double
 * @param context
 * @param parameter
 * @param value result
 * @return TRUE if succesful
 */
scpi_bool_t SCPI_ParamToDouble(scpi_t * context, scpi_parameter_t * parameter, double * value) {
    scpi_bool_t result = FALSE;
    int32_t valint;

    if (!value) {
        SCPI_ErrorPush(context, SCPI_ERROR_SYSTEM_ERROR);
        return FALSE;
    }

    switch (parameter->type) {
        case SCPI_TOKEN_HEXNUM:
        case SCPI_TOKEN_OCTNUM:
        case SCPI_TOKEN_BINNUM:
            result = SCPI_ParamToInt(context, parameter, &valint);
            *value = valint;
            break;
        case SCPI_TOKEN_DECIMAL_NUMERIC_PROGRAM_DATA:
        case SCPI_TOKEN_DECIMAL_NUMERIC_PROGRAM_DATA_WITH_SUFFIX:
            result = strToDouble(parameter->ptr, value) > 0 ? TRUE : FALSE;
            break;
    }
    return result;
}

/**
 * Read floating point parameter
 * @param context
 * @param value
 * @param mandatory
 * @return
 */
scpi_bool_t SCPI_ParamDouble(scpi_t * context, double * value, scpi_bool_t mandatory) {
    scpi_bool_t result;
    scpi_parameter_t param;

    if (!value) {
        SCPI_ErrorPush(context, SCPI_ERROR_SYSTEM_ERROR);
        return FALSE;
    }

    result = SCPI_Parameter(context, &param, mandatory);
    if (result) {
        if (SCPI_ParamIsNumber(&param, FALSE)) {
            SCPI_ParamToDouble(context, &param, value);
        } else if (SCPI_ParamIsNumber(&param, TRUE)) {
            SCPI_ErrorPush(context, SCPI_ERROR_SUFFIX_NOT_ALLOWED);
            result = FALSE;
        } else {
            SCPI_ErrorPush(context, SCPI_ERROR_DATA_TYPE_ERROR);
            result = FALSE;
        }
    }
    return result;
}

/**
 * Read integer parameter
 * @param context
 * @param value
 * @param mandatory
 * @return
 */
scpi_bool_t SCPI_ParamInt(scpi_t * context, int32_t * value, scpi_bool_t mandatory) {
    scpi_bool_t result;
    scpi_parameter_t param;

    if (!value) {
        SCPI_ErrorPush(context, SCPI_ERROR_SYSTEM_ERROR);
        return FALSE;
    }

    result = SCPI_Parameter(context, &param, mandatory);
    if (result) {
        if (SCPI_ParamIsNumber(&param, FALSE)) {
            SCPI_ParamToInt(context, &param, value);
        } else if (SCPI_ParamIsNumber(&param, TRUE)) {
            SCPI_ErrorPush(context, SCPI_ERROR_SUFFIX_NOT_ALLOWED);
            result = FALSE;
        } else {
            SCPI_ErrorPush(context, SCPI_ERROR_DATA_TYPE_ERROR);
            result = FALSE;
        }
    }
    return result;
}

/**
 * Read character parameter
 * @param context
 * @param value
 * @param len
 * @param mandatory
 * @return
 */
scpi_bool_t SCPI_ParamCharacters(scpi_t * context, const char ** value, size_t * len, scpi_bool_t mandatory) {
    scpi_bool_t result;
    scpi_parameter_t param;

    if (!value || !len) {
        SCPI_ErrorPush(context, SCPI_ERROR_SYSTEM_ERROR);
        return FALSE;
    }

    result = SCPI_Parameter(context, &param, mandatory);
    if (result) {
        *value = param.ptr;
        *len = param.len;

        // TODO: return also parameter type (ProgramMnemonic, ArbitraryBlockProgramData, SingleQuoteProgramData, DoubleQuoteProgramData
    }

    return result;
}

/**
 * Get arbitrary block program data and returns pointer to data
 * @param context
 * @param value result pointer to data
 * @param len result length of data
 * @param mandatory
 * @return
 */
scpi_bool_t SCPI_ParamArbitraryBlock(scpi_t * context, const char ** value, size_t * len, scpi_bool_t mandatory) {
    scpi_bool_t result;
    scpi_parameter_t param;

    if (!value || !len) {
        SCPI_ErrorPush(context, SCPI_ERROR_SYSTEM_ERROR);
        return FALSE;
    }

    result = SCPI_Parameter(context, &param, mandatory);
    if (result) {
        if (param.type == SCPI_TOKEN_ARBITRARY_BLOCK_PROGRAM_DATA) {
            *value = param.ptr;
            *len = param.len;
        } else {
            SCPI_ErrorPush(context, SCPI_ERROR_DATA_TYPE_ERROR);
            result = FALSE;
        }
    }

    return result;
}

scpi_bool_t SCPI_ParamCopyText(scpi_t * context, char * buffer, size_t buffer_len, size_t * copy_len, scpi_bool_t mandatory) {
    scpi_bool_t result;
    scpi_parameter_t param;
    size_t i_from;
    size_t i_to;
    char quote;

    if (!buffer || !copy_len) {
        SCPI_ErrorPush(context, SCPI_ERROR_SYSTEM_ERROR);
        return FALSE;
    }

    result = SCPI_Parameter(context, &param, mandatory);
    if (result) {

        switch (param.type) {
            case SCPI_TOKEN_SINGLE_QUOTE_PROGRAM_DATA:
            case SCPI_TOKEN_DOUBLE_QUOTE_PROGRAM_DATA:
                quote = param.type == SCPI_TOKEN_SINGLE_QUOTE_PROGRAM_DATA ? '\'' : '"';
                for (i_from = 0, i_to = 0; i_from < (size_t) param.len; i_from++) {
                    if (i_from >= buffer_len) {
                        break;
                    }
                    buffer[i_to] = param.ptr[i_from];
                    i_to++;
                    if (param.ptr[i_from] == quote) {
                        i_from++;
                    }
                }
                break;
            default:
                SCPI_ErrorPush(context, SCPI_ERROR_DATA_TYPE_ERROR);
                result = FALSE;
        }
    }

    return result;
}

/**
 * Convert parameter to choice
 * @param context
 * @param parameter - should be PROGRAM_MNEMONIC
 * @param options - NULL terminated list of choices
 * @param value - index to options
 * @return
 */
scpi_bool_t SCPI_ParamToChoice(scpi_t * context, scpi_parameter_t * parameter, const scpi_choice_def_t * options, int32_t * value) {
    size_t res;
    scpi_bool_t result = FALSE;

    if (!options || !value) {
        SCPI_ErrorPush(context, SCPI_ERROR_SYSTEM_ERROR);
        return FALSE;
    }

    if (parameter->type == SCPI_TOKEN_PROGRAM_MNEMONIC) {
        for (res = 0; options[res].name; ++res) {
            if (matchPattern(options[res].name, strlen(options[res].name), parameter->ptr, parameter->len, NULL)) {
                *value = options[res].tag;
                result = TRUE;
                break;
            }
        }

        if (!result) {
            SCPI_ErrorPush(context, SCPI_ERROR_ILLEGAL_PARAMETER_VALUE);
        }
    } else {
        SCPI_ErrorPush(context, SCPI_ERROR_DATA_TYPE_ERROR);
    }

    return result;
}

/**
 * Find tag in choices and returns its first textual representation
 * @param options specifications of choices numbers (patterns)
 * @param tag numerical representatio of choice
 * @param text result text
 * @return TRUE if succesfule, else FALSE
 */
scpi_bool_t SCPI_ChoiceToName(const scpi_choice_def_t * options, int32_t tag, const char ** text) {
    int i;

    for (i = 0; options[i].name != NULL; i++) {
        if (options[i].tag == tag) {
            *text = options[i].name;
            return TRUE;
        }
    }

    return FALSE;
}

/**
 * Read BOOL parameter (0,1,ON,OFF)
 * @param context
 * @param value
 * @param mandatory
 * @return
 */
scpi_bool_t SCPI_ParamBool(scpi_t * context, scpi_bool_t * value, scpi_bool_t mandatory) {
    scpi_bool_t result;
    scpi_parameter_t param;
    int32_t intval;

    scpi_choice_def_t bool_options[] = {
        {"OFF", 0},
        {"ON", 1},
        SCPI_CHOICE_LIST_END /* termination of option list */
    };

    if (!value) {
        SCPI_ErrorPush(context, SCPI_ERROR_SYSTEM_ERROR);
        return FALSE;
    }

    result = SCPI_Parameter(context, &param, mandatory);

    if (result) {
        if (param.type == SCPI_TOKEN_DECIMAL_NUMERIC_PROGRAM_DATA) {
            SCPI_ParamToInt(context, &param, &intval);
            *value = intval ? TRUE : FALSE;
        } else {
            result = SCPI_ParamToChoice(context, &param, bool_options, &intval);
            if (result) {
                *value = intval ? TRUE : FALSE;
            }
        }
    }

    return result;
}

/**
 * Read value from list of options
 * @param context
 * @param options
 * @param value
 * @param mandatory
 * @return
 */
scpi_bool_t SCPI_ParamChoice(scpi_t * context, const scpi_choice_def_t * options, int32_t * value, scpi_bool_t mandatory) {
    scpi_bool_t result;
    scpi_parameter_t param;

    if (!options || !value) {
        SCPI_ErrorPush(context, SCPI_ERROR_SYSTEM_ERROR);
        return FALSE;
    }

    result = SCPI_Parameter(context, &param, mandatory);
    if (result) {
        result = SCPI_ParamToChoice(context, &param, options, value);
    }

    return result;
}

/**
 * Parse one parameter and detect type
 * @param state
 * @param token
 * @return
 */
int scpiParser_parseProgramData(lex_state_t * state, scpi_token_t * token) {
    scpi_token_t tmp;
    int result = 0;
    int wsLen;
    int suffixLen;
    int realLen = 0;
    realLen += scpiLex_WhiteSpace(state, &tmp);

    if (result == 0) result = scpiLex_NondecimalNumericData(state, token);
    if (result == 0) result = scpiLex_CharacterProgramData(state, token);
    if (result == 0) {
        result = scpiLex_DecimalNumericProgramData(state, token);
        if (result != 0) {
            wsLen = scpiLex_WhiteSpace(state, &tmp);
            suffixLen = scpiLex_SuffixProgramData(state, &tmp);
            if (suffixLen > 0) {
                token->len += wsLen + suffixLen;
                token->type = SCPI_TOKEN_DECIMAL_NUMERIC_PROGRAM_DATA_WITH_SUFFIX;
                result = token->len;
            }
        }
    }

    if (result == 0) result = scpiLex_StringProgramData(state, token);
    if (result == 0) result = scpiLex_ArbitraryBlockProgramData(state, token);
    if (result == 0) result = scpiLex_ProgramExpression(state, token);

    realLen += scpiLex_WhiteSpace(state, &tmp);

    return result + realLen;
}

/**
 * Skip all parameters to correctly detect end of command line.
 * @param state
 * @param token
 * @param numberOfParameters
 * @return
 */
int scpiParser_parseAllProgramData(lex_state_t * state, scpi_token_t * token, int * numberOfParameters) {

    int result;
    scpi_token_t tmp;
    int paramCount = 0;

    token->len = -1;
    token->type = SCPI_TOKEN_ALL_PROGRAM_DATA;
    token->ptr = state->pos;


    for (result = 1; result != 0; result = scpiLex_Comma(state, &tmp)) {
        token->len += result;

        if (result == 0) {
            token->type = SCPI_TOKEN_UNKNOWN;
            token->len = 0;
            paramCount = -1;
            break;
        }

        result = scpiParser_parseProgramData(state, &tmp);
        if (tmp.type != SCPI_TOKEN_UNKNOWN) {
            token->len += result;
        } else {
            token->type = SCPI_TOKEN_UNKNOWN;
            token->len = 0;
            paramCount = -1;
            break;
        }
        paramCount++;
    }

    if (token->len == -1) {
        token->len = 0;
    }

    if (numberOfParameters != NULL) {
        *numberOfParameters = paramCount;
    }
    return token->len;
}

/**
 * Skip complete command line - program header and parameters
 * @param state
 * @param buffer
 * @param len
 * @return
 */
int scpiParser_detectProgramMessageUnit(scpi_parser_state_t * state, char * buffer, int len) {
    lex_state_t lex_state;
    scpi_token_t tmp;
    int result = 0;

    lex_state.buffer = lex_state.pos = buffer;
    lex_state.len = len;
    state->numberOfParameters = 0;

    /* ignore whitespace at the begginig */
    scpiLex_WhiteSpace(&lex_state, &tmp);

    if (scpiLex_ProgramHeader(&lex_state, &state->programHeader) >= 0) {
        if (scpiLex_WhiteSpace(&lex_state, &tmp) > 0) {
            scpiParser_parseAllProgramData(&lex_state, &state->programData, &state->numberOfParameters);
        } else {
            invalidateToken(&state->programData, lex_state.pos);
        }
    } else {
        invalidateToken(&state->programHeader, lex_state.buffer);
        invalidateToken(&state->programData, lex_state.buffer);
    }

    if (result == 0) result = scpiLex_NewLine(&lex_state, &tmp);
    if (result == 0) result = scpiLex_Semicolon(&lex_state, &tmp);

    if (!scpiLex_IsEos(&lex_state) && (result == 0)) {
        lex_state.pos++;

        state->programHeader.len = 1;
        state->programHeader.type = SCPI_TOKEN_INVALID;

        invalidateToken(&state->programData, lex_state.buffer);
    }

    if (SCPI_TOKEN_SEMICOLON == tmp.type) {
        state->termination = SCPI_MESSAGE_TERMINATION_SEMICOLON;
    } else if (SCPI_TOKEN_NL == tmp.type) {
        state->termination = SCPI_MESSAGE_TERMINATION_NL;
    } else {
        state->termination = SCPI_MESSAGE_TERMINATION_NONE;
    }

    return lex_state.pos - lex_state.buffer;
}

/**
 * Check current command
 *  - suitable for one handle to multiple commands
 * @param context
 * @param cmd
 * @return
 */
scpi_bool_t SCPI_IsCmd(scpi_t * context, const char * cmd) {
    const char * pattern;

    if (!context->param_list.cmd) {
        return FALSE;
    }

    pattern = context->param_list.cmd->pattern;
    return matchCommand (pattern, cmd, strlen (cmd), NULL, 0);
}

/**
 * Return the .tag field of the matching scpi_command_t
 * @param context
 * @return
 */
int32_t SCPI_CmdTag(scpi_t * context) {
    if (context->param_list.cmd) {
        return context->param_list.cmd->tag;
    } else {
        return 0;
    }
}

scpi_bool_t SCPI_Match(const char * pattern, const char * value, size_t len) {
    return matchCommand (pattern, value, len, NULL, 0);
}

scpi_bool_t SCPI_CommandNumbers(scpi_t * context, int32_t * numbers, size_t len) {
    return matchCommand (context->param_list.cmd->pattern,  context->param_list.cmd_raw.data, context->param_list.cmd_raw.length, numbers, len);
}
