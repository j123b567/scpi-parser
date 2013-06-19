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

#include "scpi/lexer.h"


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
static int skipCommonProgramHeader(lex_state_t * state) {
    if (skipStar(state)) {
        if(!skipProgramMnemonic(state)) {
            state->pos--;
        } else {
            return 1;
        }
    }
    return 0;
}

static int skipCompoundProgramHeader(lex_state_t * state) {
    const char * rollback = state->pos;
    
    skipColon(state);
    
    if(skipProgramMnemonic(state)) {
        while(skipColon(state)) {
            if(!skipProgramMnemonic(state)) {
                state->pos = rollback;
                return 0;
            }
        }
        return 1;
    } else {
        state->pos = rollback;        
        return 0;
    }
}

int SCPI_LexProgramHeader(lex_state_t * state,  token_t * token) {
    token->ptr = state->pos;
    token->type = TokUnknown;
    
    if(skipCommonProgramHeader(state)) {
        if (skipChr(state, '?')) {
            token->type = TokCommonQueryProgramHeader;
        } else {
            token->type = TokCommonProgramHeader;
        }
    } else if(skipCompoundProgramHeader(state)) {
        if (skipChr(state, '?')) {
            token->type = TokCompoundQueryProgramHeader;
        } else {
            token->type = TokCompoundProgramHeader;
        }
    }
    
    if (token->type != TokUnknown) {
        token->len = state->pos - token->ptr;
    } else {
        token->len = 0;
        state->pos = token->ptr;
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
    
    return someNumbers;
}

static int skipExponent(lex_state_t * state) {
    int someNumbers = 0;
    
    if(!iseos(state) && isE(state->pos[0])) {
        state->pos++;
        
        skipWs(state);
        
        skipPlusmn(state);
        
        someNumbers = skipNumbers(state);
    }
    
    return someNumbers;
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
    } else {
        state->pos = token->ptr;
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
        token->len = 0;
    }
    return token->len > 0 ? token->len + 2 : 0;
}


/* 7.7.5 <STRING PROGRAM DATA> */
static int isascii7bit(int c) {
    return (c >= 0) && (c <= 0x7f);
}


static int skipQuoteProgramData(lex_state_t * state, int quote) {
    while(!iseos(state)) {
        if (isascii7bit(state->pos[0]) && !ischr(state, quote)) {
            state->pos++;
        } else if (ischr(state, quote)) {
            state->pos++;
            if (!iseos(state) && ischr(state, quote)) {
                state->pos++;
            } else {
                state->pos--;
                break;
            }
        }
    }
}
    
static int skipDoubleQuoteProgramData(lex_state_t * state) {
    skipQuoteProgramData(state, '"');
}

static int skipSingleQuoteProgramData(lex_state_t * state) {
    skipQuoteProgramData(state, '\'');
}

int SCPI_LexStringProgramData(lex_state_t * state,  token_t * token) {
    token->ptr = state->pos;
    
    if (!iseos(state)) {
        if (ischr(state, '"')) {
            state->pos++;
            token->type = TokDoubleQuoteProgramData;
            skipDoubleQuoteProgramData(state);
            if (!iseos(state) && ischr(state, '"')) {
                state->pos++;
                token->len = state->pos - token->ptr;
            } else {
                state->pos = token->ptr;
            }
        } else if (ischr(state, '\'')) {
            state->pos++;
            token->type = TokSingleQuoteProgramData;
            skipSingleQuoteProgramData(state);
            if (!iseos(state) && ischr(state, '\'')) {
                state->pos++;
                token->len = state->pos - token->ptr;
            } else {
                state->pos = token->ptr;
            }
        }
    }
    
    token->len = state->pos - token->ptr;
    
    if((token->len > 0)) {
        token->ptr++;
        token->len -= 2;
    } else {
        token->type = TokUnknown;
        state->pos = token->ptr;
        token->len = 0;
    }
    
    return token->len > 0 ? token->len + 2 : 0;
}

/* 7.7.6 <ARBITRARY BLOCK PROGRAM DATA> */
static int isNonzeroDigit(int c) {
    return isdigit(c) && (c != '0');
}

int SCPI_LexArbitraryBlockProgramData(lex_state_t * state, token_t * token) {
    int i;
    int j = 0;
    const char * ptr = state->pos;
    token->ptr = state->pos;

    if (skipChr(state, '#')) {
        if (!iseos(state) && isNonzeroDigit(state->pos[0])) {
            /* Get number of digits */
            i = state->pos[0] - '0';
            state->pos++;
            
            for(; i > 0; i--) {
                if (!iseos(state) && isdigit(state->pos[0])) {
                    j *= 10;
                    j += (state->pos[0] - '0');
                    state->pos++;
                } else {
                    break;
                }
            }
            
            if(i == 0) {
                state->pos += j;
                if ((state->buffer + state->len) < (state->pos)) {
                    token->len = 0;
                } else {
                    token->ptr = state->pos - j;
                    token->len = j;
                }
            } else {
                token->len = 0;
            }
        } else {
            token->len = 0;
        }
    }

    if((token->len > 0)) {
        token->type = TokArbitraryBlockProgramData;
    } else {
        token->type = TokUnknown;
        state->pos = token->ptr;
        token->len = 0;
    }
    
    return token->len + (token->ptr - ptr);
}

/* 7.7.7 <EXPRESSION PROGRAM DATA> */
static int isProgramExpression(int c) {
    if ((c >= 0x20) && (c<=0x7e)) {
        if ((c!= 0x22)
            && (c!= 0x23) 
            && (c!= 0x27) 
            && (c!= 0x28) 
            && (c!= 0x29) 
            && (c!= 0x3B)) {
            return 1;
        }
    }
    
    return 0;
}

static void skipProgramExpression(lex_state_t * state) {
    while(!iseos(state) && isProgramExpression(state->pos[0])) {
        state->pos++;
    }
}

// TODO: 7.7.7.2-2 recursive - any program data
int SCPI_LexProgramExpression(lex_state_t * state,  token_t * token) {
    token->ptr = state->pos;
    
    if (!iseos(state) && ischr(state, '(')) {
        state->pos++;
        skipProgramExpression(state);
        
        if (!iseos(state) && ischr(state, ')')) {
            state->pos++;
            token->len = state->pos - token->ptr;
        } else {
            token->len = 0;
        }
    }
    
    if((token->len > 0)) {
        token->type = TokProgramExpression;
    } else {
        token->type = TokUnknown;
        state->pos = token->ptr;
        token->len = 0;
    }
    
    return token->len;
}

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



