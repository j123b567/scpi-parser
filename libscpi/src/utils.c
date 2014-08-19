/*-
 * Copyright (c) 2013 Jan Breuer
 *                    Richard.hmm
 * Copyright (c) 2012 Jan Breuer
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
 * @file   scpi_utils.c
 * @date   Thu Nov 15 10:58:45 UTC 2012
 * 
 * @brief  Conversion routines and string manipulation routines
 * 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "scpi/utils_private.h"

static size_t patternSeparatorShortPos(const char * pattern, size_t len);
static size_t patternSeparatorPos(const char * pattern, size_t len);
static size_t cmdSeparatorPos(const char * cmd, size_t len);

/**
 * Find the first occurrence in str of a character in set.
 * @param str
 * @param size
 * @param set
 * @return 
 */
const char * strnpbrk(const char *str, size_t size, const char *set) {
    const char *scanp;
    long c, sc;
    const char * strend = str + size;

    while ((strend != str) && ((c = *str++) != 0)) {
        for (scanp = set; (sc = *scanp++) != '\0';)
            if (sc == c)
                return str - 1;
    }
    return (NULL);
}

/**
 * Converts signed 32b integer value to string
 * @param val   integer value
 * @param str   converted textual representation
 * @param len   string buffer length
 * @return number of bytes written to str (without '\0')
 */
size_t longToStr(int32_t val, char * str, size_t len) {
    uint32_t x = 1000000000L;
    int_fast8_t digit;
    size_t pos = 0;

    if (val == 0) {
        if (pos < len) str[pos++] = '0';
    } else {
        if (val < 0) {
            val = -val;
            if (pos < len) str[pos++] = '-';
        }

        while ((val / x) == 0) {
            x /= 10;
        }

        do {
            digit = (uint8_t) (val / x);
            if (pos < len) str[pos++] = digit + '0';
            val -= digit * x;
            x /= 10;
        } while (x && (pos < len));
    }

    if (pos < len) str[pos] = 0;
    return pos;
}

/**
 * Converts double value to string
 * @param val   double value
 * @param str   converted textual representation
 * @param len   string buffer length
 * @return number of bytes written to str (without '\0')
 */
size_t doubleToStr(double val, char * str, size_t len) {
    return snprintf(str, len, "%lg", val);
}

/**
 * Converts string to signed 32bit integer representation
 * @param str   string value
 * @param val   32bit integer result
 * @return      number of bytes used in string
 */
size_t strToLong(const char * str, int32_t * val) {
    char * endptr;
    *val = strtol(str, &endptr, 0);
    return endptr - str;
}

/**
 * Converts string to double representation
 * @param str   string value
 * @param val   double result
 * @return      number of bytes used in string
 */
size_t strToDouble(const char * str, double * val) {
    char * endptr;
    *val = strtod(str, &endptr);
    return endptr - str;
}

/**
 * Compare two strings with exact length
 * @param str1
 * @param len1
 * @param str2
 * @param len2
 * @return TRUE if len1==len2 and "len" characters of both strings are equal
 */
scpi_bool_t compareStr(const char * str1, size_t len1, const char * str2, size_t len2) {
    if (len1 != len2) {
        return FALSE;
    }

    if (SCPI_strncasecmp(str1, str2, len2) == 0) {
        return TRUE;
    }

    return FALSE;
}

/**
 * Compare two strings, one be longer but may contains only numbers in that section
 * @param str1
 * @param len1
 * @param str2
 * @param len2
 * @return TRUE if strings match
 */
scpi_bool_t compareStrAndNum(const char * str1, size_t len1, const char * str2, size_t len2) {
    scpi_bool_t result = FALSE;
    size_t i;

    if (len2 < len1) {
        return FALSE;
    }

    if (SCPI_strncasecmp(str1, str2, len1) == 0) {
        result = TRUE;
    }

    for (i = len1; i<len2; i++) {
        if (!isdigit(str2[i])) {
            result = FALSE;
            break;
        }
    }

    return result;
}

