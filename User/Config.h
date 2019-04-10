/********************************************************************************************************
**                                 Copyright (c)   2011     MicroMouse
**                                                 All Rights Reserved
**  
**                                 Email:ustbyf@126.com   QQ:316969230
**-------------------------------------------------------------------------------------------------------
**  MCU        : STM32F405RG (STMicroelectronics)
**  Compiler   : Keil uVision 4.22a
**  Module Name: Config.h
**  Module Date: 2012-02-23
**  Module Auth: ÔÆ·å
**  Description: 
**  Version    : V2.0 
**  Notes      : 
**-------------------------------------------------------------------------------------------------------
**  Version    : 
**  Modified By: 
**  Date       : 
**  Content    : 
**  Notes      : 
********************************************************************************************************/

#ifndef __CONFIG_H__
#define __CONFIG_H__

/*-----------------------------------------  I N C L U D E S  -----------------------------------------*/

/*---------------------------------------  D E F I N I T I O N  ---------------------------------------*/

/*-------------------------------------------  M A C R O S  -------------------------------------------*/
/* DataType Define */
typedef unsigned char                  UINT8;       //            0 ~ 255
typedef   signed char                  SINT8;       //         -128 ~ 127
typedef unsigned short                 UINT16;      //            0 ~ 65535
typedef   signed short                 SINT16;      //       -32768 ~ 32767
typedef unsigned long                  UINT32;      //            0 ~ 4294967295
typedef   signed long                  SINT32;      //  -2147483648 ~ 2147483647

typedef unsigned char                  INT8U;       //            0 ~ 255
typedef   signed char                  INT8S;       //         -128 ~ 127
typedef unsigned short                 INT16U;      //            0 ~ 65535
typedef   signed short                 INT16S;      //       -32768 ~ 32767
typedef unsigned long                  INT32U;      //            0 ~ 4294967295
typedef   signed long                  INT32S;      //  -2147483648 ~ 2147483647

typedef unsigned char                  uint8;       //            0 ~ 255
typedef   signed char                  sint8;       //         -128 ~ 127
typedef unsigned short                 uint16;      //            0 ~ 65535
typedef   signed short                 sint16;      //       -32768 ~ 32767
typedef unsigned long                  uint32;      //            0 ~ 4294967295
typedef   signed long                  sint32;      //  -2147483648 ~ 2147483647

typedef unsigned char                  int8u;       //            0 ~ 255
typedef   signed char                  int8s;       //         -128 ~ 127
typedef unsigned short                 int16u;      //            0 ~ 65535
typedef   signed short                 int16s;      //       -32768 ~ 32767
typedef unsigned long                  int32u;      //            0 ~ 4294967295
typedef   signed long                  int32s;      //  -2147483648 ~ 2147483647

typedef float                          FP32;
typedef double                         FP64;

/* Const Define */
#ifndef ON
#define ON                             (1)
#endif

#ifndef OFF
#define OFF                            (0)
#endif

#ifndef YES
#define YES                            (1)
#endif

#ifndef NO
#define NO                             (0)
#endif

#ifndef VALID
#define VALID                          (1)
#endif

#ifndef INVALID
#define INVALID                        (0)
#endif

#ifndef TRUE
#define TRUE                           (1)
#endif

#ifndef FALSE
#define FALSE                          (0)
#endif

/* MIN/MAX/ABS Macros */
#define MIN(a, b)                      (((a) < (b))?(a):(b))
#define MAX(a, b)                      (((a) > (b))?(a):(b))
#define ABS(x)                         (((x) > 0)?(x):(-(x)))

/* Constants */
#define PI                             3.14159265359

/* Function Define */
#ifndef ClrBit
#define ClrBit(reg, bit)               reg &= ~(1 << bit)
#endif

#ifndef SetBit
#define SetBit(reg, bit)               reg |= (1 << bit)
#endif

#endif

//===========================================  End Of File  ===========================================//

