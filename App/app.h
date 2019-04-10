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
#ifndef _APP_H
#define _APP_H

/*-----------------------------------------  I N C L U D E S  -----------------------------------------*/
#include "Global.h"

/*---------------------------------------  D E F I N I T I O N  ---------------------------------------*/

typedef enum LED_STATE
{
	//COLOR
  LED_COLOR_CLOSE = 0,
  LED_COLOR_RED = 1,
  LED_COLOR_BLUE = 2,
  LED_COLOR_GREEN = 3,
  LED_COLOR_YELLOW= 4,
  LED_COLOR_PURPLE = 5,
	LED_COLOR_WHITE = 6,
  
  //SPARK
	LED_SPARK_NO = 16,
	LED_SPARK_ONE = 17,
  LED_SPARK_TWO = 18,
  LED_SPARK_THREE = 19
  
}LED_STATE;

typedef enum UAV_STATUE
{
  UAV_STATUE_WAIT=0,
  UAV_STATUE_ATTITUDE=1,
  UAV_STATUE_IDLE=2,
  UAV_STATUE_ALTITUDE=3,
  UAV_STATUE_POSITION=4,
	UAV_STATUE_CORRECT=5,
  UAV_STATUE_ERROR_DEFAULT = 10,
  UAV_STATUE_ERROR_NORECEIVER = 11,
  UAV_STATUE_ERROR_POWERLOST = 12
}UAV_STATUE;

typedef enum DEVICE_WIFI_ERR
{
  WIFI_ERR_NONE=0,
  WIFI_ERR_CONROUTER,
  WIFI_ERR_CONSERVER,
  WIFI_ERR_MODESET,
  WIFI_ERR_MODESTART
}DEVICE_WIFI_ERR;

typedef enum UAV_DEBUG_MODE
{
  UAV_MODE_NORMAL=0,
  UAV_MODE_DEBUG_MOTOR=0,
  UAV_MODE_DEBUG_NOMOTOR,
  UAV_MODE_WRITE_TRACK,
}UAV_DEBUG_MODE;

typedef struct DEVICE_DATA
{
  u8 enable:1;
  u8 startStatue:1;
  u8 workStatue:2;
  u8 errStatue:4;
}DEVICE_DATA;

typedef struct APP_DATA
{
  OS_TCB appTestTCB;
  OS_TCB appBuzzerTCB;
  OS_TCB appSendBluetoothTCB;
  OS_TCB appWatchDog;
  OS_TCB appMainCycleTCB;
  OS_TCB appCmdHanderTCB;
  OS_TCB appPositionCtlTCB;
  OS_TCB appBlackBoxTCB;
	OS_TCB appRunTestTCB;
		OS_TCB appCorrectingModeTCB;
	OS_TCB appUSART1IRQTCB;
  OS_ERR err;
  u8 systemStatue;
  OS_SEM imuUpdate;
  OS_MEM byte8Mem;
  OS_SEM tfKardDmaTc;
  CPU_STK_SIZE testStkUsed;
  CPU_STK_SIZE testStkFree;
  OS_SEM monitorFlag;
  u8 stkTestSelect;
  u8 monitorHasSend;
}APP_DATA;

typedef struct UAV_DATA
{
  float powerVotage;
  u16 receriverValue[6];
  u16 motorPwm[4];
  float angleVelocityPid[3][3];
  float anglePid[3][3];
  float posPid[3][3];
  float velocityPid[3][3];
  float otherPara[20];
  
  float refAngle[3];
  float refAngleVelocity[3];
  float specialAngle1[3];
  float specialAngle2[3];
  
  float position[3];
  float velocity[3];
  float refPosition[3];
  float refVelocity[3];
  float specialPosition[3];
  float specialVelocity[3];
  
  float controlValue[4];
  float sonar2Dis;
    
  float para[3];
  UAV_STATUE uavStatue;
  u8 wifiReady;
  
  DEVICE_DATA imuDevice;
  DEVICE_DATA altitudeSensorDevice;
  DEVICE_DATA positionSensorDevice;
  DEVICE_DATA e2promDevice;
  DEVICE_DATA tfCardDevice;
  DEVICE_DATA ledBellDevice;
  DEVICE_DATA overflowDevice;
  DEVICE_DATA wifiDevice;
  
  u8 timeOrDate;
  u8 sysHour;
  u8 sysMin;
  u8 sysSecond;
	u8 roll_ax;
	u8 pitch_ax;
	u8 yaw_ax;
	u8 oil_ax;
  UAV_DEBUG_MODE uavDubugMode;
  u8 e2promTrackNums[4];
  
  
  
}UAV_DATA;

typedef struct IMU_DATA
{
  float angle[3];
  float angleV[3];
  short position[3];
  float positionV[3];
  short natnetPosition[3];
}IMU_DATA;  

typedef struct WATCHDOG_DATA
{
  u8 receiverDog;
  u16 imuDog;
}WATCHDOG_DATA;

typedef struct SONAR_TEST
{
  u16 sonarMea;
  u16 natnetMea;
  u16 heightFilter;
  s16 speedFilter1;
  s16 speedFilter2;
}SONAR_TEST;


/*-------------------------------------------  M A C R O S  -------------------------------------------*/


/*--------------------------------------  D E C L A R A T I O N  --------------------------------------*/
/* Internal Variable */

/* External Variable */
//extern OS_TCB appTaskStartTCB;
extern UAV_DATA uavData;
extern APP_DATA appData;
extern WATCHDOG_DATA dogData;
extern IMU_DATA imuData;
extern SONAR_TEST sonarTest;
/* Internal Function */
static void appTest(void *p_arg) ;
u32 crc32Calculate(u8 *buffer, u16 offset, u16 length);
void getFileNameFromNumber(int number, char *name);
float constrain_float(float value, float max, float min);
/* External Function */
void setMotor(u16 pwm1, u16 pwm2, u16 pwm3, u16 pwm4);
void appTaskStart(void *p_arg);
void appBuzzer(void *p_arg);
void appSendBluetooth(void *parg);
void appWatchDog(void *p_arg);
void appMainCycle(void *parg);
void appPositionCtl(void *parg);
void appRunTest(void *parg);
void appCmdHander(void *parg);
void appBlackBox(void *parg);
void appUSART1IRQ(void *parg);
void appCorrectingMode(void *p_arg);
float RoughSigmoid(float xValue);
float PosSlowFun(float xValue);
void displayMonitor(char *p);
#endif
//===========================================  End Of File  ===========================================//
