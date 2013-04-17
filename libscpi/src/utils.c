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

#include "utils.h"

static size_t patternSeparatorShortPos(const char * pattern, size_t len);

/**
 * Find the first occurrence in str of a character in set.
 * @param str
 * @param size
 * @param set
 * @return 
 */
char * strnpbrk(const char *str, size_t size, const char *set) {
    const char *scanp;
    long c, sc;
    const char * strend = str + size;

    while ((strend != str) && ((c = *str++) != 0)) {
        for (scanp = set; (sc = *scanp++) != '\0';)
            if (sc == c)
                return ((char *) (str - 1));
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
bool_t compareStr(const char * str1, size_t len1, const char * str2, size_t len2) {
    if (len1 != len2) {
        return FALSE;
    }

    if (SCPI_strncasecmp(str1, str2, len2) == 0) {
        return TRUE;
    }

    return FALSE;
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
static bool_t isFinalState(locate_text_states state) {
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
static bool_t locateTextAutomaton(locate_text_nfa * nfa, unsigned char c) {
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
bool_t locateText(const char * str1, size_t len1, const char ** str2, size_t * len2) {
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
static bool_t locateStrAutomaton(locate_text_nfa * nfa, unsigned char c) {
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
bool_t locateStr(const char * str1, size_t len1, const char ** str2, size_t * len2) {
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
        if (!isspace((unsigned char)cmd[i])) {
            return i;
        }
    }
    return len;
}

/**
 * is colon or not.  add by hmm 2013.4.1
 * @param cmd - command
 * @return
 */
static bool_t iscolon(const char * cmd) {
    char* pColon = ":";
    if(0 == SCPI_strncasecmp(cmd, pColon, 1))
	{
	    return TRUE;
	}
    return FALSE;
}

/**
 * Count colon from the beggining  add by hmm 2013.4.1
 * @param cmd - command
 * @param len - max search length
 * @return number of colon
 */
size_t skipColon(const char * cmd, size_t len) {
    size_t i;
    for (i = 0; i < len; i++) {
        if (!iscolon(&cmd[i])) {
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
        if (islower((unsigned char)pattern[i])) {
            return i;
        }
    }
    return i;
}

/**
 * Match pattern and str. Pattern is in format UPPERCASElowercase
 * @param pattern
 * @param pattern_len
 * @param str
 * @param str_len
 * @return 
 */
bool_t matchPattern(const char * pattern, size_t pattern_len, const char * str, size_t str_len) {
    int pattern_sep_pos_short = patternSeparatorShortPos(pattern, pattern_len);

    //return compareStr(pattern, pattern_len, str, str_len) ||
    return compareStr(pattern, str_len, str, str_len) ||   // edit by hmm 2013.4.3
            compareStr(pattern, pattern_sep_pos_short, str, str_len);
}


#if !HAVE_STRNLEN
/* use FreeBSD strnlen */

/*-
 * Copyright (c) 2009 David Schultz <das@FreeBSD.org>
 * All rights reserved.
 */
size_t
BSD_strnlen(const char *s, size_t maxlen)
{
	size_t len;
	
	for (len = 0; len < maxlen; len++, s++) {
		if (!*s)
			break;
	}
	return (len);
}
#endif

