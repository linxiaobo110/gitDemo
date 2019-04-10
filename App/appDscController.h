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
#ifndef _APP_DSC_CONTROLLER_H
#define _APP_DSC_CONTROLLER_H

/*-----------------------------------------  I N C L U D E S  -----------------------------------------*/
#include "Global.h"

/*---------------------------------------  D E F I N I T I O N  ---------------------------------------*/
typedef struct DSC_DATA
{
  /* X-Y  */
  //state
  float x11,x21;//positon x,y
  float x12,x22;//speed x,y
  float x13,x23;//sin(angle) theta,phy
  float x14,x24;//angular theta,phy
  //ref-virtual input
  float r11,r21;
  float x12d,x22d;
  float x13d,x23d;
  float x14d,x24d;
  //diffective of vitual input
  float dr11,dr21;
  float dx12d,dx22d;
  float dx13d,dx23d;
  float dx14d,dx24d;
  //disturbance observer
  float dis1,dis2;//observer in x12,x22
  float dis4,dis5;//observer in x14,x24
  float dis1I,dis2I;//the integral part in dis1,2
  float dis4I,dis5I;//the integral part in dis3,5
  float dis1Z,dis2Z;//the observer state,x12,x22
  float dis4Z,dis5Z;//the observer state,x14,x24
  //slide mode differentitor
  float smd110,smd111;
  float smd210,smd211;
  float smd120,smd121;
  float smd220,smd221;
  float smd130,smd131;
  float smd230,smd231;
  float smd140,smd141;
  float smd240,smd241;
  
  //surface error
  float sx11,sx21;
  float sx12,sx22;
  float sx13,sx23;
  float sx14,sx24;
  
  /* Z-psi */
  float x31,x32;//position z, speed z
  float x41,x42;//angle psi, angular psi
  //ref-virtual input
  float r31,x32d;
  float r41,x42d;
  //diffective of vitual input
  float dr31,dx32d;
  float dr41,dx42d;
  //disturbance observer
  float dis3,dis6;//oberser in x32,x42
  float dis3I,dis6I;//the integral part in dis 3,6
  float dis3Z,dis6Z;//the observer state,x32,x42
  //slide mode differentitor
  float smd310,smd311;
  float smd320,smd321;
  float smd410,smd411;
  float smd420,smd421;
  //surface error
  float sx31,sx41;
  float sx32,sx42;
  
  /* control value */
  float u1,u2,u3,u4;
  
  /* attitude cycle */
  float disAL1;//the L1 in attitude cycle
  float disAL2;//the L2 in attitude cycle
  float smdAE0;//SMD:the E0 in attitude cycle 
  float smdAE1;//the E1 in attitude cycle
  float smdAngleE0;//SMD:the E0 in attitude cycle 
  float smdAngleE1;//the E1 in attitude cycle
  float disYawL1;//DIS:the L1 in Yaw cycle
  float disYawL2;//the L1 in Yaw cycle
  
  /* position cycle */
  float smdHE0;//SMD:the E0 in altitude cycle
  float smdHE1;
  float smdHVE0;//SMD:the E0 in the altitude speed cycle
  float smdHVE1;
  float smdPE0;//SMD:the E0 in XY of positon cycle
  float smdPE1;
  float smdPVE0;//SMD: the E0 in XY of speed cycle
  float smdPVE1;
  
  u8 isTakenOf;
  
  float test1,test2,test3;
}DSC_DATA;

/*-------------------------------------------  M A C R O S  -------------------------------------------*/


/*--------------------------------------  D E C L A R A T I O N  --------------------------------------*/
/* Internal Variable */

/* External Variable */
extern DSC_DATA dscData;
/* Internal Function */
/* External Function */
void attitudeController(void);
void DSC_altitudeController(void);
void DSC_positionXYController(void);
void attitudeStateClear(void);
void DSC_altitudeStateClear(void);
void DSC_positionStateClear(void);
float signSqrt(float x);
void dscParaInit(void);
float signf(float x);
#endif
//===========================================  End Of File  ===========================================//
