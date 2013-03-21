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
 * @file   lexer.c
 * @date   Wed Mar 20 19:35:19 UTC 2013
 * 
 * @brief  SCPI Lexer
 * 
 * 
 */

#include <ctype.h>
#include <stdio.h>
#include <string.h>

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

/* identify character */
static int isws ( int c ) {
    if ((c == ' ') || (c == '\t')) {
        return 1;
    }
    return 0;
}

static int isbdigit ( int c ) {
    if ((c == '0') || (c == '1')) {
        return 1;
    }
    return 0;
}

static int isqdigit ( int c ) {
    if ((c == '0') || (c == '1') || (c == '2') || (c == '3') || (c == '4') || (c == '5') || (c == '6') || (c == '7')) {
        return 1;
    }
    return 0;
}

static int iseos(lex_state_t * state) {
    if ((state->buffer + state->len) <= (state->pos)) {
        return 1;
    } else {
        return 0;
    }
}

static int ischr(lex_state_t * state, char chr) {
    return (state->pos[0] == chr);
}

static int isplusmn(int c) {
    return c == '+' || c == '-';
}

static int isH(int c) {
    return c == 'h' || c == 'H';  
}

static int isB(int c) {
    return c == 'b' || c == 'B';  
}

static int isQ(int c) {
    return c == 'q' || c == 'Q';  
}

static int isE(int c) {
    return c == 'e' || c == 'E';  
}

/* skip characters */
/* 7.4.1 <PROGRAM MESSAGE UNIT SEPARATOR>*/
// TODO: static int skipProgramMessageUnitSeparator(lex_state_t * state)
static int skipWs(lex_state_t * state) {
    int someSpace = 0;
    while(!iseos(state) && isws(state->pos[0])) {
        state->pos++;
        someSpace++;
    }
    
    return someSpace;
}

/* 7.4.2 <PROGRAM DATA SEPARATOR> */
// static int skipProgramDataSeparator(lex_state_t * state)

/* 7.5.2 <PROGRAM MESSAGE TERMINATOR> */
// static int skipProgramMessageTerminator(lex_state_t * state)

static int skipDigit(lex_state_t * state) {
    if(!iseos(state) && isdigit(state->pos[0])) {
        state->pos++;
        return 1;
    } else {
        return 0;
    }
}

static int skipNumbers(lex_state_t * state) {
    int someNumbers = 0;
    while(!iseos(state) && isdigit(state->pos[0])) {
        state->pos++;
        someNumbers++;
    }
    return someNumbers;
}

static int skipPlusmn(lex_state_t * state) {
    if(!iseos(state) && isplusmn(state->pos[0])) {
        state->pos++;
        return 1;
    } else {
        return 0;
    }
}

static int skipAlpha(lex_state_t * state) {
    int someLetters = 0;
    while(!iseos(state) && isalpha(state->pos[0])) {
        state->pos++;
        someLetters++;
    }
    return someLetters;    
}

static int skipChr(lex_state_t * state, int chr) {
    if(!iseos(state) && ischr(state, chr)) {
        state->pos++;
        return 1;
    } else {
        return 0;
    }
}

static int skipSlashDot(lex_state_t * state) {
    if(!iseos(state) && (ischr(state, '/') | ischr(state, '.'))) {
        state->pos++;
        return 1;
    } else {
        return 0;
    }    
}

static int skipStar(lex_state_t * state) {
    if(!iseos(state) && ischr(state, '*')) {
        state->pos++;
        return 1;
    } else {
        return 0;
    }    
}

static int skipColon(lex_state_t * state) {
    if(!iseos(state) && ischr(state, ':')) {
        state->pos++;
        return 1;
    } else {
        return 0;
    }    
}

/* 7.6.1.2 <COMMAND PROGRAM HEADER> */
static int skipProgramMnemonic(lex_state_t * state) {
    const char * startPos = state->pos;
    if(!iseos(state) && isalpha(state->pos[0])) {
        state->pos++;
        while(!iseos(state) && (isalnum(state->pos[0]) || ischr(state, '_'))) {
            state->pos++;
        }
    }
    
    return state->pos - startPos;
}

/* tokens */
int SCPI_LexWhiteSpace(lex_state_t * state, token_t * token) {
    token->ptr = state->pos;
    
    skipWs(state);
    
    token->len = state->pos - token->ptr;  
    
    if (token->len > 0) {
        token->type = TokWhiteSpace;
    } else {
        token->type = TokUnknown;
    }        
    
    return token->len;
}

