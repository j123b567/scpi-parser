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
#define  PANEL_INPUT                      2       /* control type: string, callback function: cb_scpi_input */
#define  PANEL_OUTPUTDEBUG                3       /* control type: textBox, callback function: (none) */
#define  PANEL_OUTPUTERR                  4       /* control type: textBox, callback function: (none) */
#define  PANEL_OUTPUT                     5       /* control type: textBox, callback function: (none) */
#define  PANEL_QUIT                       6       /* control type: command, callback function: cb_quit */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK cb_quit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK cb_scpi_input(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
