/********************************************************************************************************
**                                 Copyright (c)   2014?????
**                                                 All Rights Reserved
**  
**                                 Email:729527658@qq.com   QQ:729527658
**-------------------------------------------------------------------------------------------------------
**  MCU        : STM32F429ZI (STMicroelectronics)
**  Compiler   : Keil uVision 5.10
**  Module Name: 
**  Module Date: 2014-
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
/*-----------------------------------------  I N C L U D E S  -----------------------------------------*/
#include "appPath.h"
/*---------------------------------------  D E F I N I T I O N  ---------------------------------------*/
PATH_STRUCT pathData;
#define PathLength 4
float pathArray[PathLength][2] = 
{
  {0,0},
  {800,0},
  {800,800},
  {0,800}
};
#define PathLength2 6
float pathArrayp2[PathLength2][3] = 
{
  {-400,-600,0},
  {500,-600,0},
  {500,-600,90},
  {500,1000,90},
  {-400,1000,90},
  {-400,1000,0},
};

//float pathArrayp2[PathLength2][3] = 
//{
//  {-400,-600,0},
//  {400,-600,0},
//  {400,800,0},
//  {-400,800,0},
//};
  
/*----------------------------------------  F U N C T I O N S  ----------------------------------------*/
/********************************************************************************************************
Function Name: 获取下一个目标点的函数
Author       : xiaobo
Date         : 2016-11-29
Description  : 
Inputs       : None
Outputs      : None
Notes        : 
Revision     : 
********************************************************************************************************/
u16 Path_getNextPosRef(float xNow, float yNow, float *xTaget, float *yTaget)
{
  static u16 stableCounter;
  if(calculateDis(xNow,yNow,pathArray[pathData.stepNow][0],pathArray[pathData.stepNow][1]) < pathData.disThreshod)
  {
    stableCounter++;
    if(stableCounter == 40)
    {
      pathData.stepNow++;
      if(pathData.stepNow == PathLength)
      {
        pathData.stepNow = 0;
      }
      stableCounter = 0;
    }
  }else
  {
    stableCounter = 0;
  }
  *xTaget = pathArray[pathData.stepNow][0];
  *yTaget = pathArray[pathData.stepNow][1];
  return 0;
}

/********************************************************************************************************
Function Name: 获取下一个目标点的函数
Author       : xiaobo
Date         : 2016-11-29
Description  : 
Inputs       : None
Outputs      : None
Notes        : 
Revision     : 
********************************************************************************************************/
u16 Path_getNextPosRefXYP(float xNow, float yNow, float yawNow, float *xTaget, float *yTaget, float *yawTaget)
{
  static u16 stableCounter;
  if(DEF_ABS(yawNow - pathArrayp2[pathData.stepNow][2]) < 5)
  {
    //延时缓解
    if( (pathData.stepNow > 1) && (pathArrayp2[pathData.stepNow][0] == pathArrayp2[pathData.stepNow-1][0])
        && (pathArrayp2[pathData.stepNow][1] == pathArrayp2[pathData.stepNow-1][1])
        && (pathArrayp2[pathData.stepNow][2] == pathArrayp2[pathData.stepNow-1][2]) )
    {
      stableCounter++;
      if(stableCounter == 200)
      {
        pathData.stepNow++;
        if(pathData.stepNow == PathLength2)
        {
          pathData.stepNow = 0;
        }
        stableCounter = 0;
      }
    }
    else if(calculateDis(xNow,yNow,pathArrayp2[pathData.stepNow][0],pathArrayp2[pathData.stepNow][1]) < pathData.disThreshod)
    {
      stableCounter++;
      if(stableCounter == 40)
      {
        pathData.stepNow++;
        if(pathData.stepNow == PathLength2)
        {
          pathData.stepNow = 0;
        }
        stableCounter = 0;
      }
    }else
    {
      stableCounter = 0;
    }
//    *yawTaget = pathArrayp2[pathData.stepNow][2];
  }else
  {
    if(*yawTaget - pathArrayp2[pathData.stepNow][2] > 3)
    {
      *yawTaget = *yawTaget - 0.2f;
    }else if(*yawTaget - pathArrayp2[pathData.stepNow][2]  < -3)
    {
      *yawTaget = *yawTaget + 0.2f;
    }else
    {
      *yawTaget = pathArrayp2[pathData.stepNow][2];
    }
    
//    if(*yawTaget - pathArrayp2[pathData.stepNow][2] > 8)
//    {
//      if( DEF_ABS( *yawTaget - yawNow) < 2 )
//      {
//        *yawTaget = *yawTaget  - 8;
//      }
//    }else if(*yawTaget - pathArrayp2[pathData.stepNow][2]  < -8)
//    {
//      if( DEF_ABS( *yawTaget - yawNow) < 2 )
//      {
//        *yawTaget = *yawTaget + 8;
//      }
////      *yawTaget = yawNow + 8;
//    }else
//    {
//      *yawTaget = pathArrayp2[pathData.stepNow][2];
//    }
  }
  *xTaget = pathArrayp2[pathData.stepNow][0];
  *yTaget = pathArrayp2[pathData.stepNow][1];
  return 0;
}

/********************************************************************************************************
Function Name: 计算两点距离，即判断是否到达目标点
Author       : xiaobo
Date         : 2016-11-29
Description  : 
Inputs       : None
Outputs      : None
Notes        : 
Revision     : 
********************************************************************************************************/
static float calculateDis(float x1, float y1, float x2, float y2)
{
  float dis1,dis2;
  dis1 = DEF_ABS(x1-x2);
  dis2 = DEF_ABS(y1-y2);
  if(dis1>dis2)
  {
    return dis1;
  }else
  {
    return dis2;
  }
}
//===========================================  End Of File  ===========================================//
