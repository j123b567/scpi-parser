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
 * @file   scpi_parser.h
 * @date   Thu Nov 15 10:58:45 UTC 2012
 * 
 * @brief  SCPI parser implementation
 * 
 * 
 */

#ifndef SCPI_PARSER_H
#define	SCPI_PARSER_H

#include <string.h>
#include "scpi/types.h"
#include "scpi/debug.h"

#ifdef	__cplusplus
extern "C" {
#endif
    void SCPI_Init(scpi_t * context);

    int SCPI_Input(scpi_t * context, const char * data, int len);
    int SCPI_Parse(scpi_t * context, const char * data, int len);


    size_t SCPI_ResultCharacters(scpi_t * context, const char * data, size_t len);
#define SCPI_ResultMnemonic(context, data) SCPI_ResultCharacters((context), (data), strlen(data))
    size_t SCPI_ResultInt(scpi_t * context, int32_t val);
    size_t SCPI_ResultIntBase(scpi_t * context, int32_t val, int8_t base);
    size_t SCPI_ResultDouble(scpi_t * context, double val);
    size_t SCPI_ResultText(scpi_t * context, const char * data);
    size_t SCPI_ResultArbitraryBlock(scpi_t * context, const char * data, size_t len);
    size_t SCPI_ResultBool(scpi_t * context, bool_t val);

    int32_t SCPI_ParamGetIntVal(scpi_t * context, scpi_parameter_t * parameter);
    double SCPI_ParamGetDoubleVal(scpi_t * context, scpi_parameter_t * parameter);
    void SCPI_ParamGetTextVal(scpi_t * context, scpi_parameter_t * parameter, const char ** data, int32_t * len);
#define SCPI_ParamGetCharactersVal SCPI_ParamGetTextVal
#define SCPI_ParamGetArbitraryBlockVal SCPI_ParamGetTextVal
    bool_t SCPI_ParamGetBoolVal(scpi_t * context, scpi_parameter_t * parameter);
    int32_t SCPI_ParamGetChoiceVal(scpi_t * context, scpi_parameter_t * parameter, const char * options[]);

    bool_t SCPI_Parameter(scpi_t * context, scpi_parameter_t * parameter, bool_t mandatory);

#ifdef	__cplusplus
}
#endif

#endif	/* SCPI_PARSER_H */

