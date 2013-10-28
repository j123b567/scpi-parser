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
 * @file   config.h
 * @date   Wed Mar 20 12:21:26 UTC 2013
 * 
 * @brief  SCPI Configuration
 * 
 * 
 */

#ifndef __SCPI_CONFIG_H_
#define __SCPI_CONFIG_H_

#ifdef	__cplusplus
extern "C" {
#endif

/* Compiler specific */
/* 8bit PIC - PIC16, etc */
#if defined(_MPC_)
#define HAVE_STRNLEN            0
#define HAVE_STRNCASECMP        0
#define HAVE_STRNICMP           1
#endif

/* PIC24 */
#if defined(__C30__)
#define HAVE_STRNLEN            0
#define HAVE_STRNCASECMP        0
#define HAVE_STRNICMP           0
#endif

/* PIC32mx */
#if defined(__C32__)
#define HAVE_STRNLEN            0
#define HAVE_STRNCASECMP        1
#define HAVE_STRNICMP           0
#endif

/* ======== test strnlen ======== */
#ifndef HAVE_STRNLEN
#define HAVE_STRNLEN            1
#endif
/* ======== test strncasecmp ======== */
#ifndef HAVE_STRNCASECMP
#define HAVE_STRNCASECMP        1
#endif
/* ======== test strnicmp ======== */
#ifndef HAVE_STRNICMP
#define HAVE_STRNICMP           0
#endif

/* define local macros depending on existance of strnlen */
#if HAVE_STRNLEN
#define SCPI_strnlen(s, l)	strnlen((s), (l))
#else
#define SCPI_strnlen(s, l)	BSD_strnlen((s), (l))
#endif

/* define local macros depending on existance of strncasecmp and strnicmp */
#if HAVE_STRNCASECMP
#define SCPI_strncasecmp(s1, s2, l)	strncasecmp((s1), (s2), (l))
#elif HAVE_STRNICMP
#define SCPI_strncasecmp(s1, s2, l)     strnicmp((s1), (s2), (l))
#else
#define SCPI_strncasecmp(s1, s2, l)	OUR_strncasecmp((s1), (s2), (l))
#endif

#ifdef	__cplusplus
}
#endif

#endif
