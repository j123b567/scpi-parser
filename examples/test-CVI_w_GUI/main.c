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
 * @file   main.c
 * @date   Thu Nov 15 10:58:45 UTC 2012
 * 
 * @brief  SCPI parser test
 * 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scpi/scpi.h"
#include "../common/scpi-def.h"

#include <cvirte.h>     
#include <userint.h>
#include "TestLibscpi.h"

static int panelHandle;

#define SCPI_OUPUT_BUFFER_SIZE      (256)
char SCPI_outputBuffer[SCPI_OUPUT_BUFFER_SIZE];
unsigned int SCPI_outputBuffer_idx = 0;

void removeTrailingEndcodes(char *buf)
{
   int len;
   len = strlen(buf);
   
   while(len > 0)
   {
      len--;
      switch(buf[len])
      {
         case '\n'   :
         case '\r'   :
                        buf[len] = '\0';
                        break;
         default     :
                        len = 0;    // stop loop
                        break;
      }
   }
   
}

void debug_output(char *buf)
{
   static char pbuf[512];
   static int pos = 0;
   int len;
   
   len = strlen(buf);
   if(buf[len-1] == '\n')
   {
      buf[len-1] == '\0';
      len--;
      memcpy(&pbuf[pos], buf, len);
      pos = 0;
      InsertTextBoxLine (panelHandle, PANEL_OUTPUTDEBUG, -1, pbuf);
   }
   else
   {
      memcpy(&pbuf[pos], buf, len);
      pos += len;
   }
   
}

size_t SCPI_Write(scpi_t * context, const char * data, size_t len)
{
   if((SCPI_outputBuffer_idx + len) > SCPI_OUPUT_BUFFER_SIZE)
   {
      len = SCPI_OUPUT_BUFFER_SIZE - SCPI_outputBuffer_idx;    // limit length to left over space
      // apparently there is no mechanism to cope with buffers that are too small
   }
   memcpy(&SCPI_outputBuffer[SCPI_outputBuffer_idx], data, len);
   SCPI_outputBuffer_idx += len;
   
   SCPI_outputBuffer[SCPI_outputBuffer_idx] = '\0';
   // return fwrite(data, 1, len, stdout);
   return len;
}

scpi_result_t SCPI_Flush(scpi_t * context)
{    
// fwrite(SCPI_outputBuffer, 1, SCPI_outputBuffer_idx, stdout);
   removeTrailingEndcodes(SCPI_outputBuffer);
   InsertTextBoxLine (panelHandle, PANEL_OUTPUT, -1, SCPI_outputBuffer);
   SCPI_outputBuffer_idx = 0;
   return SCPI_RES_OK;
}

int SCPI_Error(scpi_t * context, int_fast16_t err)
{
   char buf[512];
   
//  fprintf(stderr, "**ERROR: %d, \"%s\"\r\n", (int16_t) err, SCPI_ErrorTranslate(err));
   sprintf(buf, "**ERROR: %d, \"%s\"", (int16_t) err, SCPI_ErrorTranslate(err));
   InsertTextBoxLine (panelHandle, PANEL_OUTPUTERR, -1, buf);
    return 0;
}

scpi_result_t SCPI_Control(scpi_t * context, scpi_ctrl_name_t ctrl, scpi_reg_val_t val) {
    if (SCPI_CTRL_SRQ == ctrl) {
        fprintf(stderr, "**SRQ: 0x%X (%d)\r\n", val, val);
    } else {
        fprintf(stderr, "**CTRL %02x: 0x%X (%d)\r\n", ctrl, val, val);
    }
    return SCPI_RES_OK;
}

scpi_result_t SCPI_Reset(scpi_t * context) {
   char buf[256];
//  fprintf(stderr, "**Reset\r\n");
   sprintf(buf, "**Reset\r\n");
   debug_output(buf);
    return SCPI_RES_OK;
}

scpi_result_t SCPI_SystemCommTcpipControlQ(scpi_t * context) {
    return SCPI_RES_ERR;
}
/*
 * 
 */
#define MYBUFSIZE    256 
int main(int argc, char** argv)
{
   char smbuffer[MYBUFSIZE];
   int result;

   SCPI_Init(&scpi_context);

   if (InitCVIRTE (0, argv, 0) == 0)
      return -1;  /* out of memory */
   if ((panelHandle = LoadPanel (0, "TestLibscpi.uir", PANEL)) < 0)
      return -1;
   DisplayPanel (panelHandle);

   RunUserInterface ();
   DiscardPanel (panelHandle);

/* printf("SCPI Interactive demo\r\nPress <CTRL>+C to exit\n");
   while (1)
   {
      if (NULL == fgets(smbuffer, MYBUFSIZE, stdin))
      {
         break;
      }
      SCPI_Input(&scpi_context, smbuffer, strlen(smbuffer));
   }*/

   return (EXIT_SUCCESS);
}

int CVICALLBACK cb_scpi_input (int panel, int control, int event,
      void *callbackData, int eventData1, int eventData2)
{
   char buf[256];
   int len;
   
   switch (event)
   {
      case EVENT_COMMIT:
         GetCtrlVal(panel, control, buf);
         len = strlen(buf);
         buf[len] = '\n';
         len++;
         SCPI_Input(&scpi_context, buf, len);
         
         break;
   }
   return 0;
}

int CVICALLBACK cb_quit (int panel, int control, int event,
      void *callbackData, int eventData1, int eventData2)
{
   switch (event)
   {
      case EVENT_COMMIT:
         QuitUserInterface (0);
         break;
   }
   return 0;
}