/* 7.6.1 <COMMAND PROGRAM HEADER> */
int SCPI_LexCommonProgramHeader(lex_state_t * state, token_t * token) {
    token->ptr = state->pos;
    
    if (skipStar(state)) {
        if(!skipProgramMnemonic(state)) {
            state->pos--;
        }
    }
    
    token->len = state->pos - token->ptr;
    
    if((token->len > 0)) {
        token->type = TokCommonProgramHeader;
    } else {
        token->type = TokUnknown;
        state->pos = token->ptr;
        token->len = 0;
    }
    
    return token->len;
}

int SCPI_LexCompoundProgramHeader(lex_state_t * state,  token_t * token) {
    token->ptr = state->pos;
    
    skipColon(state);
    
    if(skipProgramMnemonic(state)) {
        while(skipColon(state)) {
            if(!skipProgramMnemonic(state)) {
                // TODO: lexer error
                break;
            }
        }
    }
    
    token->len = state->pos - token->ptr;
    
    if((token->len > 0)) {
        token->type = TokCompoundProgramHeader;
    } else {
        token->type = TokUnknown;
        state->pos = token->ptr;
        token->len = 0;
    }
    
    return token->len;
}

int SCPI_LexProgramHeader(lex_state_t * state,  token_t * token) {
    int res;
    
    res = SCPI_LexCommonProgramHeader(state, token);
    if(res > 0) return res;
    
    res = SCPI_LexCompoundProgramHeader(state, token);
    if(res > 0) return res;
    
    return 0;
}

int SCPI_LexQuestion(lex_state_t * state, token_t * token) {
    token->ptr = state->pos;
    
    if (skipChr(state, '?')) {
        token->len = 1;
        token->type = TokQuiestion;
    } else {
        token->len = 0;
        token->type = TokUnknown;
    }
    
    return token->len;
}

/* 7.7.1 <CHARACTER PROGRAM DATA> */
int SCPI_LexCharacterProgramData(lex_state_t * state, token_t * token) {
    token->ptr = state->pos;
    
    if(!iseos(state) && isalpha(state->pos[0])) {
        state->pos++;
        while(!iseos(state) && (isalnum(state->pos[0]) || ischr(state, '_'))) {
            state->pos++;
        }
    }
    
    token->len = state->pos - token->ptr;
    if(token->len > 0) {
        token->type = TokProgramMnemonic;
    } else {
        token->type = TokUnknown;
    }
    
    return token->len;
}

/* 7.7.2 <DECIMAL NUMERIC PROGRAM DATA> */
static int skipMantisa(lex_state_t * state) {
    int someNumbers = 0;
    
    skipPlusmn(state);
    
    someNumbers += skipNumbers(state);
    
    if(skipChr(state, '.')) {
        someNumbers += skipNumbers(state);
    }
    
    return someNumbers > 0;
}

static int skipExponent(lex_state_t * state) {
    int someNumbers = 0;
    
    if(!iseos(state) && isE(state->pos[0])) {
        state->pos++;
        
        skipWs(state);
        
        skipPlusmn(state);
        
        someNumbers += skipNumbers(state);
    }
    
    return someNumbers > 0;
}

int SCPI_LexDecimalNumericProgramData(lex_state_t * state, token_t * token) {
    const char * rollback;
    token->ptr = state->pos;
    
    if (skipMantisa(state)) {
        rollback = state->pos;
        skipWs(state);
        if(!skipExponent(state)) {
            state->pos = rollback;
        }
    }
    
    token->len = state->pos - token->ptr;
    if(token->len > 0) {
        token->type = TokDecimalNumericProgramData;
    } else {
        token->type = TokUnknown;
    }
    
    return token->len;
}

/* 7.7.3 <SUFFIX PROGRAM DATA> */
int SCPI_LexSuffixProgramData(lex_state_t * state, token_t * token) {
    token->ptr = state->pos;
    
    skipChr(state, '/');
    
    // TODO: strict parsing  : SLASH? (ALPHA+ (MINUS? DIGIT)?) ((SLASH | DOT) (ALPHA+ (MINUS? DIGIT)?))*
    if (skipAlpha(state)) {
        skipChr(state, '-');
        skipDigit(state);
        
        while (skipSlashDot(state)) {
            skipAlpha(state);
            skipChr(state, '-');
            skipDigit(state);            
        }
    }
    
    token->len = state->pos - token->ptr;
    if((token->len > 0)) {
        token->type = TokSuffixProgramData;
    } else {
        token->type = TokUnknown;
        state->pos = token->ptr;
        token->len = 0;
    }
    
    return token->len;
}

