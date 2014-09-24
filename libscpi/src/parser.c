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
        return writeData(context, ",", 2);
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
    const scpi_command_t * cmd = context->param_list.cmd;
    lex_state_t * state = &context->param_list.lex_state;

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
        if (matchCommand(cmd->pattern, header, len)) {
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

        r = detectProgramMessageUnit(state, data, len);

        if (state->programHeader.type == TokInvalid) {
            SCPI_ErrorPush(context, SCPI_ERROR_INVALID_CHARACTER);
        } else if (state->programHeader.len > 0) {
            if (findCommandHeader(context, state->programHeader.ptr, state->programHeader.len)) {

                context->param_list.lex_state.buffer = state->programData.ptr;
                context->param_list.lex_state.pos = context->param_list.lex_state.buffer;
                context->param_list.lex_state.len = state->programData.len;
                context->param_list.cmd_raw.data = state->programHeader.ptr;
                context->param_list.cmd_raw.position = 0;
                context->param_list.cmd_raw.length = state->programHeader.len;

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
            cmdlen = detectProgramMessageUnit(&context->parser_state, context->buffer.data + totcmdlen, context->buffer.position - totcmdlen);
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

static const char * getBasePrefix(int8_t base) {
    switch (base) {
        case 2: return "#B";
        case 8: return "#Q";
        case 16: return "#H";
        default: return NULL;
    }
}

size_t SCPI_ResultIntBase(scpi_t * context, int32_t val, int8_t base) {
    char buffer[33];
    const char * basePrefix;
    size_t result = 0;
    size_t len;

    len = longToStr(val, buffer, sizeof (buffer), base);
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

scpi_bool_t SCPI_Parameter(scpi_t * context, scpi_parameter_t * parameter, scpi_bool_t mandatory) {
    scpi_token_t token;
    lex_state_t * state;
    int32_t value;

    parameter->data.ptr = NULL;
    parameter->data.len = 0;
    parameter->number.value = 0;
    parameter->number.base = 10;
    parameter->number.unit = SCPI_UNIT_NONE;
    parameter->number.type = SCPI_NUM_NUMBER;
    parameter->type = TokUnknown;

    state = &context->param_list.lex_state;

    if (state->pos >= (state->buffer + state->len)) {
        if (mandatory) {
            SCPI_ErrorPush(context, SCPI_ERROR_MISSING_PARAMETER);
        } else {
            parameter->number.type = SCPI_NUM_DEF;
            parameter->type = TokProgramMnemonic; // TODO: select something different
        }
        return FALSE;
    }
    if (context->input_count != 0) {
        lexComma(state, &token);
        if (token.type != TokComma) {
            SCPI_ErrorPush(context, SCPI_ERROR_INVALID_SEPARATOR);
            return FALSE;
        }
    }

    context->input_count++;

    parseProgramData(&context->param_list.lex_state, &token);

    parameter->type = token.type;
    parameter->data.ptr = token.ptr;
    parameter->data.len = token.len;

    switch (token.type) {
        case TokHexnum:
            parameter->number.base = 16;
            strToLong(token.ptr, &value, 16);
            parameter->number.value = value;
            return TRUE;
        case TokOctnum:
            parameter->number.base = 8;
            strToLong(token.ptr, &value, 8);
            parameter->number.value = value;
            return TRUE;
        case TokBinnum:
            parameter->number.base = 2;
            strToLong(token.ptr, &value, 2);
            parameter->number.value = value;
            return TRUE;
        case TokProgramMnemonic:
            return TRUE;
        case TokDecimalNumericProgramData:
            strToDouble(token.ptr, &parameter->number.value);
            return TRUE;
        case TokDecimalNumericProgramDataWithSuffix:
            strToDouble(token.ptr, &parameter->number.value);
            return TRUE;
        case TokArbitraryBlockProgramData:
            return TRUE;
        case TokSingleQuoteProgramData:
            // TODO: replace double "single qoute"
            return TRUE;
        case TokDoubleQuoteProgramData:
            // TODO: replace double "double qoute"
            return TRUE;
        case TokProgramExpression:
            return TRUE;
        default:
            parameter->type = TokUnknown;
            parameter->data.ptr = NULL;
            parameter->data.len = 0;
            SCPI_ErrorPush(context, SCPI_ERROR_INVALID_STRING_DATA);
            return FALSE;
    }
}

int32_t SCPI_ParamGetIntVal(scpi_t * context, scpi_parameter_t * parameter) {
    switch (parameter->type) {
        case TokHexnum:
        case TokOctnum:
        case TokBinnum:
        case TokDecimalNumericProgramData:
        case TokDecimalNumericProgramDataWithSuffix:
            return parameter->number.value;
        default:
            SCPI_ErrorPush(context, SCPI_ERROR_DATA_TYPE_ERROR);
            return 0;
    }
}

double SCPI_ParamGetDoubleVal(scpi_t * context, scpi_parameter_t * parameter) {
    return parameter->number.value;
}

void SCPI_ParamGetTextVal(scpi_t * context, scpi_parameter_t * parameter, const char ** data, int32_t * len) {
    *data = parameter->data.ptr;
    *len = parameter->data.len;
}

/* SCPI-99 7.3 Boolean Program Data */
scpi_bool_t SCPI_ParamGetBoolVal(scpi_t * context, scpi_parameter_t * parameter) {
    switch (parameter->type) {
        case TokDecimalNumericProgramData:
            return parameter->number.value ? 1 : 0;
        case TokProgramMnemonic:
            if (compareStr("ON", 2, parameter->data.ptr, parameter->data.len)) {
                return TRUE;
            } else if (compareStr("OFF", 3, parameter->data.ptr, parameter->data.len)) {
                return FALSE;
            } else {
                SCPI_ErrorPush(context, SCPI_ERROR_ILLEGAL_PARAMETER_VALUE);
                return FALSE;
            }
        default:
            SCPI_ErrorPush(context, SCPI_ERROR_DATA_TYPE_ERROR);
            return FALSE;
    }
}

/**
 * Get choice parameter
 */
int32_t SCPI_ParamGetChoiceVal(scpi_t * context, scpi_parameter_t * parameter, const char * options[]) {
    size_t res;

    if (!options) {
        SCPI_ErrorPush(context, SCPI_ERROR_SYSTEM_ERROR);
        return -1;
    }

    switch(parameter->type) {
        case TokProgramMnemonic:
            for (res = 0; options[res]; ++res) {
                if (matchPattern(options[res], strlen(options[res]), parameter->data.ptr, parameter->data.len)) {
                    return res;
                }
            }
            SCPI_ErrorPush(context, SCPI_ERROR_ILLEGAL_PARAMETER_VALUE);
            return -1;
        default:
            SCPI_ErrorPush(context, SCPI_ERROR_DATA_TYPE_ERROR);
            return -1;
    }
}

int parseProgramData(lex_state_t * state, scpi_token_t * token) {
    scpi_token_t tmp;
    int result = 0;
    int wsLen;
    int suffixLen;
    int realLen = 0;
    realLen += lexWhiteSpace(state, &tmp);

    if (result == 0) result = lexNondecimalNumericData(state, token);
    if (result == 0) result = lexCharacterProgramData(state, token);
    if (result == 0) {
        result = lexDecimalNumericProgramData(state, token);
        if (result != 0) {
            wsLen = lexWhiteSpace(state, &tmp);
            suffixLen = lexSuffixProgramData(state, &tmp);
            if (suffixLen > 0) {
                token->len += wsLen + suffixLen;
                token->type = TokDecimalNumericProgramDataWithSuffix;
                result = token->len;
            }
        }
    }

    if (result == 0) result = lexStringProgramData(state, token);
    if (result == 0) result = lexArbitraryBlockProgramData(state, token);
    if (result == 0) result = lexProgramExpression(state, token);

    realLen += lexWhiteSpace(state, &tmp);

    return result + realLen;
}

int parseAllProgramData(lex_state_t * state, scpi_token_t * token, int * numberOfParameters) {

    int result;
    scpi_token_t tmp;
    int paramCount = 0;

    token->len = -1;
    token->type = TokAllProgramData;
    token->ptr = state->pos;


    for (result = 1; result != 0; result = lexComma(state, &tmp)) {
        token->len += result;

        if (result == 0) {
            token->type = TokUnknown;
            token->len = 0;
            paramCount = -1;
            break;
        }

        result = parseProgramData(state, &tmp);
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

static void invalidateToken(scpi_token_t * token, const char * ptr) {
    token->len = 0;
    token->ptr = ptr;
    token->type = TokUnknown;
}

int detectProgramMessageUnit(scpi_parser_state_t * state, const char * buffer, int len) {
    lex_state_t lex_state;
    scpi_token_t tmp;
    int result = 0;

    lex_state.buffer = lex_state.pos = buffer;
    lex_state.len = len;
    state->numberOfParameters = 0;

    /* ignore whitespace at the begginig */
    lexWhiteSpace(&lex_state, &tmp);

    if (lexProgramHeader(&lex_state, &state->programHeader) >= 0) {
        if (lexWhiteSpace(&lex_state, &tmp) > 0) {
            parseAllProgramData(&lex_state, &state->programData, &state->numberOfParameters);
        } else {
            invalidateToken(&state->programData, lex_state.pos);
        }
    } else {
        invalidateToken(&state->programHeader, lex_state.buffer);
        invalidateToken(&state->programData, lex_state.buffer);
    }

    if (result == 0) result = lexNewLine(&lex_state, &tmp);
    if (result == 0) result = lexSemicolon(&lex_state, &tmp);

    if (!lexIsEos(&lex_state) && (result == 0)) {
        lex_state.pos++;

        state->programHeader.len = 1;
        state->programHeader.type = TokInvalid;

        invalidateToken(&state->programData, lex_state.buffer);        
    }

    if (TokSemicolon == tmp.type) {
        state->termination = PmutSemicolon;
    } else if (TokNewLine == tmp.type) {
        state->termination = PmutNewLine;
    } else {
        state->termination = PmutNone;
    }

    return lex_state.pos - lex_state.buffer;
}














