/********************************************************************************************************
**                                 Copyright (c)   2014?????
**                                                 All Rights Reserved
**  
**                                 Email:729527658@qq.com   QQ:729527658
**-------------------------------------------------------------------------------------------------------
**  MCU        : STM32F407VG (STMicroelectronics)
**  Compiler   : Keil uVision 5.10
**  Module Name: 
**  Module Date: 2016-10-26
**  Module Auth: xiaobo
**  Description: 
**  Version    : V1.0
**  Notes      : This is designed for verifying the DSC method and oberser in paper
**-------------------------------------------------------------------------------------------------------
**  Version    : 
**  Modified By: 
**  Date       : 
**  Content    : 
**  Notes      : 
********************************************************************************************************/
/*-----------------------------------------  I N C L U D E S  -----------------------------------------*/
#include "appDscController.h"
/*---------------------------------------  D E F I N I T I O N  ---------------------------------------*/
//DSC control object
DSC_DATA dscData;
#define DSC_ATTITUDE_U_CONSTRAIN 200
#define DSC_ATTITUDE_DIS 1200
#define DSC_ALTITUDE_DIS 12000
#define DSC_POS_DIS 10
#define Pitch_Angle_Res 0.130f
/*----------------------------------------  F U N C T I O N S  ----------------------------------------*/
/********************************************************************************************************
Function Name: 
Author       : xiaobo
Date         : 2016-10-26
Description  : 
Inputs       : None
Outputs      : None
Notes        : 
Revision     : 
********************************************************************************************************/
void attitudeController(void)
{
  //static float x14Last,x24Last;
  float x13sin,x23sin,x13cos,x23cos;
  float temp;
  
  //x13 -> theta
  //x23 -> phy
  //dis3,4,5 angular phy,theta,psi
  //sin cos sqrt convet 
  dscData.x13 = imuData.angle[1];
  dscData.x23 = imuData.angle[0];
  arm_sin_cos_f32(imuData.angle[0], &x23sin, &x23cos);
  arm_sin_cos_f32(imuData.angle[1], &x13sin, &x13cos);
  dscData.x41 = imuData.angle[2];
  dscData.x14 = imuData.angleV[1];
  dscData.x24 = imuData.angleV[0];
  dscData.x42 = imuData.angleV[2];
  
  /***** Angle Cycle *****/

  temp = dscData.x41 - dscData.r41;
  dscData.sx41 = constrain_float(temp, 6, -6);
  //calculate the derivative
  dscData.smd131 += -dscData.smdAngleE1 * signf(dscData.smd131 - dscData.dx13d)*0.01f;//0.1是由于参数无法配置的而乘的因子
  dscData.dx13d = -dscData.smdAngleE0 * signSqrt(dscData.smd130 - dscData.x13d)*0.01f + dscData.smd131;
  dscData.smd130 += dscData.dx13d;
  //dscData.dx13d = 0;
  dscData.smd231 += -dscData.smdAngleE1 * signf(dscData.smd231 - dscData.dx23d)*0.01f;
  dscData.dx23d = -dscData.smdAngleE0 * signSqrt(dscData.smd230 - dscData.x23d)*0.01f + dscData.smd231;
  dscData.smd230 += dscData.dx23d;
  //dscData.dx23d = 0;
  dscData.smd411 += -dscData.smdAngleE1 * signf(dscData.smd411 - dscData.dr41)*0.01f;
  dscData.dr41 = -dscData.smdAngleE0 * signSqrt(dscData.smd410 - dscData.r41)*0.1f + dscData.smd411;
  dscData.smd410 += dscData.dr41;
  
  //calculate the surface error
  dscData.sx13 = x13sin - dscData.smd130;
  dscData.sx23 = x23sin - dscData.smd230; 
  
  //calculate the control value
  if(x13cos <= 0)
  {
    x13cos = 1;
  }
  temp = 1/x13cos * ( dscData.dx13d - uavData.anglePid[1][0]*dscData.sx13 );//*200是时间的缩放
  dscData.x14d = temp;
  if(x23cos <= 0)
  {
    x23cos = 1;
  }
  temp = 1/x23cos * ( dscData.dx23d - uavData.anglePid[0][0]*dscData.sx23 );
  dscData.x24d = temp;
  temp = dscData.dr41 * uavData.anglePid[2][2]  - uavData.anglePid[2][0] * dscData.sx41;
  dscData.x42d = temp;
  
  /***** Angular Cycle *****/
  //calculate the derivative
  dscData.smd141 += -dscData.smdAE1 * signf(dscData.smd141 - dscData.dx14d)*0.1f;
  dscData.dx14d = -dscData.smdAE0 * signSqrt(dscData.smd140 - dscData.x14d) + dscData.smd141;
  dscData.smd140 += dscData.dx14d;
 // dscData.dx14d = 0;
  dscData.smd241 += -dscData.smdAE1 * signf(dscData.smd241 - dscData.dx24d)*0.1f;
  dscData.dx24d = -dscData.smdAE0 * signSqrt(dscData.smd240 - dscData.x24d) + dscData.smd241;
  dscData.smd240 += dscData.dx24d;
  //dscData.dx24d = 0;
  dscData.smd421 += -dscData.smdAE1 * signf(dscData.smd421 - dscData.dx42d)*0.1f;
  dscData.dx42d = -dscData.smdAE0 * signSqrt(dscData.smd420 - dscData.x42d) + dscData.smd421;
  dscData.smd420 += dscData.dx42d;
  //calculate the surface error
  dscData.sx14 = dscData.x14 - dscData.smd140;
  dscData.sx24 = dscData.x24 - dscData.smd240; 
  dscData.sx42 = dscData.x42 - dscData.x42d;
  //calculate the disturbance
  /*** dis4 ***/
  dscData.dis4Z += -uavData.angleVelocityPid[0][0] * dscData.sx24 * 0.005f; // Z(k) = Z(k-1) - K_24*S_24
  temp = dscData.dis4Z - dscData.sx24;//err = Z - S
  dscData.dis4I += uavData.angleVelocityPid[0][1] * signf(temp); // L_k2*\sum(\sign(err))
  dscData.dis4 = -signSqrt(temp) * uavData.angleVelocityPid[0][2] - dscData.dis4I;// dis = -L_k1*|err|^(1/2) - dis4I
  //dscData.dis4 += dscData.sx24;//old version in PID
  dscData.dis4 = constrain_float(dscData.dis4,DSC_ATTITUDE_DIS, -DSC_ATTITUDE_DIS);//constrain the max integral
  /*** dis5 ***/
  dscData.dis5Z += -uavData.angleVelocityPid[1][0] * dscData.sx14 * 0.005f; // Z(k) = Z(k-1) - K_24*S_24
  temp = dscData.dis5Z - dscData.sx14;//err = Z - S
  dscData.dis5I += uavData.angleVelocityPid[1][1] * signf(temp); // L_k2*\sum(\sign(err))
  dscData.dis5 = -signSqrt(temp) * uavData.angleVelocityPid[1][2] - dscData.dis5I;// dis = -L_k1*|err|^(1/2) - dis4I
  //dscData.dis5 += dscData.sx14;
  dscData.dis5 = constrain_float(dscData.dis5,DSC_ATTITUDE_DIS, -DSC_ATTITUDE_DIS);
  /*** dis6 ***/
  dscData.dis6Z += -uavData.angleVelocityPid[2][0] * dscData.sx42 * 0.005f * 0.01f; // Z(k) = Z(k-1) - K_24*S_24
  temp = dscData.dis6Z - dscData.sx42;//err = Z - S
  dscData.dis6I += uavData.angleVelocityPid[2][1] * signf(temp); // L_k2*\sum(\sign(err))
  dscData.dis6 = -signSqrt(temp) * uavData.angleVelocityPid[2][2] - dscData.dis6I;// dis = -L_k1*|err|^(1/2) - dis4I
  //dscData.dis5 += dscData.sx14;
  dscData.dis6 = constrain_float(dscData.dis6,DSC_ATTITUDE_DIS, -DSC_ATTITUDE_DIS);
  //calculate the control value
  //-kp*s_14 -ki*d_5*0.1 - ki*cos(x_13)*s_13
  temp = -uavData.angleVelocityPid[1][0] * dscData.sx14 - uavData.angleVelocityPid[1][1] * 
              (
              dscData.dis5 * 0.1f + x13cos*dscData.sx13 - dscData.dx14d - 0.33f * dscData.x42 * dscData.x24 * 4
              );
  temp = constrain_float(temp, DSC_ATTITUDE_U_CONSTRAIN, -DSC_ATTITUDE_U_CONSTRAIN);
  dscData.u2 = temp;
  temp = -uavData.angleVelocityPid[0][0] * dscData.sx24 - uavData.angleVelocityPid[0][1] * 
              ( 
              dscData.dis4 * 0.1f + x23cos*dscData.sx23 - dscData.dx24d + 0.33f * dscData.x42 * dscData.x14 * 4
              );//*200=/0.005是时间尺度变换 *4=*2*2=*200/100 *100已经在传感器器板上实现
  temp = constrain_float(temp, DSC_ATTITUDE_U_CONSTRAIN, -DSC_ATTITUDE_U_CONSTRAIN);
  dscData.u1 = temp;
  temp = -uavData.angleVelocityPid[2][0] * dscData.sx42 - uavData.angleVelocityPid[2][1]*
              (
               dscData.dis6 * 0.1f + dscData.sx41 - dscData.dx42d*0.01f //- 0.25f * dscData.x24 * dscData.x14 * 4
              );
//              dscData.test3 = dscData.sx41;
  temp = constrain_float(temp, DSC_ATTITUDE_U_CONSTRAIN, -DSC_ATTITUDE_U_CONSTRAIN);
  dscData.u3 = temp;
  //测试
  uavData.specialAngle2[0] = 0.33f * dscData.x42 * dscData.x14 * 4;
  uavData.specialAngle2[1] = dscData.dx24d;
  uavData.specialAngle2[2] = dscData.u1;
}

