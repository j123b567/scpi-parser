/*
 * Copyright (c) 2012-2013 Jan Breuer, Richard.hmm
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
 * @brief SCPI parser test by hmm
 * @details
 * @version V1.0.0
 * @author Richard.hmm
 * @date 2013-4-14
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scpi/scpi.h"
#include "scpi-def-hmm.h"

size_t SCPI_Write(scpi_t * context, const char * data, size_t len) {
    (void) context;
    return fwrite(data, 1, len, stdout);
}

scpi_result_t SCPI_Flush(scpi_t * context) {    
    return SCPI_RES_OK;
}

int SCPI_Error(scpi_t * context, int_fast16_t err) {
    (void) context;

    fprintf(stderr, "**ERROR: %d, \"%s\"\r\n", (int32_t) err, SCPI_ErrorTranslate(err));
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

scpi_result_t SCPI_Test(scpi_t * context) {
    fprintf(stderr, "**Test\r\n");
    return SCPI_RES_OK;
}

scpi_result_t SCPI_Reset(scpi_t * context) {
    fprintf(stderr, "**Reset\r\n");
    return SCPI_RES_OK;
}

scpi_result_t SCPI_SystemCommTcpipControlQ(scpi_t * context) {
    return SCPI_RES_ERR;
}

/**
 * Interface to the application. Adds data to system buffer and try to search
 * command line termination. If the termination is found or if len=0, command
 * parser is called.
 * 
 * @param context
 * @param data - data to process
 * @param len - length of data
 * @return 
 */
int SCPI_Input_Dbg(scpi_t * context, const char * data, size_t len)
{
    char temp1[100];

	strcpy(temp1, data);
	printf("receive SCPI CMD: %s", temp1); // HMM

    int result = SCPI_Input(context, data, len);
    printf("-------------------finish--------------------------------\n\n"); // HMM

    return result;
}


/*
 * 
 */
int main(int argc, char** argv) {
    (void) argc;
    (void) argv;
    int result;

    SCPI_Init(&scpi_context);

#if 1 // no interactive demo switch
#define TEST_SCPI_INPUT(cmd)    result = SCPI_Input_Dbg(&scpi_context, cmd, strlen(cmd))

#if 1  // hmm
	TEST_SCPI_INPUT("*CLS\r\n");
	TEST_SCPI_INPUT("ACQ:ATT 5.0\r\n");
    TEST_SCPI_INPUT(":ACQ:ATT 5.0\r\n");
    TEST_SCPI_INPUT(":ACQ:SMP:VALUE?\r\n");
    TEST_SCPI_INPUT(":ACQ:AVER:TIME 30\r\n");
    TEST_SCPI_INPUT(":ANAL:FMAR:LMT LSA\r\n");
    TEST_SCPI_INPUT(":WAV:SEND:ASC?\r\n");
    TEST_SCPI_INPUT(":WAV:SEND:BIN?\r\n");
    TEST_SCPI_INPUT(":WAV:SEND:BIN?;*IDN?\r\n");
    TEST_SCPI_INPUT(":WAV:SEND:BIN?;*IDN?;*CLS\r\n");

    // test optional keyword
    TEST_SCPI_INPUT(":MEASure:VOLTage:DC?\r\n");
    TEST_SCPI_INPUT("VOLTage:DC?\r\n");
    TEST_SCPI_INPUT("MEAS:VOLT:DC?  12,50\r\n");
    TEST_SCPI_INPUT("VOLT:DC?  12,50\r\n");
    TEST_SCPI_INPUT("ACQuire:SMPinterval:VALue?\r\n");
    TEST_SCPI_INPUT("ACQuire:VALue?\r\n");
    TEST_SCPI_INPUT("ACQ:SMP:VAL?\r\n");
    TEST_SCPI_INPUT("ACQ:VAL?\r\n");
    TEST_SCPI_INPUT("ACQuire:AVERage:TIME 50\r\n");
    TEST_SCPI_INPUT("ACQuire:AVERage 50\r\n");
    TEST_SCPI_INPUT(":ANALysis:FMARker:LMTechnique LSA\r\n");
    TEST_SCPI_INPUT(":FMARker:LMTechnique LSA\r\n");
    TEST_SCPI_INPUT(":ANAL:FMAR:LMT LSA\r\n");
    TEST_SCPI_INPUT(":FMAR:LMT LSA\r\n");
    TEST_SCPI_INPUT("WAVedata:SEND:ASCii?\r\n");
    TEST_SCPI_INPUT("WAVedata:ASCii?\r\n");
    TEST_SCPI_INPUT("WAV:SEND:ASC?\r\n");
    TEST_SCPI_INPUT("WAV:ASC?\r\n");
    TEST_SCPI_INPUT("WAVedata:SEND:BINary?\r\n");
    TEST_SCPI_INPUT("WAVedata:SEND?\r\n");
#endif

#if 0 // DMM
    TEST_SCPI_INPUT("*CLS\r\n");
    TEST_SCPI_INPUT("*RST\r\n");
    TEST_SCPI_INPUT("MEAS:volt:DC? 12,50;*OPC\r\n");
    TEST_SCPI_INPUT("*IDN?\r\n");
    TEST_SCPI_INPUT("SYST:VERS?");
    TEST_SCPI_INPUT("\r\n*ID");
    TEST_SCPI_INPUT("N?");
    TEST_SCPI_INPUT(""); // emulate command timeout

    TEST_SCPI_INPUT("*ESE\r\n"); // cause error -109, missing parameter
    TEST_SCPI_INPUT("*ESE 0x20\r\n");

    TEST_SCPI_INPUT("*SRE 0xFF\r\n");
    
    TEST_SCPI_INPUT("IDN?\r\n"); // cause error -113, undefined header

    TEST_SCPI_INPUT("SYST:ERR?\r\n");
    TEST_SCPI_INPUT("SYST:ERR?\r\n");
    TEST_SCPI_INPUT("*STB?\r\n");
    TEST_SCPI_INPUT("*ESR?\r\n");
    TEST_SCPI_INPUT("*STB?\r\n");

    TEST_SCPI_INPUT("meas:volt:dc? 0.01 V, Default\r\n");
    TEST_SCPI_INPUT("meas:volt:dc?\r\n");
    TEST_SCPI_INPUT("meas:volt:dc? def, 0.00001\r\n");
    TEST_SCPI_INPUT("meas:volt:dc? 0.00001\r\n");
#endif
#endif

#if 1 // interactive demo switch
    //printf("%.*s %s\r\n",  3, "asdadasdasdasdas", "b");
    // interactive demo
    printf("\n\n============interactive stage============\n");
    char smbuffer[50];
    while (1) {
         printf("Please input SCPI CMD, end with 'ENTER': \n");
         fgets(smbuffer, 50, stdin);
         SCPI_Input_Dbg(&scpi_context, smbuffer, strlen(smbuffer));
    }

#endif
    return (EXIT_SUCCESS);
}