/* 7.7.4 <NONDECIMAL NUMERIC PROGRAM DATA> */
static int skipHexNum(lex_state_t * state) {
    int someNumbers = 0;
    while(!iseos(state) && isxdigit(state->pos[0])) {
        state->pos++;
        someNumbers++;
    }
    return someNumbers;
}

static int skipOctNum(lex_state_t * state) {
    int someNumbers = 0;
    while(!iseos(state) && isqdigit(state->pos[0])) {
        state->pos++;
        someNumbers++;
    }
    return someNumbers;
}

static int skipBinNum(lex_state_t * state) {
    int someNumbers = 0;
    while(!iseos(state) && isbdigit(state->pos[0])) {
        state->pos++;
        someNumbers++;
    }
    return someNumbers;
}


int SCPI_LexNondecimalNumericData(lex_state_t * state, token_t * token) {
    token->ptr = state->pos;
    int someNumbers = 0;
    if(skipChr(state, '#')) {
        if(!iseos(state)) {
            if(isH(state->pos[0])) {
                state->pos++;
                someNumbers = skipHexNum(state);
                token->type = TokHexnum;
            } else if(isQ(state->pos[0])) {
                state->pos++;
                someNumbers = skipOctNum(state);
                token->type = TokOctnum;
            } else if(isB(state->pos[0])) {
                state->pos++;
                someNumbers = skipBinNum(state);
                token->type = TokBinnum;
            }
        }
    }
    
    if (someNumbers) {
        token->ptr += 2; // ignore number prefix
        token->len = state->pos - token->ptr;
    } else {
        token->type = TokUnknown;
        state->pos = token->ptr;
    }
    return token->len;
}


/* 7.7.5 <STRING PROGRAM DATA> */
// TODO: int SCPI_LexStringProgramData(lex_state_t * state,  token_t * token)
// void SCPI_LexSingleQuoteProgramData(lex_state_t * state)
// void SCPI_LexDoubleQuoteProgramDatalex_state_t * state)

/* 7.7.6 <ARBITRARY BLOCK PROGRAM DATA> */
// TODO: int SCPI_LexArbitraryBlockProgramData(lex_state_t * state,  token_t * token)

/* 7.7.7 <EXPRESSION PROGRAM DATA> */
// TODO: int SCPI_LexProgramExpression(lex_state_t * state,  token_t * token)

int SCPI_LexComma(lex_state_t * state, token_t * token) {
    token->ptr = state->pos;
    
    if (skipChr(state, ',')) {
        token->len = 1;
        token->type = TokComma;
    } else {
        token->len = 0;
        token->type = TokUnknown;
    }
    
    return token->len;
}

int SCPI_LexSemicolon(lex_state_t * state, token_t * token) {
    token->ptr = state->pos;
    
    if (skipChr(state, ';')) {
        token->len = 1;
        token->type = TokSemicolon;
    } else {
        token->len = 0;
        token->type = TokUnknown;
    }
    
    return token->len;
}

int SCPI_LexNewLine(lex_state_t * state,  token_t * token) {
    token->ptr = state->pos;
    
    skipChr(state, '\r');
    skipChr(state, '\n');
    
    token->len = state->pos - token->ptr;
    
    if((token->len > 0)) {
        token->type = TokNewLine;
    } else {
        token->type = TokUnknown;
        state->pos = token->ptr;
        token->len = 0;
    }
    
    return token->len;
}

const char * typeToStr(token_type_t type) {
    switch(type) {
        case TokComma: return "TokComma";
        case TokSemicolon: return "TokSemicolon";
        case TokQuiestion: return "TokQuiestion";
        case TokNewLine: return "TokNewLine";
        case TokHexnum: return "TokHexnum";
        case TokOctnum: return "TokOctnum";
        case TokBinnum: return "TokBinnum";
        case TokProgramMnemonic: return "TokProgramMnemonic";
        case TokDecimalNumericProgramData: return "TokDecimalNumericProgramData";
        case TokSuffixProgramData: return "TokSuffixProgramData";
        case TokSingleQuoteProgramData: return "TokSingleQuoteProgramData";
        case TokDoubleQuoteProgramData: return "TokDoubleQuoteProgramData";
        case TokProgramExpression: return "TokProgramExpression";
        case TokCompoundProgramHeader: return "TokCompoundProgramHeader";
        case TokCommonProgramHeader: return "TokCommonProgramHeader";
        case TokWhiteSpace: return "TokWhiteSpace";
        default: return "TokUnknown";
    }
}

