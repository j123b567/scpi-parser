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

#include "utils_private.h"
#include "scpi/utils.h"

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
 * @param base  output base
 * @return number of bytes written to str (without '\0')
 */
size_t SCPI_LongToStr(int32_t val, char * str, size_t len, int8_t base) {
    const char digits[] = "0123456789ABCDEF";

#define ADD_CHAR(c) if (pos < len) str[pos++] = (c)
    uint32_t x = 0;
    int_fast8_t digit;
    size_t pos = 0;
    uint32_t uval = val;

    if (uval == 0) {
        ADD_CHAR('0');
    } else {

        switch (base) {
            case 2: 
                x = 0x80000000L;
                break;
            case 8:
                x = 0x40000000L;
                break;
            case 10:
                x = 1000000000L;
                break;
            case 0x10:
                x = 0x10000000L;
                break;
            default:
                x = 1000000000L;
                base = 10;
                break;
        }

        // add sign for numbers in base 10
        if ((val < 0) && (base == 10)) {
            uval = -val;
            ADD_CHAR('-');
        }

        // remove leading zeros
        while ((uval / x) == 0) {
            x /= base;
        }

        do {
            digit = (uint8_t) (uval / x);
            ADD_CHAR(digits[digit]);
            uval -= digit * x;
            x /= base;
        } while (x && (pos < len));
    }

    if (pos < len) str[pos] = 0;
    return pos;
#undef ADD_CHAR
}

/**
 * Converts double value to string
 * @param val   double value
 * @param str   converted textual representation
 * @param len   string buffer length
 * @return number of bytes written to str (without '\0')
 */
size_t SCPI_DoubleToStr(double val, char * str, size_t len) {
    return SCPIDEFINE_doubleToStr(val, str, len);
}

/**
 * Converts string to signed 32bit integer representation
 * @param str   string value
 * @param val   32bit integer result
 * @return      number of bytes used in string
 */
size_t strToLong(const char * str, int32_t * val, int8_t base) {
    char * endptr;
    *val = strtol(str, &endptr, base);
    return endptr - str;
}

/**
 * Converts string to unsigned 32bit integer representation
 * @param str   string value
 * @param val   32bit integer result
 * @return      number of bytes used in string
 */
size_t strToULong(const char * str, uint32_t * val, int8_t base) {
    char * endptr;
    *val = strtoul(str, &endptr, base);
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

    if (SCPIDEFINE_strncasecmp(str1, str2, len2) == 0) {
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
scpi_bool_t compareStrAndNum(const char * str1, size_t len1, const char * str2, size_t len2, int32_t * num) {
    scpi_bool_t result = FALSE;
    size_t i;

    if (len2 < len1) {
        return FALSE;
    }

    if (SCPIDEFINE_strncasecmp(str1, str2, len1) == 0) {
        result = TRUE;

        if (num) {
            if (len1 == len2) {
                *num = 1;
            } else {
                int32_t tmpNum;
                i = len1 + strToLong(str2 + len1, &tmpNum, 10);
                if (i != len2) {
                    result = FALSE;
                } else {
                    *num = tmpNum;
                }
            }
        } else {
            for (i = len1; i<len2; i++) {
                if (!isdigit((int) str2[i])) {
                    result = FALSE;
                    break;
                }
            }
        }
    }

    return result;
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
static size_t patternSeparatorShortPos(const char * pattern, size_t len) {
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
static size_t patternSeparatorPos(const char * pattern, size_t len) {

    char * separator = strnpbrk(pattern, len, "?:[]");
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
static size_t cmdSeparatorPos(const char * cmd, size_t len) {
    char * separator = strnpbrk(cmd, len, ":?");
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
scpi_bool_t matchPattern(const char * pattern, size_t pattern_len, const char * str, size_t str_len, int32_t * num) {
    int pattern_sep_pos_short;

    if (pattern[pattern_len - 1] == '#') {
        size_t new_pattern_len = pattern_len - 1;

        pattern_sep_pos_short = patternSeparatorShortPos(pattern, new_pattern_len);

        return compareStrAndNum(pattern, new_pattern_len, str, str_len, num) ||
                compareStrAndNum(pattern, pattern_sep_pos_short, str, str_len, num);
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
scpi_bool_t matchCommand(const char * pattern, const char * cmd, size_t len, int32_t *numbers, size_t numbers_len) {
    scpi_bool_t result = FALSE;
    int leftFlag = 0; // flag for '[' on left
    int rightFlag = 0; // flag for ']' on right
    int cmd_sep_pos = 0;

    size_t numbers_idx = 0;
    int32_t *number_ptr = NULL;

    const char * pattern_ptr = pattern;
    int pattern_len = strlen(pattern);
    const char * pattern_end = pattern + pattern_len;

    const char * cmd_ptr = cmd;
    size_t cmd_len = SCPIDEFINE_strnlen(cmd, len);
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
        if ((cmd_len >= 2) && (cmd_ptr[1] != '*')) {
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

        if (pattern_ptr[pattern_sep_pos - 1] == '#') {
            if (numbers && (numbers_idx < numbers_len)) {
                number_ptr = numbers + numbers_idx;
                *number_ptr = 1; // default value
            } else {
                number_ptr = NULL;
            }
            numbers_idx++;
        } else {
            number_ptr = NULL;
        }

        if (matchPattern(pattern_ptr, pattern_sep_pos, cmd_ptr, cmd_sep_pos, number_ptr)) {
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
 * @param prev pointer to previous command
 * @param current pointer of current command
 *
 * prev and current should be in the same memory buffer
 */
scpi_bool_t composeCompoundCommand(const scpi_token_t * prev, scpi_token_t * current) {
    size_t i;

    /* Invalid input */
    if (current == NULL || current->ptr == NULL || current->len == 0)
        return FALSE;

    /* no previous command - nothing to do*/
    if (prev->ptr == NULL || prev->len == 0)
        return TRUE;

    /* Common command or command root - nothing to do */
    if (current->ptr[0] == '*' || current->ptr[0] == ':')
        return TRUE;

    /* Previsou command was common command - nothing to do */
    if (prev->ptr[0] == '*')
        return TRUE;

    /* Find last occurence of ':' */
    for (i = prev->len; i > 0; i--) {
        if (prev->ptr[i - 1] == ':') {
            break;
        }
    }

    /* Previous command was simple command - nothing to do*/
    if (i == 0)
        return TRUE;

    current->ptr -= i;
    current->len += i;
    memmove(current->ptr, prev->ptr, i);
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

    for (; n != 0; n--) {
        c1 = tolower((unsigned char) *s1++);
        c2 = tolower((unsigned char) *s2++);
        if (c1 != c2) {
            return c1 - c2;
        }
        if (c1 == '\0') {
            return 0;
        }
    }
    return 0;
}
#endif

