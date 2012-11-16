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

#include "scpi.h"
#include "scpi_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    return snprintf(str, len, "%lf", val);
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