void printToken(token_t * token) {
    printf("Token:\r\n");
    printf("\t->type = %s\r\n", typeToStr(token->type));
    printf("\t->ptr = %p (\"%.*s\")\r\n", token->ptr, token->len, token->ptr);
    printf("\t->len = %d\r\n", token->len);
}

#define INIT_STATE(str) do {            \
    state.buffer = state.pos = (str);   \
    state.len = strlen((str));          \
} while(0)

int main(int argc, char ** argv) {
    lex_state_t state;
    token_t token;
    
    
    //    INIT_STATE("MEAS:VOLT:DC? 1, 5\r\n");
    INIT_STATE("  \t MEAS:VOLT:DC? 1.58, .125,  5V\r\n");  
    SCPI_LexWhiteSpace(&state, &token); printToken(&token);
    
    INIT_STATE("#H123fe5A , ");
    SCPI_LexNondecimalNumericData(&state, &token); printToken(&token);
    
    INIT_STATE("#B0111010101 , ");
    SCPI_LexNondecimalNumericData(&state, &token); printToken(&token);
    
    INIT_STATE("#Q125725433 , ");
    SCPI_LexNondecimalNumericData(&state, &token); printToken(&token);
    
    INIT_STATE("abc_213as564 , ");
    SCPI_LexCharacterProgramData(&state, &token); printToken(&token);
    
    INIT_STATE("10 , ");
    SCPI_LexDecimalNumericProgramData(&state, &token); printToken(&token);
    
    INIT_STATE("-10.5 , ");
    SCPI_LexDecimalNumericProgramData(&state, &token); printToken(&token);
    
    INIT_STATE("+.5 , ");
    SCPI_LexDecimalNumericProgramData(&state, &token); printToken(&token);
    
    INIT_STATE("-. , ");
    SCPI_LexDecimalNumericProgramData(&state, &token); printToken(&token);
    
    INIT_STATE("-1 e , ");
    SCPI_LexDecimalNumericProgramData(&state, &token); printToken(&token);
    
    INIT_STATE("-1 e 3, ");
    SCPI_LexDecimalNumericProgramData(&state, &token); printToken(&token);
    
    INIT_STATE("1.5E12 , ");
    SCPI_LexDecimalNumericProgramData(&state, &token); printToken(&token);
    
    INIT_STATE("A/V , ");
    SCPI_LexSuffixProgramData(&state, &token); printToken(&token);
    
    INIT_STATE("mA.h , ");
    SCPI_LexSuffixProgramData(&state, &token); printToken(&token);
    
    INIT_STATE("*IDN?, ");
    SCPI_LexCommonProgramHeader(&state, &token); printToken(&token);
    
    INIT_STATE("*?, ");
    SCPI_LexCommonProgramHeader(&state, &token); printToken(&token);
    
    INIT_STATE("MEAS:VOLT:DC?, ");
    SCPI_LexCommonProgramHeader(&state, &token); printToken(&token);
    
    INIT_STATE("MEAS:VOLT:DC?, ");
    SCPI_LexCompoundProgramHeader(&state, &token); printToken(&token);
    
    INIT_STATE(":MEAS:VOLT:DC?, ");
    SCPI_LexCompoundProgramHeader(&state, &token); printToken(&token);
    
    INIT_STATE(":MEAS::VOLT:DC?, ");
    SCPI_LexCompoundProgramHeader(&state, &token); printToken(&token);
    
    INIT_STATE(":MEAS::VOLT:DC?, ");
    SCPI_LexProgramHeader(&state, &token); printToken(&token);
    
    INIT_STATE("MEAS:VOLT:DC?, ");
    SCPI_LexProgramHeader(&state, &token); printToken(&token);
    
    INIT_STATE("*IDN?, ");
    SCPI_LexProgramHeader(&state, &token); printToken(&token);
    return 0;
}

