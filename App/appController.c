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
#include "App.h"
/*---------------------------------------  D E F I N I T I O N  ---------------------------------------*/

/*----------------------------------------  F U N C T I O N S  ----------------------------------------*/
/********************************************************************************************************
Function Name: 
Author       : xiaobo
Date         : 2013-03-
Description  : 
Inputs       : None
Outputs      : None
Notes        : 
Revision     : 
********************************************************************************************************/
#define div1 0.5
void setMotor(u16 pwm1, u16 pwm2, u16 pwm3, u16 pwm4)
{
  TIM_SetCompare1(TIM4, pwm1*div1);
  TIM_SetCompare2(TIM4, pwm2*div1);
  TIM_SetCompare3(TIM4, pwm3*div1);
  TIM_SetCompare4(TIM4, pwm4*div1);
}

float constrain_float(float value, float max, float min)
{
  if(value > max)
  {
    value = max;
  }else if(value < min)
  {
    value = min;
  }
  return value;
}

//short constrain_short(short value, short max, short min)
//{
//  if(value > max)
//  {
//    value = max;
//  }else if(value < min)
//  {
//    value = min;
//  }
//  return value;
//}

#define Contrain_Height_I 11000
#define Contrain_Height 400
#define Contrain_Gravity 8000
#define oil_min 1000
float sonar1,sonar2;
void appPositionCtl(void *parg)
{
  OS_ERR err;
  CPU_TS ts;
  
  while(DEF_ON)
  {
    OSTaskSemPend(0, OS_OPT_PEND_BLOCKING, &ts, &err);
   
    
  }
}


