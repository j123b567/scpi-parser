/*------------------------------------------------------------------------------
   DEBUG utilities for test projects
   for evaluation purpose
   
   Author:  Lutz Hoerl, Thorlabs GmbH
   Date:    Jul-13-2015
   Name:    debug.c
*----------------------------------------------------------------------------*/

//#include <cmsis_os.h>
#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include <stdarg.h>        // for va_start, va_end, vsprintf
//#include "rl_usb.h"

#if defined (LPC175x_6x)
   #include "LPC17xx.h"
#elif defined (LPC177x_8x)
   #include "LPC177x_8x.h"
#elif defined (LPC40xx)             // this macro has to be defined via compiler option, is not included setting
   #include "LPC407x_8x_177x_8x.h"
#endif


#include "debug.h"

#ifdef __CC_ARM
volatile int32_t ITM_RxBuffer;         // this is referenced by core_cm4.h

#if defined(DEBUG) || defined(DEBUG_MIN)
   static osMutexId debugMutex;
   osMutexDef(debMtx);
#endif

#elif defined(_CVI_)
   void debug_output(char *buf);

#endif

/*========================================================================*//**
   \brief      Initializes the debig output
   \details    This function creates a mutex that will shiel the \ref itm_printf
               function when it is called from different threads
*//*=========================================================================*/
void init_debug(void)
#if defined(DEBUG) || defined(DEBUG_MIN)
{
#ifdef __CC_ARM
   debugMutex = osMutexCreate(osMutex(debMtx));
#endif   
}
#else
{
}
#endif   

/*========================================================================*//**
   \brief      Print debug output via ITM to SVO
   \details    This function provides a debugging type of printf.
               Instead of output to STDOUT it will write to the debuggers
               SVO (Serial Viewer Output) via ITM (Instrumented Trace Macrocell)
   \param[in]  fmt a format buffer as in sprintf
*//*=========================================================================*/
#if defined(DEBUG) || defined(DEBUG_MIN)
void itm_printf(char *fmt, ...)
{
   va_list           arg_ptr;
   static char       buf[1024];
   char              *cp;

#ifdef __CC_ARM
   osMutexWait(debugMutex, 0);
#endif   
   va_start (arg_ptr, fmt);
   /* len = */ vsprintf (buf, fmt, arg_ptr);
   va_end (arg_ptr);

#ifdef __CC_ARM
   cp = buf;
   while(*cp)
   {
      ITM_SendChar(*cp);
      cp++;
   }
#elif defined(_CVI_)
   debug_output(buf);
#else
   cp = buf;
   while(*cp)
   {
      putchar(*cp);
      cp++;
   }
#endif   

   
#ifdef __CC_ARM
   osMutexRelease(debugMutex);
#endif   
}
#else
void itm_printf(char *fmt, ...)
{
   UNUSED(fmt);
}
#endif   

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
#if defined(DEBUG) || defined(DEBUG_MIN)
void itm_print_buffer(uint8_t *buf, size_t len)
{
   itm_printf("[%d]:", len);
   while(len)
   {
      switch(*buf)
      {
         case '\r':
            itm_printf("<CR>");
            break;

         case '\n':
            itm_printf("<LF>");
            break;

         default:
            if(isprint(*buf))
            {
               itm_printf("%c", *buf);
            }
            else
            {
               itm_printf("<0x%02X>", *buf);
            }
            break;
      }
      buf ++;
      len --;
   }
}
#else
void itm_print_buffer(uint8_t *buf, size_t len)
{
   UNUSED(buf);
   UNUSED(len);
}
#endif   