/********************************************************************************************************
Function Name: 
Author       : xiaobo
Date         : 2016-11-7
Description  : 
Inputs       : None
Outputs      : None
Notes        : 
Revision     : 
********************************************************************************************************/
void DSC_altitudeController(void)
{
  float x13cos,x23cos;
  float temp;
  #define DSC_D2R 0.01745327f
  dscData.x31 = uavData.position[2];
  dscData.x32 = uavData.velocity[2];
  x13cos = arm_cos_f32(dscData.x13 * DSC_D2R);
  x23cos = arm_cos_f32(dscData.x23 * DSC_D2R);
  if(x13cos == 0)
  {
    x13cos = 1;
  }
  if(x23cos == 0)
  {
    x23cos = 1;
  }
  //position cycle
  temp = dscData.x31 - dscData.r31;
  temp = constrain_float(temp, 150, -150);;
  dscData.sx31 = temp;
  //calculate the derivative
  dscData.smd311 += -dscData.smdHE1 * signf(dscData.smd311 - dscData.dr31)*0.01f;//0.1是由于参数无法配置的而乘的因子
  dscData.dr31 = -dscData.smdHE0 * signSqrt(dscData.smd310 - dscData.r31)*0.1f + dscData.smd311;
  dscData.smd310 += dscData.dr31;
  //height
  //*0.001/0.025 乘以尺度变换，除以时间常数
  temp = dscData.dr31*0.04f  - uavData.posPid[2][0] * dscData.sx31 * 0.1f;
  dscData.x32d = temp;
  
  //position speed cycle

  //calculate the derivative
  dscData.smd321 += -dscData.smdHVE1 * signf(dscData.smd321 - dscData.dx32d) * 0.01f;
  dscData.dx32d = -dscData.smdHVE0 * signSqrt(dscData.smd320 - dscData.x32d) * 0.1f + dscData.smd321;
  dscData.smd320 += dscData.dx32d;
  //calculate the surface error 
  dscData.sx32 = dscData.x32 - dscData.smd320;
  /*** dis3 ***/
  //the integration of error statue (Z) will not begin until the taken-off is completed
  if(dscData.isTakenOf == 1)
  {
    dscData.dis3Z += -uavData.velocityPid[2][0] * dscData.sx32 * 0.025f; // Z(k) = Z(k-1) - K_24*S_24
  }else
  {
    dscData.dis3Z  = 0;
  }
  temp = dscData.dis3Z - dscData.sx32;//err = Z - S
  //the increasement should be bigger when the UAV is taking off
  if(dscData.isTakenOf == 0)
  {
    dscData.dis3I += uavData.velocityPid[2][1] * signf(temp) * 2; // L_k2*\sum(\sign(err))
  }else
  {
    dscData.dis3I += uavData.velocityPid[2][1] * signf(temp); // L_k2*\sum(\sign(err))
  }
  dscData.dis3 = -signSqrt(temp) * uavData.velocityPid[2][2] - dscData.dis3I;// dis = -L_k1*|err|^(1/2) - dis4I
  dscData.dis3 = constrain_float(dscData.dis3,DSC_ALTITUDE_DIS, -DSC_ALTITUDE_DIS);//constrain the max integral
  
  //m = 628g/4,这 里计算出来的是一个旋翼需要的推力，另外，在推力测量时使用了克作为单位，而不是牛，所以不用乘9.8
  temp = 628/x13cos/x23cos*0.25f + 0.628f/x13cos/x23cos*(
     + dscData.dx32d * 40 - uavData.velocityPid[2][0] * dscData.sx32  - dscData.dis3 - dscData.sx31 * 0.25f *0.001f//
      );   
  //控制量倒数乘以40是乘一个时间常数
  //测试
//  uavData.specialAngle2[0] = dscData.dr31;
//  uavData.specialAngle2[1] = dscData.dx32d;
//  uavData.specialPosition[2] = dscData.dis3Z;
//  uavData.specialPosition[0] = dscData.dr31;
//  uavData.specialPosition[1] = dscData.dx32d;;
   
  dscData.u4 = temp;
  uavData.specialPosition[0] = dscData.dis3I;
//  //测试
//  uavData.specialPosition[0] = dscData.smd310;
//  uavData.specialPosition[1] = dscData.smd320 * 10;
//  uavData.specialPosition[2] = dscData.dx32d;
}