#define Contrain_Attitude_I 300
#define Contrain_Attitude 1000
#define Contrain_Attitude_Velocity_I 300
#define Contrain_Attitude_Velocity 200
void appMainCycle(void *parg)
{
  OS_ERR err;
  CPU_TS ts;
  float uValue;
  float error;
  float yawTempError;
  float attitudeI[3], attitudeVelocityI[3];
  float attitudeErrLast[3], attitudeVelocityErrLast[3];
//			 uavData.uavStatue = UAV_STATUE_IDLE;
//	OSTaskSuspend(&appData.appMainCycleTCB, &err);
//  float yawControlDelay = 0.7f;//偏航角的控制延时平滑
  setMotor(oil_min, oil_min, oil_min, oil_min);
//    setMotor(1400, 1400, 1400, 1400);
//		OSTimeDly(4000, OS_OPT_TIME_DLY, &err);
//		setMotor(1000, 1000, 1000, 1000);
//  //OSTimeDly(500, OS_OPT_TIME_DLY, &err);
  while(uavData.uavStatue == 0)
  {
    OSTimeDly(100,OS_OPT_TIME_DLY, &err);
  }
   while(DEF_ON)
  {
    OSTaskSemPend(0, OS_OPT_PEND_BLOCKING, &ts, &err);
    //姿态模式
    if(uavData.uavStatue == UAV_STATUE_ATTITUDE)
    {
      if( uavData.receriverValue[5] <= 1400 )
      {
        uavData.uavStatue = UAV_STATUE_IDLE;
      }else if(uavData.receriverValue[4] > 1200)
      {
        uavData.uavStatue = UAV_STATUE_ALTITUDE;
      }
    }
    //定高模式
    else if(uavData.uavStatue == UAV_STATUE_ALTITUDE)
    {
      if( uavData.receriverValue[5] <= 1400 )
      {
        uavData.uavStatue = UAV_STATUE_IDLE;
      }else
      {
        if(uavData.receriverValue[4] > 1600)
        {
          uavData.uavStatue = UAV_STATUE_POSITION;
        }else if(uavData.receriverValue[4] < 1200)
        {
          uavData.uavStatue = UAV_STATUE_ATTITUDE;
        }
      }
    }
    //位置模式
    else if(uavData.uavStatue == UAV_STATUE_POSITION)
    {
      if( uavData.receriverValue[5] <= 1400 )
      {
        uavData.uavStatue = UAV_STATUE_IDLE;
      }else
      {
        if(uavData.receriverValue[4] < 1200)
        {
          uavData.uavStatue = UAV_STATUE_ATTITUDE;
        }else if(uavData.receriverValue[4] < 1600)
        {
          uavData.uavStatue = UAV_STATUE_ALTITUDE;
        }
      }
    }
    //关闭油门
    else if(uavData.uavStatue == UAV_STATUE_IDLE)
    {
      if( uavData.receriverValue[5] > 1400 )
      {
        uavData.uavStatue = UAV_STATUE_ATTITUDE;
      }
    }
    
    
    //状态实现
    if(uavData.uavStatue == UAV_STATUE_WAIT)
    {
          setMotor(oil_min,oil_min,oil_min,oil_min);
    }
    else if(uavData.uavStatue == UAV_STATUE_IDLE)
    {
      uavData.motorPwm[0] =oil_min;
      uavData.motorPwm[1] =oil_min;
      uavData.motorPwm[2] =oil_min;
      uavData.motorPwm[3] =oil_min;
      setMotor(uavData.motorPwm[0], uavData.motorPwm[1], uavData.motorPwm[2], uavData.motorPwm[3]);
      uavData.refAngle[2] = imuData.angle[2];
//      attitudeStateClear();
    }else if( (uavData.uavStatue == UAV_STATUE_ATTITUDE) || (uavData.uavStatue == UAV_STATUE_ALTITUDE) || (uavData.uavStatue == UAV_STATUE_POSITION))
    {
      //读取摇杆数据,姿态环或高度环模式下，摇杆直接控制姿态
      if((uavData.uavStatue == UAV_STATUE_ATTITUDE) || (uavData.uavStatue == UAV_STATUE_ALTITUDE))
      {
        uavData.refAngle[0] = (uavData.receriverValue[0] - 1016) * (0.05f);
        uavData.refAngle[1] = (uavData.receriverValue[1] - 1024) * (0.05f);
      }
      
      if((uavData.uavStatue == UAV_STATUE_ATTITUDE) || (uavData.uavStatue == UAV_STATUE_ALTITUDE))
      {
        if(DEF_ABS(uavData.receriverValue[3] - 1054) > 30)
        {
          uavData.refAngle[2] = uavData.refAngle[2] - (uavData.receriverValue[3] - 1054) * 0.002f;
        

				}
      }
      
      if(uavData.receriverValue[2]<1100)
      {
        uavData.motorPwm[0] = oil_min;
        uavData.motorPwm[1] = oil_min;
        uavData.motorPwm[2] = oil_min;
        uavData.motorPwm[3] = oil_min;
        setMotor(uavData.motorPwm[0], uavData.motorPwm[1], uavData.motorPwm[2], uavData.motorPwm[3]);
        //积分清零
        attitudeI[0] = 0;
        attitudeI[1] = 0;
        attitudeI[2] = 0;;
        attitudeVelocityI[0] = 0;
        attitudeVelocityI[1] = 0;
        attitudeVelocityI[2] = 0;
        uavData.specialAngle2[0] = attitudeI[0];
        uavData.specialAngle2[1] = attitudeI[1];
        uavData.specialAngle2[2] = attitudeI[2];
        uavData.specialAngle1[0] = attitudeVelocityI[0];
        uavData.specialAngle1[1] = attitudeVelocityI[1];
        uavData.specialAngle1[2] = attitudeVelocityI[2];
//        attitudeStateClear();
      }
      else if(uavData.receriverValue[2]>1100)
      {
        
        
        /*
        --------------------
          *姿态环PID
        ---------------------
        */
				
				        //ROLL
        error = uavData.refAngle[0] - imuData.angle[0];
        attitudeI[0] += error;
        attitudeI[0] = constrain_float(attitudeI[0], Contrain_Attitude_I, -Contrain_Attitude_I);
        uValue = uavData.anglePid[0][0] *  error + uavData.anglePid[0][1] * attitudeI[0]*0.1f + uavData.anglePid[0][2] * ( error - attitudeErrLast[0] );
        uavData.refAngleVelocity[0] = constrain_float(uValue, Contrain_Attitude, -Contrain_Attitude) * 0.1f;
        attitudeErrLast[0] = error;
        uavData.specialAngle2[0] = attitudeI[0];

				        //PITCH
        error = uavData.refAngle[1] - imuData.angle[1];
        attitudeI[1] += error;
        attitudeI[1] = constrain_float(attitudeI[1], Contrain_Attitude_I, -Contrain_Attitude_I);
        uValue = uavData.anglePid[1][0] *  error + uavData.anglePid[1][1] * attitudeI[1]*0.1f + uavData.anglePid[1][2] * ( error - attitudeErrLast[1] );
        uavData.refAngleVelocity[1] = constrain_float(uValue, Contrain_Attitude, -Contrain_Attitude) * 0.1f;
        attitudeErrLast[1] = error;
        uavData.specialAngle2[1] = attitudeI[1];
				

        //YAW
        error = uavData.refAngle[2] - imuData.angle[2];
        yawTempError = constrain_float(error,10, -10);
        if( DEF_ABS(yawTempError) >= DEF_ABS(attitudeErrLast[2]) )
        {
          attitudeI[2] += yawTempError;
        }
        attitudeI[2] = constrain_float(attitudeI[2], Contrain_Attitude_I , -Contrain_Attitude_I);
        uValue = uavData.anglePid[2][0] *  yawTempError + uavData.anglePid[2][1] * attitudeI[2]*0.1f + uavData.anglePid[2][2] * ( yawTempError - attitudeErrLast[2] );
        //uValue = uavData.refAngleVelocity[2]*(1-yawControlDelay) + uValue * yawControlDelay;
        uavData.refAngleVelocity[2] = constrain_float(uValue, Contrain_Attitude, -Contrain_Attitude) * 0.1f;
        attitudeErrLast[2] = yawTempError;
        uavData.specialAngle2[2] = attitudeI[2];
//              
        /*
        --------------------
          *角速度环PID
        ---------------------
        */
        //ROLL VELOCITY
        error = uavData.refAngleVelocity[0] - imuData.angleV[0];
        attitudeVelocityI[0] += error;
        attitudeVelocityI[0] = constrain_float(attitudeVelocityI[0], Contrain_Attitude_Velocity_I, -Contrain_Attitude_Velocity_I);
        uValue = uavData.angleVelocityPid[0][0] *  error + uavData.angleVelocityPid[0][1] * attitudeVelocityI[0]*0.1f + uavData.angleVelocityPid[0][2] * ( error - attitudeVelocityErrLast[0] );
        uavData.controlValue[0] = constrain_float(uValue, Contrain_Attitude_Velocity, -Contrain_Attitude_Velocity);
        attitudeVelocityErrLast[0] = error;
        uavData.specialAngle1[0] = attitudeVelocityI[0];
        //PITCH VELOCITY
        error = uavData.refAngleVelocity[1] - imuData.angleV[1];
        attitudeVelocityI[1] += error;
        attitudeVelocityI[1] = constrain_float(attitudeVelocityI[1], Contrain_Attitude_Velocity_I, -Contrain_Attitude_Velocity_I);
        uValue = uavData.angleVelocityPid[1][0] *  error + uavData.angleVelocityPid[1][1] * attitudeVelocityI[1]*0.1f + uavData.angleVelocityPid[1][2] * ( error - attitudeVelocityErrLast[1] );
        uavData.controlValue[1] = constrain_float(uValue, Contrain_Attitude_Velocity, -Contrain_Attitude_Velocity);
        attitudeVelocityErrLast[1] = error;
        uavData.specialAngle1[1] = attitudeVelocityI[1];
        //YAW VELOCITY
        error = uavData.refAngleVelocity[2] - imuData.angleV[2];
        if( DEF_ABS(error) > DEF_ABS(attitudeVelocityErrLast[2]) )
        {
          attitudeVelocityI[2] += error;
        }
        attitudeVelocityI[2] = constrain_float(attitudeVelocityI[2], Contrain_Attitude_Velocity_I, -Contrain_Attitude_Velocity_I);
        uValue = uavData.angleVelocityPid[2][0] *  error + uavData.angleVelocityPid[2][1] * attitudeVelocityI[2] + uavData.angleVelocityPid[2][2] * ( error - attitudeVelocityErrLast[2] );
        uavData.controlValue[2] = constrain_float(uValue, Contrain_Attitude_Velocity, -Contrain_Attitude_Velocity);
        attitudeVelocityErrLast[2] = error;
        uavData.specialAngle1[2] = attitudeVelocityI[2];
        
        //油门
        uavData.controlValue[3] = uavData.receriverValue[2];
        
        
        uavData.specialAngle2[1] = uavData.velocity[2];
        
        
        //控制量分配
        uavData.motorPwm[0] = uavData.controlValue[3] - uavData.controlValue[0] + uavData.controlValue[1] + uavData.controlValue[2];
        uavData.motorPwm[1] = uavData.controlValue[3] - uavData.controlValue[0] - uavData.controlValue[1] - uavData.controlValue[2];
        uavData.motorPwm[2] = uavData.controlValue[3] + uavData.controlValue[0] - uavData.controlValue[1] + uavData.controlValue[2];
        uavData.motorPwm[3] = uavData.controlValue[3] + uavData.controlValue[0] + uavData.controlValue[1] - uavData.controlValue[2];
        
//        if(uavData.controlValue[3]  < 1200)
//        {
//          uavData.controlValue[3] = 1200;
        
        if(uavData.controlValue[3] > 1550)
        {
            uavData.controlValue[3] = 1550;
        }
        if(uavData.uavStatue == UAV_STATUE_ATTITUDE)
        {
          if(uavData.controlValue[3] > 1500)
        {
            uavData.controlValue[3] = 1500;
        }
        }
//        if(uavData.uavDubugMode == UAV_MODE_DEBUG_NOMOTOR)
//        {
//          setMotor(uavData.motorPwm[0], uavData.motorPwm[1], uavData.motorPwm[2], uavData.motorPwm[3]);
//        }else
//        {
//          setMotor(uavData.motorPwm[0], uavData.motorPwm[1], uavData.motorPwm[2], uavData.motorPwm[3]);
//        }
        setMotor(uavData.motorPwm[0], uavData.motorPwm[1], uavData.motorPwm[2], uavData.motorPwm[3]);
        //setMotor(uavData.receriverValue[2],uavData.receriverValue[2],uavData.receriverValue[2],uavData.receriverValue[2]);
       //setMotor(800,800,800,800);
      }
      
    }
    //未考虑的无人机状态
    else
    {
      uavData.motorPwm[0] = oil_min;
      uavData.motorPwm[1] = oil_min;
      uavData.motorPwm[2] = oil_min;
      uavData.motorPwm[3] = oil_min;
      setMotor(uavData.motorPwm[0], uavData.motorPwm[1], uavData.motorPwm[2], uavData.motorPwm[3]);
    }
    
  }
}

