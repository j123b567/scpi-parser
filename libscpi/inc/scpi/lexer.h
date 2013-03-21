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
 * @file   lexer.h
 * @date   Thu Mar 21 15:00:58 UTC 2013
 * 
 * @brief  SCPI Lexer
 * 
 * 
 */

#ifndef SCPI_LEXER_H
#define	SCPI_LEXER_H

#include "scpi/types.h"

#ifdef	__cplusplus
extern "C" {
#endif

enum _token_type_t {
    TokComma,
    TokSemicolon,
    TokQuiestion,
    TokNewLine,
    TokHexnum,
    TokOctnum,
    TokBinnum,
    TokProgramMnemonic,
    TokDecimalNumericProgramData,
    TokSuffixProgramData,
    TokArbitraryBlockProgramData,
    TokSingleQuoteProgramData,
    TokDoubleQuoteProgramData,
    TokProgramExpression,
    TokCompoundProgramHeader,
    TokCommonProgramHeader,
    TokWhiteSpace,
    TokUnknown,
};
typedef enum _token_type_t token_type_t;

struct _token_t {
    token_type_t type;
    const char * ptr;
    int len;
};
typedef struct _token_t token_t;

struct _lex_state_t {
    const char * buffer;
    const char * pos;
    int len;
};
typedef struct _lex_state_t lex_state_t;


int SCPI_LexWhiteSpace(lex_state_t * state, token_t * token);
int SCPI_LexCommonProgramHeader(lex_state_t * state, token_t * token);
int SCPI_LexCompoundProgramHeader(lex_state_t * state,  token_t * token);
int SCPI_LexProgramHeader(lex_state_t * state,  token_t * token);
int SCPI_LexQuestion(lex_state_t * state, token_t * token);
int SCPI_LexCharacterProgramData(lex_state_t * state, token_t * token);
int SCPI_LexDecimalNumericProgramData(lex_state_t * state, token_t * token);
int SCPI_LexSuffixProgramData(lex_state_t * state, token_t * token);
int SCPI_LexNondecimalNumericData(lex_state_t * state, token_t * token);
int SCPI_LexStringProgramData(lex_state_t * state,  token_t * token);
int SCPI_LexArbitraryBlockProgramData(lex_state_t * state, token_t * token);
int SCPI_LexProgramExpression(lex_state_t * state,  token_t * token);
int SCPI_LexComma(lex_state_t * state, token_t * token);
int SCPI_LexSemicolon(lex_state_t * state, token_t * token);
int SCPI_LexNewLine(lex_state_t * state,  token_t * token);

#ifdef	__cplusplus
}
#endif

#endif	/* SCPI_LEXER_H */

