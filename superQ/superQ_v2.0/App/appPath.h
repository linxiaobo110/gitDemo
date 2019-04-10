/********************************************************************************************************
**                                 Copyright (c)   MEI14
**                                                 All Rights Reserved
**  
**                                 Email:729527658@qq.com   QQ:729527658
**-------------------------------------------------------------------------------------------------------
**  MCU        : STM32F429IG (STMicroelectronics)
**  Compiler   : Keil uVision 5.10
**  Module Name: 
**  Module Date: 2014-03-
**  Module Auth: xiaobo
**  Description: 
**  Version    : V1.0
**  Notes      : 
**-------------------------------------------------------------------------------------------------------
**  Version    : 
**  Modified By: 
**  Date       : 
**  Content    : 
**  Notes      : 
********************************************************************************************************/
#ifndef _APP_PATH_H
#define _APP_PATH_H

/*-----------------------------------------  I N C L U D E S  -----------------------------------------*/
#include "Global.h"

/*---------------------------------------  D E F I N I T I O N  ---------------------------------------*/
typedef struct Path_Struct
{
  u16 stepNow;
  float disThreshod;
  u8 flightPlan;
  float testx;
  float testy;
}PATH_STRUCT;


/*-------------------------------------------  M A C R O S  -------------------------------------------*/


/*--------------------------------------  D E C L A R A T I O N  --------------------------------------*/
/* Internal Variable */

/* External Variable */
extern PATH_STRUCT pathData;
/* Internal Function */
static float calculateDis(float x1, float y1, float x2, float y2);
/* External Function */
u16 Path_getNextPosRef(float xNow, float yNow, float *xTaget, float *yTaget);
u16 Path_getNextPosRefXYP(float xNow, float yNow, float yawNow, float *xTaget, float *yTaget, float *yawTaget);
#endif
//===========================================  End Of File  ===========================================//
