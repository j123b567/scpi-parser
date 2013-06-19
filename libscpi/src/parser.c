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
#include "scpi/lexer.h"
#include "utils.h"
#include "scpi/error.h"

static void paramSkipBytes(scpi_t * context, size_t num);
static void paramSkipWhitespace(scpi_t * context);
static bool_t paramNext(scpi_t * context, bool_t mandatory);

/*
int _strnicmp(const char* s1, const char* s2, int len) {
    int result = 0;
    int i;

    for (i = 0; i < len && s1[i] && s2[i]; i++) {
        char c1 = tolower(s1[i]);
        char c2 = tolower(s2[i]);
        if (c1 != c2) {
            result = (int) c1 - (int) c2;
            break;
        }
    }

    return result;
}
 */

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
        return writeData(context, ", ", 2);
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
        len = writeData(context, "\r\n", 2);
        flushData(context);
        return len;
    } else {
        return 0;
    }
}

/**
 * Process command
 * @param context
 */
static void processCommand(scpi_t * context) {
    const scpi_command_t * cmd = context->paramlist.cmd;

    context->cmd_error = FALSE;
    context->output_count = 0;
    context->input_count = 0;

    SCPI_DEBUG_COMMAND(context);
    /* if callback exists - call command callback */
    if (cmd->callback != NULL) {
        if ((cmd->callback(context) != SCPI_RES_OK) && !context->cmd_error) {
            SCPI_ErrorPush(context, SCPI_ERROR_EXECUTION_ERROR);
        }
    }

    /* conditionaly write new line */
    writeNewLine(context);

    /* skip all whitespaces */
    paramSkipWhitespace(context);

    /* set error if command callback did not read all parameters */
    if (context->paramlist.length != 0 && !context->cmd_error) {
        SCPI_ErrorPush(context, SCPI_ERROR_PARAMETER_NOT_ALLOWED);
    }
}

/**
 * Cycle all patterns and search matching pattern. Execute command callback.
 * @param context
 * @result TRUE if context->paramlist is filled with correct values
 */
