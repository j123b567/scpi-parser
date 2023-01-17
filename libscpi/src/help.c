/*-
 * BSD 2-Clause License
 *
 * Copyright (c) 2023, Helge Wurst
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 
 /**
 * @file   help.c
 * @date   Tue Jan 17 16:27:00 UTC 2023
 *
 * @brief  HELP? ["<search string>"] command handler
 *
 *
 */

#include "scpi/parser.h"
#include "scpi/help.h"
#include "scpi/constants.h"
#include "scpi/error.h"
#include "scpi/ieee488.h"
#include "utils_private.h"

/**
 * HELP? ["<string>"]
 * @param context
 * @return
 */
scpi_result_t SCPI_HelpQ(scpi_t * context) {
#if USE_HELP_FILTER
    size_t search_string_len = 0;
    const char * search_string = NULL;
    scpi_bool_t narrowed_down = SCPI_ParamCharacters(context, &search_string, &search_string_len, FALSE);
#endif
    size_t i;
    for(i = 0; context->cmdlist[i].pattern != NULL; i++) {
        size_t pattern_len = strlen(context->cmdlist[i].pattern);
#if USE_HELP_FILTER
        if(narrowed_down && (NULL == strncasestrn(context->cmdlist[i].pattern, pattern_len, search_string, search_string_len))){
	        continue;
        }
#endif
        size_t block_len = 1 + pattern_len + strlen(SCPI_LINE_ENDING);
#if USE_COMMAND_DESCRIPTIONS
        size_t description_len = context->cmdlist[i].description ? strlen(context->cmdlist[i].description) : 0;
        if(description_len > 0){
            block_len = 1 + pattern_len + 1 + description_len + strlen(SCPI_LINE_ENDING);
        }
#endif
        SCPI_ResultArbitraryBlockHeader(context, block_len);
        SCPI_ResultArbitraryBlockData(context, "\t", 1);
        SCPI_ResultArbitraryBlockData(context, context->cmdlist[i].pattern, pattern_len);
#if USE_COMMAND_DESCRIPTIONS
        if(description_len > 0){
            SCPI_ResultArbitraryBlockData(context, " ", 1);
            SCPI_ResultArbitraryBlockData(context, context->cmdlist[i].description, description_len);
        }
#endif
        SCPI_ResultArbitraryBlockData(context, SCPI_LINE_ENDING, strlen(SCPI_LINE_ENDING));
    }
    return SCPI_RES_OK;
}
