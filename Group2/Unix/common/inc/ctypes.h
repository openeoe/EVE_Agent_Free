#ifndef __CTYPES_H__
#define __CTYPES_H__

/*****************************************************************************
 *
 * ctypes.h
 *
 *      Copyright (c) 2002 Wipro Technologies
 *      All right Reserved
 *
 * author:
 *       D. Sai Siva Kumar
 *
 * description:
 *      ctypes.h provides common typedef for buit in data types
 *
 *
 * CM Details:
 *
 * $Author:  $
 * $Log: $
 * $State: Exp $
 * $Id: $
 * $Name:  $
 *
 ****************************************************************************/



 /********************************** includes *******************************/
#include <limits.h>
#include <stddef.h>

 /********************************** macros *********************************/

#define TRUE    1

#define FALSE   0

 /******************************* exported types ****************************/
#if SCHAR_MAX == 127
typedef signed char I8  ;
#else
#error "No type available for signed 8 bit"
#endif

#if UCHAR_MAX == 255
typedef unsigned char UI8;
#else
#error "No type available for unsigned 8 bit"
#endif


#if INT_MAX == 32767
typedef int I16;
#elif SHRT_MAX == 32767
typedef short I16;
#else
#error "No type available for signed 16 bit"
#endif

#if UINT_MAX == 0xffffU
typedef unsigned int UI16;
#elif USHRT_MAX == 0xffffU
typedef unsigned short UI16;
#else
#error "No type available for unsigned 16 bit"
#endif

#if LONG_MAX == 2147483647L
typedef long I32;
#elif INT_MAX == 2147483647L
typedef int I32;
#else
#error "No type available for signed 32 bit"
#endif

#if ULONG_MAX == 0xffffffffUL
typedef unsigned long UI32  ;
#elif UINT_MAX == 0xffffffffUL
typedef unsigned int UI32   ;
#else
#error "No type available for unsigned 32 bit"
#endif

#ifndef BOOL
#define BOOL    int
#endif

/************************* exported function prototypes ********************/

/***************************** exported globals ****************************/

#endif /* __CTYPES_H__ */

/* end of ctypes.h */