/********************************************************************************************************
Function Name: 
Author       : xiaobo
Date         : 2016-11-16
Description  : 
Inputs       : None
Outputs      : None
Notes        : 
Revision     : 
********************************************************************************************************/
void DSC_positionXYController(void)
{
  float x41sin,x41cos;
  float temp,tempDis1,tempDis2;
//  short intValue;
  dscData.x11 = imuData.position[0];
  dscData.x12 = imuData.positionV[0];
  dscData.x21 = imuData.position[1];
  dscData.x22 = imuData.positionV[1];
  arm_sin_cos_f32(dscData.x41, &x41sin, &x41cos);
  
//  dscData.r11 = 0;
//  dscData.r21 = 0;
  
  /** position cycle **/
  /* x */
  temp = dscData.x11 - dscData.r11;
  temp = constrain_float(temp, 300, -300);;
  if(DEF_ABS(temp)< 40)
  {
    dscData.sx11 = 0;
  }else
  {
    dscData.sx11 = temp;
  }
  
  //calculate the derivative
  dscData.smd111 += -dscData.smdPE1 * signf(dscData.smd111 - dscData.dr11)*0.01f;//0.1是由于参数无法配置的而乘的因子
  dscData.dr11 = -dscData.smdPE0 * signSqrt(  dscData.smd110 - dscData.r11)*0.1f + dscData.smd111;
  dscData.smd110 += dscData.dr11;
  //Control Value dscData.dr11
  temp =  - uavData.posPid[0][0] * dscData.sx11 * 0.1f;
  dscData.x12d = temp;
  
  /* y */
  temp = dscData.x21 - dscData.r21;
  temp = constrain_float(temp, 300, -300);
  if(DEF_ABS(temp)<40)
  {
    dscData.sx21 = 0; 
  }else
  {
    dscData.sx21 = temp;
  }
  
  //calculate the derivative
  dscData.smd211 += -dscData.smdPE1 * signf(dscData.smd211 - dscData.dr21)*0.01f;//0.1是由于参数无法配置的而乘的因子
  dscData.dr21 = -dscData.smdPE0 * signSqrt(dscData.smd210 - dscData.r21)*0.1f + dscData.smd211;
  dscData.smd210 += dscData.dr21;
  //Control Value  dscData.dr21 -
  temp =  -uavData.posPid[1][0] * dscData.sx21 * 0.1f;
  dscData.x22d = temp;
  
//  dscData.x12d = (uavData.receriverValue[1] - 1420) * (0.4f);
//  dscData.x22d = -(uavData.receriverValue[0] - 1420) * (0.4f);

  
  /** speed cycle **/
  /* x-y */
  //calculate the derivative
  /*** smd x ***/
  dscData.smd121 += -dscData.smdPVE1 * signf(dscData.smd121 - dscData.dx12d) * 0.01f;
  dscData.dx12d = -dscData.smdPVE0 * signSqrt(dscData.smd120 - dscData.x12d) * 0.1f + dscData.smd121;
  dscData.smd120 += dscData.dx12d;
  /*** smd y ***/
  dscData.smd221 += -dscData.smdPVE1 * signf(dscData.smd221 - dscData.dx22d) * 0.01f;
  dscData.dx22d = -dscData.smdPVE0 * signSqrt(dscData.smd220 - dscData.x22d) * 0.1f + dscData.smd221;
  dscData.smd220 += dscData.dx22d;
  //calculate the surface error 
  dscData.sx12 = dscData.x12 - dscData.smd120;
  dscData.sx22 = dscData.x22 - dscData.smd220;
  /*** dis1 x ***/
  dscData.dis1Z += -uavData.velocityPid[0][0] * dscData.sx12 * 0.025f * dscData.u4 / 2403; // Z(k) = Z(k-1) - K_24*S_24*f/24.03*0.01
  if( (dscData.sx11 < 100) && (dscData.sx11 > -100) )
  {
    dscData.dis1Z += -uavData.velocityPid[0][0] * dscData.sx12 * 0.025f * dscData.u4 / 2403; // Z(k) = Z(k-1) - K_24*S_24*f/24.03*0.01
  }else
  {
    dscData.dis1Z  = 0;
  }
  temp = dscData.dis1Z - dscData.sx12;//err = Z - S
  dscData.dis1I += uavData.velocityPid[0][1] * signf(temp) * 0.01f; // L_k2*\sum(\sign(err))
  if( (dscData.sx11 < 150) && (dscData.sx11 > -150) )
  {
    dscData.dis1I += uavData.velocityPid[0][1] * signf(temp) * 0.01f; // L_k2*\sum(\sign(err))
  }else
  {
//    dscData.dis1I = 0;
    dscData.dis1I += uavData.velocityPid[0][1] * signf(temp) * 0.01f;
    dscData.dis1I = constrain_float(dscData.dis1I, 3,  -3);
  }
  dscData.dis1 = -signSqrt(temp) * uavData.velocityPid[0][2] * 0.01f - dscData.dis1I;// dis = -L_k1*|err|^(1/2) - dis4I
  dscData.dis1 = constrain_float(dscData.dis1,DSC_POS_DIS, -DSC_POS_DIS);//constrain the max integral
  /*** dis2 ***/
  //the integration of error statue (Z) will not begin until the taken-off is completed
  //dscData.dis2Z += -uavData.velocityPid[1][0] * dscData.sx22 * 0.025f * dscData.u4 / 2403; // Z(k) = Z(k-1) - K_24*S_24
  if( (dscData.sx21 < 100) && (dscData.sx21 > -100) )
  {
    dscData.dis2Z += -uavData.velocityPid[1][0] * dscData.sx22 * 0.025f * dscData.u4 / 2403; // Z(k) = Z(k-1) - K_24*S_24
  }else
  {
    dscData.dis2Z  = 0;
  }
  temp = dscData.dis2Z - dscData.sx22;//err = Z - S
  //dscData.dis2I += uavData.velocityPid[1][1] * signf(temp) * 0.01f;
  if( (dscData.sx21 < 150) && (dscData.sx21 > -150) )
  {
    dscData.dis2I += uavData.velocityPid[1][1] * signf(temp) * 0.01f; // L_k2*\sum(\sign(err))
  }else
  {
//    dscData.dis2I = 0;
    dscData.dis2I += uavData.velocityPid[1][1] * signf(temp) * 0.01f;;
    dscData.dis2I = constrain_float(dscData.dis2I, 3,  -3);
  }
  dscData.dis2 = -signSqrt(temp) * uavData.velocityPid[1][2] * 0.01f - dscData.dis2I;// dis = -L_k1*|err|^(1/2) - dis4I
  dscData.dis2 = constrain_float(dscData.dis2,DSC_POS_DIS, -DSC_POS_DIS);//constrain the max integral
  
  /** 计算控制量 **/
  
  //////先调y,x设为手动
  /* x->pitch */
//  temp = (uavData.receriverValue[1] - 1420) * (0.05f) + 1.9f;
//  dscData.x13d = arm_sin_f32(temp*0.0174f);

//  if( DEF_ABS(dscData.sx22) < 50 )
//  {
//    temp = 50;
//  }else
//  {
//    temp = DEF_ABS(dscData.sx22);
//  }
  //为了避免控制量不连续，使用一个低通函数来近似连续
  temp = 20;
//  intValue = (s16)(DEF_ABS(dscData.dis1));
  tempDis1 = -signSqrt(dscData.dis1Z - dscData.sx12) * uavData.velocityPid[0][2] * 0.01f 
             - dscData.dis1I * dscData.dis1I * 0.8f * uavData.posPid[0][1] * signf(dscData.dis1I) / temp
             + dscData.dis1I * dscData.dis1I * 0.2f /temp * signf(dscData.sx12);
//  dscData.dis1 * dscData.dis1 *0.8f * uavData.posPid[0][1] * signf(dscData.dis1) / temp + 
//   dscData.dis1 * dscData.dis1 *0.2f / temp * signf(dscData.sx12);
  //tempDis1 = dscData.dis1 * dscData.dis1 / temp * signf(dscData.sx12);
//  if(dscData.dis1*dscData.sx12 >= 0)
//  {
//    tempDis1 = dscData.dis1 * dscData.dis1 / temp * signf(dscData.sx12);
//  }else
//  {
////    dscData.dis1I = dscData.dis1I*0.95f;
//    tempDis1 = dscData.dis1 * dscData.dis1 / temp * signf(dscData.dis1)*0.9f;;
//  }
  
//  if( DEF_ABS(dscData.sx12) < 50 )
//  {
//    temp = 50;
//  }else
//  {
//    temp = DEF_ABS(dscData.sx12);
//  }
  temp = 20;
//  intValue = (s16)(DEF_ABS(dscData.dis2));
  tempDis2 = -signSqrt(dscData.dis2Z - dscData.sx22) * uavData.velocityPid[1][2] * 0.01f 
             - dscData.dis2I * dscData.dis2I * 0.8f * uavData.posPid[1][1] * signf(dscData.dis2I) / temp
             + dscData.dis2I * dscData.dis2I * 0.2f /temp * signf(dscData.sx22);
//  tempDis2 = dscData.dis2 * dscData.dis2 *0.8f * uavData.posPid[1][1] * signf(dscData.dis2) / temp + 
//  dscData.dis2 * dscData.dis2 *0.2f / temp * signf(dscData.sx22);
//  tempDis2 = dscData.dis2 * dscData.dis2 / temp * signf(dscData.sx22);
//  if(dscData.dis2*dscData.sx22 >= 0)
//  {
//    tempDis2 = dscData.dis2 * dscData.dis2 / temp * signf(dscData.sx22);
//  }else
//  {
////    dscData.dis2I = dscData.dis2I*0.95f;
//    tempDis2 = dscData.dis2 * dscData.dis2 / temp * signf(dscData.dis2)*0.9f;;
//  }
  //tempDis2 = dscData.dis2 * dscData.dis2 / temp * signf(dscData.sx22);
  /* x-y->roll */
  /* control value roll */
  temp = 
         -24.03f/dscData.u4*x41sin*
        ( uavData.velocityPid[0][0] * dscData.sx12 * 0.01f + tempDis1 - uavData.posPid[0][2] * dscData.x12d * 0.01f
        )
         +24.03f/dscData.u4*x41cos*
        ( uavData.velocityPid[0][0] * dscData.sx22 * 0.01f + tempDis2 - uavData.posPid[1][2] * dscData.x22d * 0.01f
        );
//  if( (dscData.sx12 != 0) && (dscData.sx22 != 0))
//  {
//    temp = 235
//         -24.03f/dscData.u4*x41sin*
//        ( uavData.velocityPid[0][0] * dscData.sx12 * 0.01f + tempDis1 )
//         +24.03f/dscData.u4*x41cos*
//        ( uavData.velocityPid[0][0] * dscData.sx22 * 0.01f + tempDis2 );
//  }else
//  {
//        temp = 
//         -24.03f/dscData.u4*x41sin*
//        ( uavData.velocityPid[0][0] * dscData.sx12 * 0.01f)
//         +24.03f/dscData.u4*x41cos*
//        ( uavData.velocityPid[0][0] * dscData.sx22 * 0.01f);
//  }
    
  temp = constrain_float(temp, Pitch_Angle_Res, -Pitch_Angle_Res);
  dscData.x23d = temp - 0.025f;
  //dscData.x23d = temp;
  
  /* control value pitch */
  temp = -24.03f/dscData.u4*x41cos*(  
  uavData.velocityPid[0][0] * dscData.sx12 * 0.01f  + tempDis1 - uavData.posPid[0][2] * dscData.x12d * 0.01f
  )
       -24.03f/dscData.u4*x41sin*( 
  uavData.velocityPid[0][0] * dscData.sx22 * 0.01f  + tempDis2 - uavData.posPid[1][2] * dscData.x22d * 0.01f
  );
//  if( (dscData.sx12 != 0) && (dscData.sx22 != 0))
//  {
//    temp = -24.03f/dscData.u4*x41cos*( 
//    uavData.velocityPid[0][0] * dscData.sx12 * 0.01f  + tempDis1
//    )
//         -24.03f/dscData.u4*x41sin*( 
//    uavData.velocityPid[0][0] * dscData.sx22 * 0.01f  + tempDis2
//    );
//  }else
//  {
//    temp = -24.03f/dscData.u4*x41cos*( uavData.velocityPid[0][0] * dscData.sx12 * 0.01f )
//         -24.03f/dscData.u4*x41sin*( uavData.velocityPid[0][0] * dscData.sx22 * 0.01f )  ;
//  }
   
  temp = constrain_float(temp, Pitch_Angle_Res, -Pitch_Angle_Res);
  //dscData.x13d = temp;
  dscData.x13d = temp + 0.0304f;

  //测试-
//  uavData.specialPosition[0] = tempDis1 * 1000;;
//  uavData.specialPosition[1] = tempDis2 * 1000;
}