/*========================================================================*//**
   \brief      Formats a buffer to debug output via ITM to SVO
   \details    This function prints a buffer of given length. The characters 
               will be printed as two digit hexadecimal number with one space
               between them
               (uses \ref itm_printf function)
   \param[in]  buf the buffer to be printed
   \param[in]  len the number of bytes of buf to be printed
*//*=========================================================================*/
#if defined(DEBUG) || defined(DEBUG_MIN)
void itm_print_array(uint8_t *buf, size_t len)
{
   itm_printf("[%d]:", len);
   while(len)
   {
      itm_printf(" %02X", *buf);
      buf ++;
      len --;
   }
}
#else
void itm_print_array(uint8_t *buf, size_t len)
{
   UNUSED(buf);
   UNUSED(len);
}
#endif   


/*========================================================================*//**
   \brief      prints a fixed string to debug output via ITM to SVO
   \param[in]  str the (null terminated) string to be printed
*//*=========================================================================*/
#if defined(DEBUG) || defined(DEBUG_MIN)
void itm_print_string(char *str)
{
   itm_printf("%s", str);
}
#else
void itm_print_string(char *str)
{
   UNUSED(str);
}
#endif   

#ifdef __CC_ARM

/*========================================================================*//**
   \brief      Print a short description for a \ref usbStatus code via ITM
   \param[in]  err the status code as returned by CMSIS USB functions
*//*=========================================================================*/
#if defined(DEBUG) || defined(DEBUG_MIN)
void printUSBError(int err)
{
   switch(err)
   {
      case usbOK              :  itm_printf("usbOK: Function completed with no error\n"); break;
      case usbTimeout         :  itm_printf("usbTimeout: Function completed; time-out occurred\n");   break;
      case usbInvalidParameter:  itm_printf("usbInvalidParameter: Invalid Parameter error: a mandatory parameter was missing or specified an incorrect object\n");   break;
      case usbThreadError     :  itm_printf("usbThreadError: CMSIS-RTOS Thread creation/termination failed\n");   break;
      case usbTimerError      :  itm_printf("usbTimerError: CMSIS-RTOS Timer creation/deletion failed\n");  break;
      case usbSemaphoreError  :  itm_printf("usbSemaphoreError: CMSIS-RTOS Semaphore creation failed\n");   break;
      case usbControllerError :  itm_printf("usbControllerError: Controller does not exist\n"); break;
      case usbDeviceError     :  itm_printf("usbDeviceError: Device does not exist\n");   break;
      case usbDriverError     :  itm_printf("usbDriverError: Driver function produced error\n");   break;
      case usbDriverBusy      :  itm_printf("usbDriverBusy: Driver function is busy\n");  break;
      case usbMemoryError     :  itm_printf("usbMemoryError: Memory management function produced error\n"); break;
      case usbNotConfigured   :  itm_printf("usbNotConfigured: Device is not configured (is connected)\n"); break;
      case usbClassErrorADC   :  itm_printf("usbClassErrorADC: Audio Device Class (ADC) error (no device or device produced error)\n");   break;
      case usbClassErrorCDC   :  itm_printf("usbClassErrorCDC: Communication Device Class (CDC) error (no device or device produced error)\n"); break;
      case usbClassErrorHID   :  itm_printf("usbClassErrorHID: Human Interface Device (HID) error (no device or device produced error)\n");  break;
      case usbClassErrorMSC   :  itm_printf("usbClassErrorMSC: Mass Storage Device (MSC) error (no device or device produced error)\n");  break;
      case usbClassErrorCustom:  itm_printf("usbClassErrorCustom: Custom device Class (Class) error (no device or device produced error)\n");   break;
      case usbUnsupportedClass:  itm_printf("usbUnsupportedClass: Unsupported Class\n");  break;
      case usbTransferStall   :  itm_printf("usbTransferStall: Transfer handshake was stall\n");   break;
      case usbUnknownError    :  itm_printf("usbUnknownError: Unspecified USB error\n");  break;
      default                 :  itm_printf("unkown USB error 0x%02X\n", err);            break;
   }
}
#else
void printUSBError(int err)
{
   UNUSED(err);
}
#endif   

#endif   // __CC_ARM