enum _locate_text_states {
    STATE_FIRST_WHITESPACE,
    STATE_TEXT_QUOTED,
    STATE_TEXT,
    STATE_LAST_WHITESPACE,
    STATE_COMMA,
    STATE_ERROR
};
typedef enum _locate_text_states locate_text_states;

struct _locate_text_nfa {
    locate_text_states state;
    int32_t startIdx;
    int32_t stopIdx;
    size_t i;
};
typedef struct _locate_text_nfa locate_text_nfa;

/**
 * Test locate text state, if it is correct final state
 */
static scpi_bool_t isFinalState(locate_text_states state) {
    return (
        ((state) == STATE_COMMA)
        || ((state) == STATE_LAST_WHITESPACE)
        || ((state) == STATE_TEXT) ||
        ((state) == STATE_FIRST_WHITESPACE)
    );
}

/**
 * Perform locateText automaton to search string pattern
 * @param nfa stores automaton state
 * @param c current char processed
 */
static scpi_bool_t locateTextAutomaton(locate_text_nfa * nfa, unsigned char c) {
    switch(nfa->state) {
        /* first state locating only white spaces */
        case STATE_FIRST_WHITESPACE:
            if(isspace(c)) {
                nfa->startIdx = nfa->stopIdx = nfa->i + 1;
            } else if (c == ',') {
                nfa->state = STATE_COMMA;
            } else if (c == '"') {
                nfa->startIdx = nfa->i + 1;
                nfa->state = STATE_TEXT_QUOTED;
            } else {
                nfa->startIdx = nfa->i;
                nfa->stopIdx = nfa->i + 1;
                nfa->state = STATE_TEXT;
            }
            break;
        /* state locating any text inside "" */
        case STATE_TEXT_QUOTED:
            if(c == '"') {
                nfa->state = STATE_LAST_WHITESPACE;
                nfa->stopIdx = nfa->i;
            }
            break;
        /* locate text ignoring quotes */
        case STATE_TEXT:
            if (c == ',') {
                nfa->state = STATE_COMMA;
            } else if (!isspace(c)) {
                nfa->stopIdx = nfa->i + 1;
            }
            break;
        /* locating text after last quote */
        case STATE_LAST_WHITESPACE:
            if (c == ',') {
                nfa->state = STATE_COMMA;
            } else if (!isspace(c)) {
                nfa->state = STATE_ERROR;
            }
            break;

        default:
            break;
    }

    /* if it is terminating state, break from for loop */
    if ((nfa->state == STATE_COMMA) || (nfa->state == STATE_ERROR)) {
        return FALSE;
    } else {
        return TRUE;
    }
}

/**
 * Locate text in string. Text is separated by two ""
 *   example: "text", next parameter
 *   regexp: ^[ \t\r\n]*"([^"]*)"[ \t\r\n]*,?
 *   regexp: ^[ \t\r\n]*([^,]*)[ \t\r\n]*,?
 * @param str1 string to be searched
 * @param len1 length of string
 * @param str2 result
 * @param len2 length of result
 * @return string str1 contains text and str2 was set
 */
scpi_bool_t locateText(const char * str1, size_t len1, const char ** str2, size_t * len2) {
    locate_text_nfa nfa;
    nfa.state = STATE_FIRST_WHITESPACE;
    nfa.startIdx = 0;
    nfa.stopIdx = 0;

    for (nfa.i = 0; nfa.i < len1; nfa.i++) {
        if(FALSE == locateTextAutomaton(&nfa, str1[nfa.i])) {
            break;
        }
    }

    if (isFinalState(nfa.state)) {

        if (str2) {
            *str2 = &str1[nfa.startIdx];
        }

        if (len2) {
            *len2 = nfa.stopIdx - nfa.startIdx;
        }
        return TRUE;
    }
    return FALSE;
}

/**
 * Perform locateStr automaton to search string pattern
 * @param nfa stores automaton state
 * @param c current char processed
 */