#define sigmoid_alpha 0.6f
/********************************************************************************************************
Function Name: RoughSigmoid
Author       : xiaobo
Date         : 2016-08-12
Description  : sigmoid函数，将0-20的数据压缩到0-1。。。
Inputs       : None
Outputs      : None
Notes        : 
Revision     : 
********************************************************************************************************/
float RoughSigmoid(float xValue)
{
  float x,xx,e,out;
  if(xValue < 0)
  {
    xValue = 0;
  }
  xValue = (xValue-10)*sigmoid_alpha;
  x = DEF_ABS(xValue);
  xx = x*x;
  e = 1 + x + xx*0.555f + xx*xx*0.143f;
  if(xValue > 0)
  {
    out = 1/e;
  }else
  {
    out = e;
  }
  out = 1/(1+out);
  return out;
}

#define PosSlowFun_alpha1 1.0f
#define PosSlowFun_alpha2 0.5f
#define PosSlowFun_alpha3 0.2f
#define PosSlowFun_alpha4 0.1f
/********************************************************************************************************
Function Name: PosSlowFun
Author       : xiaobo
Date         : 2016-08-13
Description  : 位置增益削减函数
Inputs       : None
Outputs      : None
Notes        : 0-200 -> 0-100
Revision     : 
********************************************************************************************************/
float PosSlowFun(float xValue)
{
  float out;
  if (xValue<50)
  {
    out = xValue*PosSlowFun_alpha1;
  }
  else if (xValue<100)
  {
    out = 50*PosSlowFun_alpha1 + (xValue-50)*PosSlowFun_alpha2;
  }
  else if (xValue<150)
  {
    out = 50*PosSlowFun_alpha1 + 50*PosSlowFun_alpha2 + (xValue-100)*PosSlowFun_alpha3;
  }
  else
  {
    out = 50*PosSlowFun_alpha1 + 50*PosSlowFun_alpha2 + 50*PosSlowFun_alpha3 + (xValue-150)*PosSlowFun_alpha4;
  }
  return out;
}
//===========================================  End Of File  ===========================================//
