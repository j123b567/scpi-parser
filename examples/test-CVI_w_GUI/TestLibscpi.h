/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/* Copyright (c) National Instruments 2015. All Rights Reserved.          */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
extern "C" {
#endif

    /* Panels and Controls: */

#define  PANEL                            1
#define  PANEL_OUTPUTDEBUG                2       /* control type: textBox, callback function: (none) */
#define  PANEL_OUTPUTERR                  3       /* control type: textBox, callback function: (none) */
#define  PANEL_OUTPUT                     4       /* control type: textBox, callback function: (none) */
#define  PANEL_INPUT                      5       /* control type: string, callback function: cb_scpi_input */
#define  PANEL_SREHEX                     6       /* control type: string, callback function: (none) */
#define  PANEL_SRE0                       7       /* control type: radioButton, callback function: (none) */
#define  PANEL_STBHEX                     8       /* control type: string, callback function: (none) */
#define  PANEL_SRE1                       9       /* control type: radioButton, callback function: (none) */
#define  PANEL_STB0                       10      /* control type: radioButton, callback function: (none) */
#define  PANEL_SRE2                       11      /* control type: radioButton, callback function: (none) */
#define  PANEL_STBTEXT0                   12      /* control type: textMsg, callback function: (none) */
#define  PANEL_SRE3                       13      /* control type: radioButton, callback function: (none) */
#define  PANEL_STB1                       14      /* control type: radioButton, callback function: (none) */
#define  PANEL_SRE4                       15      /* control type: radioButton, callback function: (none) */
#define  PANEL_STBTEXT1                   16      /* control type: textMsg, callback function: (none) */
#define  PANEL_SRE5                       17      /* control type: radioButton, callback function: (none) */
#define  PANEL_STB2                       18      /* control type: radioButton, callback function: (none) */
#define  PANEL_SRE6                       19      /* control type: radioButton, callback function: (none) */
#define  PANEL_STBTEXT2                   20      /* control type: textMsg, callback function: (none) */
#define  PANEL_SRE7                       21      /* control type: radioButton, callback function: (none) */
#define  PANEL_STB3                       22      /* control type: radioButton, callback function: (none) */
#define  PANEL_STBTEXT3                   23      /* control type: textMsg, callback function: (none) */
#define  PANEL_STB4                       24      /* control type: radioButton, callback function: (none) */
#define  PANEL_STBTEXT4                   25      /* control type: textMsg, callback function: (none) */
#define  PANEL_STB5                       26      /* control type: radioButton, callback function: (none) */
#define  PANEL_STBTEXT5                   27      /* control type: textMsg, callback function: (none) */
#define  PANEL_STB6                       28      /* control type: radioButton, callback function: (none) */
#define  PANEL_STBTEXT6                   29      /* control type: textMsg, callback function: (none) */
#define  PANEL_STB7                       30      /* control type: radioButton, callback function: (none) */
#define  PANEL_STBTEXT7                   31      /* control type: textMsg, callback function: (none) */
#define  PANEL_ESEHEX                     32      /* control type: string, callback function: (none) */
#define  PANEL_ESRHEX                     33      /* control type: string, callback function: (none) */
#define  PANEL_ESE0                       34      /* control type: radioButton, callback function: (none) */
#define  PANEL_ESR0                       35      /* control type: radioButton, callback function: (none) */
#define  PANEL_ESE1                       36      /* control type: radioButton, callback function: (none) */
#define  PANEL_ESRTEXT0                   37      /* control type: textMsg, callback function: (none) */
#define  PANEL_ESE2                       38      /* control type: radioButton, callback function: (none) */
#define  PANEL_ESR1                       39      /* control type: radioButton, callback function: (none) */
#define  PANEL_ESE3                       40      /* control type: radioButton, callback function: (none) */
#define  PANEL_ESRTEXT1                   41      /* control type: textMsg, callback function: (none) */
#define  PANEL_ESE4                       42      /* control type: radioButton, callback function: (none) */
#define  PANEL_ESR2                       43      /* control type: radioButton, callback function: (none) */
#define  PANEL_ESE5                       44      /* control type: radioButton, callback function: (none) */
#define  PANEL_ESRTEXT2                   45      /* control type: textMsg, callback function: (none) */
#define  PANEL_ESE6                       46      /* control type: radioButton, callback function: (none) */
#define  PANEL_ESR3                       47      /* control type: radioButton, callback function: (none) */
#define  PANEL_ESE7                       48      /* control type: radioButton, callback function: (none) */
#define  PANEL_ESRTEXT3                   49      /* control type: textMsg, callback function: (none) */
#define  PANEL_ESR4                       50      /* control type: radioButton, callback function: (none) */
#define  PANEL_ESRTEXT4                   51      /* control type: textMsg, callback function: (none) */
#define  PANEL_ESR5                       52      /* control type: radioButton, callback function: (none) */
#define  PANEL_ESRTEXT5                   53      /* control type: textMsg, callback function: (none) */
#define  PANEL_ESR6                       54      /* control type: radioButton, callback function: (none) */
#define  PANEL_ESRTEXT6                   55      /* control type: textMsg, callback function: (none) */
#define  PANEL_ESR7                       56      /* control type: radioButton, callback function: (none) */
#define  PANEL_ESRTEXT7                   57      /* control type: textMsg, callback function: (none) */
#define  PANEL_QUIT                       58      /* control type: command, callback function: cb_quit */
#define  PANEL_LEDSRQ                     59      /* control type: LED, callback function: (none) */


    /* Control Arrays: */

    /* (no control arrays in the resource file) */


    /* Menu Bars, Menus, and Menu Items: */

    /* (no menu bars in the resource file) */


    /* Callback Prototypes: */

    int CVICALLBACK cb_quit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
    int CVICALLBACK cb_scpi_input(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
}
#endif