static scpi_bool_t locateStrAutomaton(locate_text_nfa * nfa, unsigned char c) {
    switch(nfa->state) {
        /* first state locating only white spaces */
        case STATE_FIRST_WHITESPACE:
            if(isspace(c)) {
                nfa->startIdx = nfa->stopIdx = nfa->i + 1;
            } else if (c == ',') {
                nfa->state = STATE_COMMA;
            } else {
                nfa->startIdx = nfa->i;
                nfa->stopIdx = nfa->i + 1;
                nfa->state = STATE_TEXT;
            }
            break;
        /* locate text ignoring quotes */
        case STATE_TEXT:
            if (c == ',') {
                nfa->state = STATE_COMMA;
            } else if (!isspace(c)) {
                nfa->stopIdx = nfa->i + 1;
            }
            break;

        default:
            break;            
    }

    /* if it is terminating state, break from for loop */
    if ((nfa->state == STATE_COMMA) || (nfa->state == STATE_ERROR)) {
        return FALSE;
    } else {
        return TRUE;
    }
}

/**
 * Locate string in string.
 *   regexp: ^[ \t\r\n]*([^,]*)[ \t\r\n]*,?
 * @param str1 string to be searched
 * @param len1 length of string
 * @param str2 result
 * @param len2 length of result
 * @return string str1 contains text and str2 was set
 */
scpi_bool_t locateStr(const char * str1, size_t len1, const char ** str2, size_t * len2) {
    locate_text_nfa nfa;
    nfa.state = STATE_FIRST_WHITESPACE;
    nfa.startIdx = 0;
    nfa.stopIdx = 0;


    for (nfa.i = 0; nfa.i < len1; nfa.i++) {
        if(FALSE == locateStrAutomaton(&nfa, str1[nfa.i])) {
            break;
        }
    }

    if (isFinalState(nfa.state)) {

        if (str2) {
            *str2 = &str1[nfa.startIdx];
        }

        if (len2) {
            *len2 = nfa.stopIdx - nfa.startIdx;
        }
        return TRUE;
    }
    return FALSE;
}


/**
 * Count white spaces from the beggining
 * @param cmd - command
 * @param len - max search length
 * @return number of white spaces
 */
size_t skipWhitespace(const char * cmd, size_t len) {
    size_t i;
    for (i = 0; i < len; i++) {
        if (!isspace((unsigned char) cmd[i])) {
            return i;
        }
    }
    return len;
}

/**
 * Pattern is composed from upper case an lower case letters. This function
 * search the first lowercase letter
 * @param pattern
 * @param len - max search length
 * @return position of separator or len
 */
size_t patternSeparatorShortPos(const char * pattern, size_t len) {
    size_t i;
    for (i = 0; (i < len) && pattern[i]; i++) {
        if (islower((unsigned char) pattern[i])) {
            return i;
        }
    }
    return i;
}

/**
 * Find pattern separator position
 * @param pattern
 * @param len - max search length
 * @return position of separator or len
 */
size_t patternSeparatorPos(const char * pattern, size_t len) {

    const char * separator = strnpbrk(pattern, len, "?:[]");
    if (separator == NULL) {
        return len;
    } else {
        return separator - pattern;
    }
}

/**
 * Find command separator position
 * @param cmd - input command
 * @param len - max search length
 * @return position of separator or len
 */
size_t cmdSeparatorPos(const char * cmd, size_t len) {
    const char * separator = strnpbrk(cmd, len, ":?");
    size_t result;
    if (separator == NULL) {
        result = len;
    } else {
        result = separator - cmd;
    }

    return result;
}

/**
 * Match pattern and str. Pattern is in format UPPERCASElowercase
 * @param pattern
 * @param pattern_len
 * @param str
 * @param str_len
 * @return 
 */
scpi_bool_t matchPattern(const char * pattern, size_t pattern_len, const char * str, size_t str_len) {
    int pattern_sep_pos_short;

    if (pattern[pattern_len - 1] == '#') {
        size_t new_pattern_len = pattern_len - 1;

        pattern_sep_pos_short = patternSeparatorShortPos(pattern, new_pattern_len);

        return compareStrAndNum(pattern, new_pattern_len, str, str_len) ||
                compareStrAndNum(pattern, pattern_sep_pos_short, str, str_len);
    } else {

        pattern_sep_pos_short = patternSeparatorShortPos(pattern, pattern_len);

        return compareStr(pattern, pattern_len, str, str_len) ||
                compareStr(pattern, pattern_sep_pos_short, str, str_len);
    }
}

