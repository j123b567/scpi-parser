/*------------------------------------------------------------------------------
   DEBUG utilities for test projects
   for evaluation purpose

   Author:  Lutz Hoerl, Thorlabs GmbH
   Date:    Jul-13-2015
   Name:    debug.h
*----------------------------------------------------------------------------*/


#define DEBUG_MIN


/*=============================================================================
  Compiler specific stuff
=============================================================================*/


#ifdef __CC_ARM

   //  Unused Variables Warning Suppresion for ARM C compiler
   //  if warning level is 2 or below
   #define UNUSED(x)    { x;}

   // This will disable Warning "#174-D: expression has no effect"
   #pragma diag_suppress = 174

#elif defined (__GNUC__)
   #define UNUSED(x)    {}

#elif defined (_CVI_)
   #define UNUSED(x)    {}
#else
   #error NO compiler defined!
#endif

#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include <stdarg.h>        // for va_start, va_end, vsprintf
   
/*=============================================================================
  DEBUG OUTPUT
=============================================================================*/
/*-----------------------------------------------------------------------------
  The following macros can be used like a printf(). Example:

  PRINT_INFO("This is loop number %d", i);

  will result in:
  "\r\nINFO 08098\Debug\loop.c:123 This is loop number 5"

  output on UART when
  - 08098\Debug\loop.c is the file with the above statement
  - 5 is the actual value of i
  - the statement is in line 123 of loop.c
-----------------------------------------------------------------------------*/

#if defined(DEBUG)
   #define PRINT_ERROR     itm_printf("\r\nERROR :" __FILE__ "[%d] ", __LINE__); itm_printf
   #define PRINT_INFO      itm_printf("\r\nINFO  :" __FILE__ "[%d] ", __LINE__); itm_printf
   #define PRINT_DEBUG     itm_printf("\r\nDEBUG :" __FILE__ "[%d] ", __LINE__); itm_printf
   #define PRINT_OUTPUT    itm_printf
   #define PRINT_ARRAY     itm_printf("\r\nARRAY :" __FILE__ "[%d] ", __LINE__); itm_print_array
   #define PRINT_BUFFER    itm_printf("\r\nBUFFER:" __FILE__ "[%d] ", __LINE__); itm_print_buffer
   #define PRINT_STRING    itm_print_string
#else
   #define PRINT_ERROR     itm_printf
   #define PRINT_INFO      itm_printf
   #define PRINT_DEBUG     itm_printf
   #define PRINT_OUTPUT    itm_printf
   #define PRINT_ARRAY     itm_print_array
   #define PRINT_BUFFER    itm_print_buffer
   #define PRINT_STRING    itm_print_string
#endif

/*========================================================================*//**
   \brief      Initializes the debig output
   \details    This function creates a mutex that will shiel the \ref itm_printf
               function when it is called from different threads
*//*=========================================================================*/
void init_debug(void);

/*========================================================================*//**
   \brief      Print debug output via ITM to SVO
   \details    This function provides a debugging type of printf.
               Instead of output to STDOUT it will write to the debuggers
               SVO (Serial Viewer Output) via ITM (Instrumented Trace Macrocell)
   \param[in]  fmt a format buffer as in sprintf
*//*=========================================================================*/
void itm_printf(char *fmt, ...);

/*========================================================================*//**
   \brief      Formats a (text) buffer to debug output via ITM to SVO
   \details    This function prints a buffer of given length. When the element
               of the buffer to be printed is printable the character will be
               printed as it is, else the character is printed as a two digit
               hexadecimal number in angle brackets, e.g. '<0x09>', the line
               feed and carriage return characters are printed as <LF>, <CR>
               (uses \ref itm_printf function)
   \param[in]  buf the buffer to be printed
   \param[in]  len the number of bytes of buf to be printed
*//*=========================================================================*/
void itm_print_buffer(uint8_t *buf, size_t len);


/*========================================================================*//**
   \brief      Formats a buffer to debug output via ITM to SVO
   \details    This function prints a buffer of given length. The characters 
               will be printed as two digit hexadecimal number with one space
               between them
               (uses \ref itm_printf function)
   \param[in]  buf the buffer to be printed
   \param[in]  len the number of bytes of buf to be printed
*//*=========================================================================*/
void itm_print_array(uint8_t *buf, size_t len);


/*========================================================================*//**
   \brief      prints a fixed string to debug output via ITM to SVO
   \param[in]  str the (null terminated) string to be printed
*//*=========================================================================*/
void itm_print_string(char *str);

/*========================================================================*//**
   \brief      Print a short description for a \ref usbStatus code via ITM
   \param[in]  err the status code as returned by CMSIS USB functions
*//*=========================================================================*/
void printUSBError(int err);

