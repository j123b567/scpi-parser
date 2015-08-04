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
 * @file   scpi_debug.c
 * @date   Thu Nov 15 10:58:45 UTC 2012
 * 
 * @brief  Debugging SCPI
 * 
 * 
 */

#include <stdio.h>
#include <stdint.h>
#include "debug.h"            /* this includes platform specific printing capabilities */
#include "scpi/scpi_debug.h"

/**
 * Debug function: show current command and its parameters
 * @param context
 * @return 
 */
scpi_bool_t SCPI_DebugCommand(scpi_t * context)
{
/*	size_t res;
	printf("**DEBUG: %s (\"", context->param_list.cmd->pattern);
	res =	fwrite(context->param_list.lex_state.buffer, 1, context->param_list.lex_state.len, stdout);
	(void)res;
        printf("\" - %lu\r\n", (unsigned long)context->param_list.lex_state.len);
*/

   /* The PRINT_DEBUG and PRINT_BUFFER macros are defined in debug.h and are platform specific */
	PRINT_DEBUG("SCPI: %s (\"", context->param_list.cmd->pattern);
	PRINT_BUFFER((uint8_t *)context->param_list.lex_state.buffer, context->param_list.lex_state.len);
	PRINT_DEBUG("\" - %lu\r\n", (unsigned long)context->param_list.lex_state.len);
	
	return TRUE;
}