/**
 * Compare pattern and command
 * @param pattern eg. [:MEASure]:VOLTage:DC?
 * @param cmd - command
 * @param len - max search length
 * @return TRUE if pattern matches, FALSE otherwise
 */
scpi_bool_t matchCommand(const char * pattern, const char * cmd, size_t len) {
    scpi_bool_t result = FALSE;
    int leftFlag = 0; // flag for '[' on left
    int rightFlag = 0; // flag for ']' on right
    int cmd_sep_pos = 0;

    const char * pattern_ptr = pattern;
    int pattern_len = strlen(pattern);
    const char * pattern_end = pattern + pattern_len;

    const char * cmd_ptr = cmd;
    size_t cmd_len = SCPI_strnlen(cmd, len);
    const char * cmd_end = cmd + cmd_len;

    /* now support optional keywords in pattern style, e.g. [:MEASure]:VOLTage:DC? */
    if (pattern_ptr[0] == '[') { // skip first '['
        pattern_len--;
        pattern_ptr++;
        leftFlag++;
    }
    if (pattern_ptr[0] == ':') { // skip first ':'
        pattern_len--;
        pattern_ptr++;
    }

    if (cmd_ptr[0] == ':') {
        /* handle errornouse ":*IDN?" */
        if((cmd_len >= 2) && (cmd_ptr[1] != '*')) {
            cmd_len--;
            cmd_ptr++;
        }
    }

    while (1) {
        int pattern_sep_pos = patternSeparatorPos(pattern_ptr, pattern_end - pattern_ptr);

        if ((leftFlag > 0) && (rightFlag > 0)) {
            leftFlag--;
            rightFlag--;
        } else {
            cmd_sep_pos = cmdSeparatorPos(cmd_ptr, cmd_end - cmd_ptr);
        }

        if (matchPattern(pattern_ptr, pattern_sep_pos, cmd_ptr, cmd_sep_pos)) {
            pattern_ptr = pattern_ptr + pattern_sep_pos;
            cmd_ptr = cmd_ptr + cmd_sep_pos;
            result = TRUE;

            /* command is complete */
            if ((pattern_ptr == pattern_end) && (cmd_ptr >= cmd_end)) {
                break;
            }

            /* pattern complete, but command not */
            if ((pattern_ptr == pattern_end) && (cmd_ptr < cmd_end)) {
                result = FALSE;
                break;
            }

            /* command complete, but pattern not */
            if (cmd_ptr >= cmd_end) {
                if (cmd_end == cmd_ptr) {
                    if (cmd_ptr[0] == pattern_ptr[pattern_end - pattern_ptr - 1]) {
                        break; /* exist optional keyword, command is complete */
                    }
                    if (']' == pattern_ptr[pattern_end - pattern_ptr - 1]) {
                        break; /* exist optional keyword, command is complete */
                    }
                }
                result = FALSE;
                break;
            }

            /* both command and patter contains command separator at this position */
            if ((pattern_ptr[0] == cmd_ptr[0]) && ((pattern_ptr[0] == ':') || (pattern_ptr[0] == '?'))) {
                pattern_ptr = pattern_ptr + 1;
                cmd_ptr = cmd_ptr + 1;
            } else if ((pattern_ptr[1] == cmd_ptr[0])
                    && (pattern_ptr[0] == '[')
                    && (pattern_ptr[1] == ':')) {
                pattern_ptr = pattern_ptr + 2; // for skip '[' in "[:"
                cmd_ptr = cmd_ptr + 1;
                leftFlag++;
            } else if ((pattern_ptr[1] == cmd_ptr[0])
                    && (pattern_ptr[0] == ']')
                    && (pattern_ptr[1] == ':')) {
                pattern_ptr = pattern_ptr + 2; // for skip ']' in "]:"
                cmd_ptr = cmd_ptr + 1;
            } else if ((pattern_ptr[2] == cmd_ptr[0])
                    && (pattern_ptr[0] == ']')
                    && (pattern_ptr[1] == '[')
                    && (pattern_ptr[2] == ':')) {
                pattern_ptr = pattern_ptr + 3; // for skip '][' in "][:"
                cmd_ptr = cmd_ptr + 1;
                leftFlag++;
            } else if (((pattern_ptr[0] == ']')
                    || (pattern_ptr[0] == '['))
                    && (*(pattern_end - 1) == '?') // last is '?'
                    && (cmd_ptr[0] == '?')) {
                result = TRUE; // exist optional keyword, and they are end with '?'
                break; // command is complete  OK
            } else {
                result = FALSE;
                break;
            }
        } else {
            pattern_ptr = pattern_ptr + pattern_sep_pos;
            if ((pattern_ptr[0] == ']') && (pattern_ptr[1] == ':')) {
                pattern_ptr = pattern_ptr + 2; // for skip ']' in "]:" , pattern_ptr continue, while cmd_ptr remain unchanged
                rightFlag++;
            } else if ((pattern_ptr[0] == ']')
                    && (pattern_ptr[1] == '[')
                    && (pattern_ptr[2] == ':')) {
                pattern_ptr = pattern_ptr + 3; // for skip ']' in "][:" , pattern_ptr continue, while cmd_ptr remain unchanged
                rightFlag++;
            } else {
                result = FALSE;
                break;
            }
        }
    }

    return result;
}

