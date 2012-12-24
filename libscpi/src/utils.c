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

    if (strncasecmp(str1, str2, len2) == 0) {
        return TRUE;
    }

    return FALSE;
}

bool_t locateText(const char * str1, size_t len1, char ** str2, size_t * len2) {
    size_t i;
    int quot = 0;
    int32_t strStart = -1;
    int32_t strStop = -1;
    int valid = 0;


    for (i = 0; i < len1; i++) {
        if ((strStart < 0) && isspace(str1[i])) {
            continue;
        }

        if ((strStart < 0) && !quot && (str1[i] == '"')) {
            quot = 1;
            continue;
        }

        if (strStart < 0) {
            strStart = i;
        }

        if ((strStop < 0) && quot && (str1[i] == '"')) {
            strStop = i;
            valid = 1;
            continue;
        }

        if ((strStop >= 0) && quot && (str1[i] == ',')) {
            break;
        }

        if ((strStop >= 0) && quot && !isspace(str1[i])) {
            valid = 0;
        }

        if (!quot && !isspace(str1[i]) && (str1[i] != ',')) {
            strStop = i;
        }

        if (isspace(str1[i])) {
            continue;
        }

        if ((strStop >= 0) && (str1[i] == ',')) {
            valid = 1;
            break;
        }
    }

    if ((i == len1) && !quot) {
        valid = 1;
        if (strStop < 0) {
            strStop = i;
        } else {
            strStop++;
        }
        if (strStart < 0) {
            strStart = i;
        }
    } else if (!quot) {
        strStop++;
    }


    if (valid) {
        if (str2) {
            *str2 = (char *) &str1[strStart];
        }

        if (len2) {
            *len2 = strStop - strStart;
        }
    }

    return valid;
}

bool_t locateStr(const char * str1, size_t len1, char ** str2, size_t * len2) {
    size_t i;
    int32_t strStart = -1;
    int32_t strStop = -1;
    int valid = 0;


    for (i = 0; i < len1; i++) {
        if ((strStart < 0) && isspace(str1[i])) {
            continue;
        }

        if (strStart < 0) {
            strStart = i;
        }

        if (!isspace(str1[i]) && (str1[i] != ',')) {
            strStop = i;
        }

        if (isspace(str1[i])) {
            continue;
        }

        if (str1[i] == ',') {
            valid = 1;

            if (strStop < 0) {
                strStop = i;
            }
            break;
        }
    }

    if (i == len1) {
        valid = 1;
        if (strStop < 0) {
            strStop = i;
        } else {
            strStop++;
        }
        if (strStart < 0) {
            strStart = i;
        }
    } else {
        strStop++;
    }


    if (valid) {
        if (str2) {
            *str2 = (char *) &str1[strStart];
        }

        if (len2) {
            *len2 = strStop - strStart;
        }
    }

    return valid;
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
        if (!isspace(cmd[i])) {
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
        if (islower(pattern[i])) {
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
    return compareStr(pattern, pattern_len, str, str_len) ||
            compareStr(pattern, pattern_sep_pos_short, str, str_len);
}