static bool_t findCommandHeader(scpi_t * context, const char * header, int len) {
    int32_t i;
    const scpi_command_t * cmd;

    for (i = 0; context->cmdlist[i].pattern != NULL; i++) {
        cmd = &context->cmdlist[i];
        if (matchCommand(cmd->pattern, header, len)) {
            context->paramlist.cmd = cmd;
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
int SCPI_Parse(scpi_t * context, const char * data, int len) {
    int result = 0;
    scpi_parser_state_t * state;
    int r;

    if (context == NULL) {
        return -1;
    }

    state = &context->parser_state;

    while (1) {
        result = 0;

        r = SCPI_DetectProgramMessageUnit(state, data, len);

        if (state->programHeader.len > 0) {
            if (findCommandHeader(context, state->programHeader.ptr, state->programHeader.len)) {

                context->paramlist.parameters = state->programData.ptr;
                context->paramlist.length = state->programData.len;

                processCommand(context);

                result = 1;
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
            cmdlen = SCPI_DetectProgramMessageUnit(&context->parser_state, context->buffer.data + totcmdlen, context->buffer.position - totcmdlen);
            totcmdlen += cmdlen;
            if (context->parser_state.termination == PmutNewLine) break;
            if (context->parser_state.programHeader.type == TokUnknown) break;
            if (totcmdlen >= context->buffer.position) break;
        }

        if (context->parser_state.termination == PmutNewLine) {
            result = SCPI_Parse(context, context->buffer.data, totcmdlen);
            memmove(context->buffer.data, context->buffer.data + totcmdlen, context->buffer.position - totcmdlen);
            context->buffer.position -= totcmdlen;
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
size_t SCPI_ResultString(scpi_t * context, const char * data) {
    size_t len = strlen(data);
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
    char buffer[12];
    size_t result = 0;
    size_t len = longToStr(val, buffer, sizeof (buffer));
    result += writeDelimiter(context);
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
    size_t len = doubleToStr(val, buffer, sizeof (buffer));
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
    result += writeData(context, data, strlen(data));
    result += writeData(context, "\"", 1);
    context->output_count++;
    return result;
}

/* parsing parameters */

/**
 * Skip num bytes from the begginig of parameters
 * @param context
 * @param num
 */
void paramSkipBytes(scpi_t * context, size_t num) {
    if (context->paramlist.length < num) {
        num = context->paramlist.length;
    }
    context->paramlist.parameters += num;
    context->paramlist.length -= num;
}

/**
 * Skip white spaces from the beggining of parameters
 * @param context
 */
void paramSkipWhitespace(scpi_t * context) {
    size_t ws = skipWhitespace(context->paramlist.parameters, context->paramlist.length);
    paramSkipBytes(context, ws);
}

/**
 * Find next parameter
 * @param context
 * @param mandatory
 * @return
 */
bool_t paramNext(scpi_t * context, bool_t mandatory) {
    paramSkipWhitespace(context);
    if (context->paramlist.length == 0) {
        if (mandatory) {
            SCPI_ErrorPush(context, SCPI_ERROR_MISSING_PARAMETER);
        }
        return FALSE;
    }
    if (context->input_count != 0) {
        if (context->paramlist.parameters[0] == ',') {
            paramSkipBytes(context, 1);
            paramSkipWhitespace(context);
        } else {
            SCPI_ErrorPush(context, SCPI_ERROR_INVALID_SEPARATOR);
            return FALSE;
        }
    }
    context->input_count++;
    return TRUE;
}

/**
 * Parse integer parameter
 * @param context
 * @param value
 * @param mandatory
 * @return
 */
bool_t SCPI_ParamInt(scpi_t * context, int32_t * value, bool_t mandatory) {
    const char * param;
    size_t param_len;
    size_t num_len;

    if (!value) {
        return FALSE;
    }

    if (!SCPI_ParamString(context, &param, &param_len, mandatory)) {
        return FALSE;
    }

    num_len = strToLong(param, value);

    if (num_len != param_len) {
        SCPI_ErrorPush(context, SCPI_ERROR_SUFFIX_NOT_ALLOWED);
        return FALSE;
    }

    return TRUE;
}

/**
 * Parse double parameter
 * @param context
 * @param value
 * @param mandatory
 * @return
 */
bool_t SCPI_ParamDouble(scpi_t * context, double * value, bool_t mandatory) {
    const char * param;
    size_t param_len;
    size_t num_len;

    if (!value) {
        return FALSE;
    }

    if (!SCPI_ParamString(context, &param, &param_len, mandatory)) {
        return FALSE;
    }

    num_len = strToDouble(param, value);

    if (num_len != param_len) {
        SCPI_ErrorPush(context, SCPI_ERROR_SUFFIX_NOT_ALLOWED);
        return FALSE;
    }

    return TRUE;
}

/**
 * Parse string parameter
 * @param context
 * @param value
 * @param len
 * @param mandatory
 * @return
 */
bool_t SCPI_ParamString(scpi_t * context, const char ** value, size_t * len, bool_t mandatory) {
    size_t length;

    if (!value || !len) {
        return FALSE;
    }

    if (!paramNext(context, mandatory)) {
        return FALSE;
    }

    if (locateStr(context->paramlist.parameters, context->paramlist.length, value, &length)) {
        paramSkipBytes(context, length);
        paramSkipWhitespace(context);
        if (len) {
            *len = length;
        }
        return TRUE;
    }

    return FALSE;
}

/**
 * Parse text parameter (can be inside "")
 * @param context
 * @param value
 * @param len
 * @param mandatory
 * @return
 */
bool_t SCPI_ParamText(scpi_t * context, const char ** value, size_t * len, bool_t mandatory) {
    size_t length;

    if (!value || !len) {
        return FALSE;
    }

    if (!paramNext(context, mandatory)) {
        return FALSE;
    }

    if (locateText(context->paramlist.parameters, context->paramlist.length, value, &length)) {
        paramSkipBytes(context, length);
        if (len) {
            *len = length;
        }
        return TRUE;
    }

    return FALSE;
}

int SCPI_ParseProgramData(lex_state_t * state, token_t * token) {
    token_t tmp;
    int result = 0;
    int wsLen;
    int suffixLen;
    int realLen = 0;
    realLen += SCPI_LexWhiteSpace(state, &tmp);

    if (result == 0) result = SCPI_LexNondecimalNumericData(state, token);
    if (result == 0) result = SCPI_LexCharacterProgramData(state, token);
    if (result == 0) {
        result = SCPI_LexDecimalNumericProgramData(state, token);
        if (result != 0) {
            wsLen = SCPI_LexWhiteSpace(state, &tmp);
            suffixLen = SCPI_LexSuffixProgramData(state, &tmp);
            if (suffixLen > 0) {
                token->len += wsLen + suffixLen;
                token->type = TokDecimalNumericProgramDataWithSuffix;
                result = token->len;
            }
        }
    }

    if (result == 0) result = SCPI_LexStringProgramData(state, token);
    if (result == 0) result = SCPI_LexArbitraryBlockProgramData(state, token);
    if (result == 0) result = SCPI_LexProgramExpression(state, token);

    realLen += SCPI_LexWhiteSpace(state, &tmp);

    return result + realLen;
}

int SCPI_ParseAllProgramData(lex_state_t * state, token_t * token, int * numberOfParameters) {

    int result;
    token_t tmp;
    int paramCount = 0;

    token->len = -1;
    token->type = TokAllProgramData;
    token->ptr = state->pos;


    for (result = 1; result != 0; result = SCPI_LexComma(state, &tmp)) {
        token->len += result;

        if (result == 0) {
            token->type = TokUnknown;
            token->len = 0;
            paramCount = -1;
            break;
        }

        result = SCPI_ParseProgramData(state, &tmp);
        if (tmp.type != TokUnknown) {
            token->len += result;
        } else {
            token->type = TokUnknown;
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

int SCPI_DetectProgramMessageUnit(scpi_parser_state_t * state, const char * buffer, int len) {
    lex_state_t lex_state;
    token_t tmp;
    int result = 0;

    lex_state.buffer = lex_state.pos = buffer;
    lex_state.len = len;

    /* ignore whitespace at the begginig */
    SCPI_LexWhiteSpace(&lex_state, &tmp);

    if (SCPI_LexProgramHeader(&lex_state, &state->programHeader) >= 0) {
        if (SCPI_LexWhiteSpace(&lex_state, &tmp) > 0) {
            SCPI_ParseAllProgramData(&lex_state, &state->programData, &state->numberOfParameters);
        } else {
            state->programData.len = 0;
            state->programData.ptr = lex_state.pos;
            state->programData.type = TokUnknown;
            state->numberOfParameters = 0;
        }
    } else {
        state->programHeader.len = 0;
        state->programData.ptr = lex_state.buffer;
        state->programHeader.type = TokUnknown;

        state->programData.len = 0;
        state->programData.ptr = lex_state.buffer;
        state->programData.type = TokUnknown;
        state->numberOfParameters = 0;
    }

    if (result == 0) result = SCPI_LexNewLine(&lex_state, &tmp);
    if (result == 0) result = SCPI_LexSemicolon(&lex_state, &tmp);

    if (TokSemicolon == tmp.type) {
        state->termination = PmutSemicolon;
    } else if (TokNewLine == tmp.type) {
        state->termination = PmutNewLine;
    } else {
        state->termination = PmutNone;
    }

    return lex_state.pos - lex_state.buffer;
}