/********************************************************************************************************
Function Name: 
Author       : xiaobo
Date         : 2016-10-26
Description  : 
Inputs       : None
Outputs      : None
Notes        : 
Revision     : 
********************************************************************************************************/
void attitudeStateClear(void)
{
  //dis
  dscData.dis4 = 0;
  dscData.dis5 = 0;
  dscData.dis6 = 0;
  dscData.dis4Z = 0;
  dscData.dis4I = 0;
  dscData.dis5Z = 0;
  dscData.dis5I = 0;
  dscData.dis6Z = 0;
  dscData.dis6I = 0;
  //sliding mode differentiator
  dscData.smd110 = 0;dscData.smd111 = 0;
  dscData.smd120 = 0;dscData.smd121 = 0;
  dscData.smd130 = 0;dscData.smd131 = 0;
  dscData.smd140 = 0;dscData.smd141 = 0;
  dscData.smd210 = 0;dscData.smd211 = 0;
  dscData.smd220 = 0;dscData.smd221 = 0;
  dscData.smd230 = 0;dscData.smd231 = 0;
  dscData.smd240 = 0;dscData.smd241 = 0;
  dscData.smd310 = 0;dscData.smd311 = 0;
  dscData.smd320 = 0;dscData.smd321 = 0;
  dscData.smd410 = 0;dscData.smd411 = 0;
  dscData.smd420 = 0;dscData.smd421 = 0;
  //clear the flag of the taken-off
  dscData.isTakenOf = 0;
}