/**
 * Compose command from previsou command anc current command
 * 
 * @param ptr_prev pointer to previous command
 * @param len_prev length of previous command
 * @param pptr pointer to pointer of current command
 * @param plen pointer to length of current command
 * 
 * ptr_prev and ptr should be in the same memory buffer
 * 
 * Function will add part of previous command prior to ptr_prev
 * 
 * char * cmd = "meas:volt:dc?;ac?"
 * char * ptr_prev = cmd;
 * size_t len_prev = 13;
 * char * ptr = cmd + 14;
 * size_t len = 3;
 * 
 * composeCompoundCommand(ptr_prev, len_prev, &ptr, &len);
 * 
 * after calling this
 * 
 * 
 * 
 */
scpi_bool_t composeCompoundCommand(char * ptr_prev, size_t len_prev,
                              char ** pptr, size_t * plen) {
    char * ptr;
    size_t len;
    size_t i;

    /* Invalid input */
    if (pptr == NULL || plen == NULL)
        return FALSE;

    /* no previous command - nothing to do*/
    if (ptr_prev == NULL || len_prev == 0)
        return TRUE;
       
    ptr = *pptr;
    len = *plen;
    
    /* No current command */
    if (len == 0 || ptr == NULL)
        return FALSE;
    
    /* Common command or command root - nothing to do */
    if (ptr[0] == '*' || ptr[0] == ':')
        return TRUE;
        
    /* Previsou command was common command - nothing to do */
    if (ptr_prev[0] == '*')
        return TRUE;
        
    /* Find last occurence of ':' */
    for (i = len_prev; i > 0; i--) {
        if (ptr_prev[i-1] == ':') {
            break;
        }
    }
    
    /* Previous command was simple command - nothing to do*/
    if (i == 0)
        return TRUE;
    
    ptr -= i;
    len += i;
    memmove(ptr, ptr_prev, i);
    *plen = len;
    *pptr = ptr;
    return TRUE;
}



#if !HAVE_STRNLEN
/* use FreeBSD strnlen */

/*-
 * Copyright (c) 2009 David Schultz <das@FreeBSD.org>
 * All rights reserved.
 */
size_t
BSD_strnlen(const char *s, size_t maxlen) {
    size_t len;

    for (len = 0; len < maxlen; len++, s++) {
        if (!*s)
            break;
    }
    return (len);
}
#endif

#if !HAVE_STRNCASECMP && !HAVE_STRNICMP
int OUR_strncasecmp(const char *s1, const char *s2, size_t n) {
    unsigned char c1, c2;

    for(; n != 0; n--) {
        c1 = tolower((unsigned char)*s1++);
        c2 = tolower((unsigned char)*s2++);
        if (c1 != c2) {
            return c1 - c2;
        }
        if (c1 = '\0') {
            return 0;
        }
    }
    return 0;
}
#endif

