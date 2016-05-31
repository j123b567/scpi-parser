/*-
 * Copyright (c) 2015 Lutz Hoerl, Thorlabs GmbH
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
 * @brief  SCPI parser test with a little GUI built with NI-CVI (Lutz Hoerl, Thorlabs)
 *         You can build this example using CVI 2012SP1, an eval version is available here:
 *         ftp://ftp.ni.com/evaluation/labview/ekit/other/downloader/NILWCVI2012SP1.exe
 *         Size is about 550 MB
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "scpi/scpi.h"
#include "../common/scpi-def.h"

/* NI-CVI specific stuff */
#include <cvirte.h>     
#include <userint.h>
#include "TestLibscpi.h"      /* this is the include for the GUI constants */

/* GLOBAL VARIABLES */
static int panelHandle;

/* PROTOTYPES */
void gui_updateSTB(uint8_t newSTB);
void gui_updateSRE(uint8_t newSTB);
void gui_updateESR(uint8_t newSTB);
void gui_updateESE(uint8_t newSTB);
void updateSSCPIRegister(void);
void updateSTB(void);
void updateSRE(void);
void updateESR(void);
void updateESE(void);

/* a global output buffer to collect output data until it will be 'flushed' */
#define SCPI_OUPUT_BUFFER_SIZE      (256)
char SCPI_outputBuffer[SCPI_OUPUT_BUFFER_SIZE];
unsigned int SCPI_outputBuffer_idx = 0;

/* helper function to remove <LF> and <CR> from the end of a string */
/* this is needed only here for the GUI, there is a 'insert line to a text box' function */

/* that automatically adds a new line, so this prohibits double line feeds for the GUI */
void removeTrailingEndcodes(char *buf) {
    int len;
    len = strlen(buf);

    while (len > 0) {
        len--;
        switch (buf[len]) {
            case '\n':
            case '\r':
                buf[len] = '\0';
                break;
            default:
                len = 0; /* stop loop */
                break;
        }
    }
}

/* wrapper for debugging output, collects debug output until a <LF> */

/* is received, then removes the <LF> and outputs the line on the GUI */
void debug_output(char *buf) {
    static char pbuf[512];
    static int pos = 0;
    int len;

    len = strlen(buf);
    if (buf[len - 1] == '\n') {
        buf[len - 1] == '\0';
        len--;
        memcpy(&pbuf[pos], buf, len);
        pos = 0;
        InsertTextBoxLine(panelHandle, PANEL_OUTPUTDEBUG, -1, pbuf);
    } else {
        memcpy(&pbuf[pos], buf, len);
        pos += len;
    }

}

size_t SCPI_Write(scpi_t * context, const char * data, size_t len) {
    if ((SCPI_outputBuffer_idx + len) > (SCPI_OUPUT_BUFFER_SIZE - 1)) {
        len = (SCPI_OUPUT_BUFFER_SIZE - 1) - SCPI_outputBuffer_idx; /* limit length to left over space */
        /* apparently there is no mechanism to cope with buffers that are too small */
    }
    memcpy(&SCPI_outputBuffer[SCPI_outputBuffer_idx], data, len);
    SCPI_outputBuffer_idx += len;

    SCPI_outputBuffer[SCPI_outputBuffer_idx] = '\0';
    /* return fwrite(data, 1, len, stdout); */
    return len;
}

scpi_result_t SCPI_Flush(scpi_t * context) {
    /* fwrite(SCPI_outputBuffer, 1, SCPI_outputBuffer_idx, stdout); */
    removeTrailingEndcodes(SCPI_outputBuffer);
    InsertTextBoxLine(panelHandle, PANEL_OUTPUT, -1, SCPI_outputBuffer);
    SCPI_outputBuffer_idx = 0;
    return SCPI_RES_OK;
}