/********************************************************************************************************
Function Name: 
Author       : xiaobo
Date         : 2016-11-8
Description  : 
Inputs       : None
Outputs      : None
Notes        : 
Revision     : 
********************************************************************************************************/
void DSC_altitudeStateClear(void)
{
  //dis
  dscData.dis3 = 0;
  dscData.dis3I = 0;
  dscData.dis3Z = 0;
  //smd
  dscData.smd310 = 0;dscData.smd311 = 0;
  dscData.smd320 = 0;dscData.smd321 = 0;
}

/********************************************************************************************************
Function Name: 
Author       : xiaobo
Date         : 2016-11-8
Description  : 
Inputs       : None
Outputs      : None
Notes        : 
Revision     : 
********************************************************************************************************/
void DSC_positionStateClear(void)
{
  //dis
  dscData.dis1 = 0;
  dscData.dis1I = 0;
  dscData.dis1Z = 0;
  dscData.dis2 = 0;
  dscData.dis2I = 0;
  dscData.dis2Z = 0;
  //smd
  dscData.smd110 = 0;dscData.smd111 = 0;
  dscData.smd120 = 0;dscData.smd121 = 0;
  dscData.smd210 = 0;dscData.smd211 = 0;
  dscData.smd220 = 0;dscData.smd221 = 0;
}
/********************************************************************************************************
Function Name: 
Author       : xiaobo
Date         : 2016-10-27
Description  : 
Inputs       : None
Outputs      : Nonev
Notes        : 
Revision     : 
**********************3**********************************************************************************/
void dscParaInit(void)
{
//  dscData.disAL2 = 0.5f;
//  dscData.disAL1 = 3;
}

/********************************************************************************************************
Function Name: 求取带符号的平方根
Author       : xiaobo
Date         : 2016-10-27
Description  : 
Inputs       : None
Outputs      : None
Notes        : 
Revision     : 
********************************************************************************************************/
float signSqrt(float x)
{
  float temp;
  temp = DEF_ABS(x);
  arm_sqrt_f32(temp, &temp);
  if(x>0)
  {
    return temp;
  }else
  {
    return -temp;
  }
}

float signf(float x)
{
  if(x>0)
    return 1;
  else
    return (-1);
}

//===========================================  End Of File  ===========================================//
