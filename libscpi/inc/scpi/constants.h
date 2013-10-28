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
 * @file   scpi_constants.h
 * @date   Thu Nov 15 10:58:45 UTC 2012
 * 
 * @brief  SCPI Device constants
 * 
 * 
 */

#ifndef SCPI_CONSTANTS_H
#define	SCPI_CONSTANTS_H

#ifdef	__cplusplus
extern "C" {
#endif


/*  4.1.3.6 *IDN? */

#define SCPI_DEFAULT_1_MANUFACTURE "CTU FEE"
#define SCPI_DEFAULT_2_MODEL "TSI3225"
#define SCPI_DEFAULT_3 "0"
#define SCPI_DEFAULT_4_REVISION "01-01"

/* 21.21 :VERSion? 
 * YYYY.V
 * YYYY = SCPI year
 * V = SCPI revision
 */
#define SCPI_STD_VERSION_REVISION "1999.0"

#ifdef	__cplusplus
}
#endif

#endif	/* SCPI_CONSTANTS_H */