int SCPI_Error(scpi_t * context, int_fast16_t err) {
    char buf[512];

    /*  fprintf(stderr, "**ERROR: %d, \"%s\"\r\n", (int16_t) err, SCPI_ErrorTranslate(err)); */
    sprintf(buf, "**ERROR: %d, \"%s\"", (int16_t) err, SCPI_ErrorTranslate(err));
    InsertTextBoxLine(panelHandle, PANEL_OUTPUTERR, -1, buf);
    return 0;
}

scpi_result_t SCPI_Control(scpi_t * context, scpi_ctrl_name_t ctrl, scpi_reg_val_t val) {
    if (SCPI_CTRL_SRQ == ctrl) {
        /*  fprintf(stderr, "**SRQ: 0x%X (%d)\r\n", val, val); */
        SetCtrlVal(panelHandle, PANEL_LEDSRQ, 1);
    } else {
        fprintf(stderr, "**CTRL %02x: 0x%X (%d)\r\n", ctrl, val, val);
    }
    return SCPI_RES_OK;
}

scpi_result_t SCPI_Reset(scpi_t * context) {
    char buf[256];
    /*  fprintf(stderr, "**Reset\r\n"); */
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
int main(int argc, char** argv) {
    int result;

    SCPI_Init(&scpi_context,
        scpi_commands,
        &scpi_interface,
        scpi_units_def,
        SCPI_IDN1, SCPI_IDN2, SCPI_IDN3, SCPI_IDN4,
        scpi_input_buffer, SCPI_INPUT_BUFFER_LENGTH,
        scpi_error_queue_data, SCPI_ERROR_QUEUE_SIZE);

    if (InitCVIRTE(0, argv, 0) == 0)
        return -1; /* out of memory */
    if ((panelHandle = LoadPanel(0, "TestLibscpi.uir", PANEL)) < 0)
        return -1;
    DisplayPanel(panelHandle);

    updateSSCPIRegister();

    RunUserInterface();
    DiscardPanel(panelHandle);

    return (EXIT_SUCCESS);
}

void updateSSCPIRegister(void) {
    updateSTB();
    updateSRE();
    updateESR();
    updateESE();
}

void updateSTB(void) {
    scpi_reg_val_t regVal;

    regVal = SCPI_RegGet(&scpi_context, SCPI_REG_STB);

    gui_updateSTB((uint8_t) (0x00FF & regVal));
}

void updateESR(void) {
    scpi_reg_val_t regVal;

    regVal = SCPI_RegGet(&scpi_context, SCPI_REG_ESR);

    gui_updateESR((uint8_t) (0x00FF & regVal));
}

void updateESE(void) {
    scpi_reg_val_t regVal;

    regVal = SCPI_RegGet(&scpi_context, SCPI_REG_ESE);

    gui_updateESE((uint8_t) (0x00FF & regVal));
}

void updateSRE(void) {
    scpi_reg_val_t regVal;

    regVal = SCPI_RegGet(&scpi_context, SCPI_REG_SRE);

    gui_updateSRE((uint8_t) (0x00FF & regVal));
}

/*
 *  The CALLBACK functions below are called from the CVI runtime engine when
 *  user clicks on buttons, inputs data on the GUI etc. 
 */
int CVICALLBACK cb_scpi_input(int panel, int control, int event,
        void *callbackData, int eventData1, int eventData2) {
    char buf[256];
    int len;

    switch (event) {
        case EVENT_COMMIT:
            GetCtrlVal(panel, control, buf);
            /* we have to add a endcode to make SCPI accept the string, here a <LF> is added */
            len = strlen(buf);
            buf[len] = '\n';
            len++;
            SCPI_Input(&scpi_context, buf, len);

            updateSSCPIRegister();
            break;
    }
    return 0;
}

int CVICALLBACK cb_quit(int panel, int control, int event,
        void *callbackData, int eventData1, int eventData2) {
    switch (event) {
        case EVENT_COMMIT:
            QuitUserInterface(0);
            break;
    }
    return 0;
}

/*
 * Helper functions for GUI
 */
void gui_updateSTB(uint8_t newSTB) {
    char buf[5];

    sprintf(buf, "0x%02X", newSTB);
    SetCtrlVal(panelHandle, PANEL_STBHEX, buf);

    SetCtrlVal(panelHandle, PANEL_STB0, (newSTB & 0x01));
    SetCtrlVal(panelHandle, PANEL_STB1, (newSTB & 0x02));
    SetCtrlVal(panelHandle, PANEL_STB2, (newSTB & 0x04));
    SetCtrlVal(panelHandle, PANEL_STB3, (newSTB & 0x08));
    SetCtrlVal(panelHandle, PANEL_STB4, (newSTB & 0x10));
    SetCtrlVal(panelHandle, PANEL_STB5, (newSTB & 0x20));
    SetCtrlVal(panelHandle, PANEL_STB6, (newSTB & 0x40));
    SetCtrlVal(panelHandle, PANEL_STB7, (newSTB & 0x80));

    if (0x00 == (newSTB & 0x40)) SetCtrlVal(panelHandle, PANEL_LEDSRQ, 0);

}

void gui_updateESR(uint8_t newESR) {
    char buf[5];

    sprintf(buf, "0x%02X", newESR);
    SetCtrlVal(panelHandle, PANEL_ESRHEX, buf);

    SetCtrlVal(panelHandle, PANEL_ESR0, (newESR & 0x01));
    SetCtrlVal(panelHandle, PANEL_ESR1, (newESR & 0x02));
    SetCtrlVal(panelHandle, PANEL_ESR2, (newESR & 0x04));
    SetCtrlVal(panelHandle, PANEL_ESR3, (newESR & 0x08));
    SetCtrlVal(panelHandle, PANEL_ESR4, (newESR & 0x10));
    SetCtrlVal(panelHandle, PANEL_ESR5, (newESR & 0x20));
    SetCtrlVal(panelHandle, PANEL_ESR6, (newESR & 0x40));
    SetCtrlVal(panelHandle, PANEL_ESR7, (newESR & 0x80));
}

void gui_updateESE(uint8_t newESE) {
    char buf[5];

    sprintf(buf, "0x%02X", newESE);
    SetCtrlVal(panelHandle, PANEL_ESEHEX, buf);

    SetCtrlVal(panelHandle, PANEL_ESE0, (newESE & 0x01));
    SetCtrlVal(panelHandle, PANEL_ESE1, (newESE & 0x02));
    SetCtrlVal(panelHandle, PANEL_ESE2, (newESE & 0x04));
    SetCtrlVal(panelHandle, PANEL_ESE3, (newESE & 0x08));
    SetCtrlVal(panelHandle, PANEL_ESE4, (newESE & 0x10));
    SetCtrlVal(panelHandle, PANEL_ESE5, (newESE & 0x20));
    SetCtrlVal(panelHandle, PANEL_ESE6, (newESE & 0x40));
    SetCtrlVal(panelHandle, PANEL_ESE7, (newESE & 0x80));
}

void gui_updateSRE(uint8_t newSRE) {
    char buf[5];

    sprintf(buf, "0x%02X", newSRE);
    SetCtrlVal(panelHandle, PANEL_SREHEX, buf);

    SetCtrlVal(panelHandle, PANEL_SRE0, (newSRE & 0x01));
    SetCtrlVal(panelHandle, PANEL_SRE1, (newSRE & 0x02));
    SetCtrlVal(panelHandle, PANEL_SRE2, (newSRE & 0x04));
    SetCtrlVal(panelHandle, PANEL_SRE3, (newSRE & 0x08));
    SetCtrlVal(panelHandle, PANEL_SRE4, (newSRE & 0x10));
    SetCtrlVal(panelHandle, PANEL_SRE5, (newSRE & 0x20));
    SetCtrlVal(panelHandle, PANEL_SRE6, (newSRE & 0x40));
    SetCtrlVal(panelHandle, PANEL_SRE7, (newSRE & 0x80));
}
