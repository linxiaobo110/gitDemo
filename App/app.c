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
#include "app.h"
/*---------------------------------------  D E F I N I T I O N  ---------------------------------------*/
static CPU_STK appTestStk[APP_CFG_TASK_EQ_STK_SIZE];
static CPU_STK appBlackBoxStk[APP_CFG_TASK_1024_STK_SIZE];
static CPU_STK appBuzzerStk[APP_CFG_TASK_NORMAL_STK_SIZE];
static CPU_STK appSendBluetoothStk[APP_CFG_TASK_NORMAL_STK_SIZE];
static CPU_STK appWatchDogStk[APP_CFG_TASK_NORMAL_STK_SIZE];
static CPU_STK appMainCycleStk[APP_CFG_TASK_EQ_STK_SIZE];
static CPU_STK appCmdHanderStk[APP_CFG_TASK_NORMAL_STK_SIZE];
//static CPU_STK appUSART1IRQstk[APP_CFG_TASK_NORMAL_STK_SIZE];
static CPU_STK appPositionCtlStk[APP_CFG_TASK_NORMAL_STK_SIZE];
static CPU_STK appRunTestStk[APP_CFG_TASK_NORMAL_STK_SIZE];
static CPU_STK appCorrectingModestk[APP_CFG_TASK_NORMAL_STK_SIZE];
u8 byte8PartionStorage[50][8];
APP_DATA appData;
UAV_DATA uavData;
WATCHDOG_DATA dogData;
IMU_DATA imuData;

//FATFS fs;
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
void appTaskStart(void *p_arg)
{
  CPU_INT32U cpu_clk_freq;
  CPU_INT32U cnts;
  OS_ERR err;
  CPU_SR_ALLOC();
  (void)p_arg;
  //************************************************************
  // 初始化阶段
  //************************************************************
  /* 初始化板级支持包 */
  BSP_init();
  //f_mount(&fs, "0", 1);
  /* 初始化CPU硬件 */
  CPU_Init();
  /* 初始化DWT时钟 */
  CPU_TS_TmrInit();
  /* 初始化systcik的时钟 1K */
  CPU_TS_TmrFreqSet(1000);
  cpu_clk_freq = BSP_CPU_ClkFreq();
  cnts = cpu_clk_freq 
              / (CPU_INT32U)OSCfg_TickRate_Hz;
  OS_CPU_SysTickInit(cnts);//初始化systick
  CPU_CRITICAL_ENTER();
  CPU_CRITICAL_EXIT();

  //************************************************************
  // 定义系统成员
  //************************************************************
  /* 信号量 */
  OSSemCreate(&appData.imuUpdate, "imu update", 0, &err);
  OSSemCreate(&appData.tfKardDmaTc, "tf dma TC", 0, &err);
  OSSemCreate(&appData.monitorFlag,"monitor update", 1, &err);
  
  if(err != OS_ERR_NONE)
  {
    LEDR = 0;
  }
//  OSSemCreate(&appData.testSem1, "testSem1", 0, &err);
//	OSSemCreate(&appData.testSem2, "testSem2", 0, &err);
  /* 定义标志组 */
//  OSFlagCreate(&appData.testFlagGrp,
//               "test flagGrp",
//               0,
//               &err);
  /* 初始化存储控制 */
//  Mem_Init();
  OSMemCreate(&appData.byte8Mem, "byte 8 partition", &byte8PartionStorage[0][0], 50, 8, &err);
  
  //************************************************************
  // 如果有统计任务则开启统计任务
  //************************************************************
  #if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&err);
  #endif
  
  #ifdef CPU_CFG_INT_MEAS_EN
    CPU_IntDisMeasMaxCurReset();
  #endif
  
  LEDR = 1;
  LEDB = 1;
  LEDG = 1;
  LED2 = 1;
  //************************************************************
  // 新建任务
  //************************************************************
  //测试任务
  OSTaskCreate( (OS_TCB       *)&appData.appTestTCB,
              (CPU_CHAR     *)"app test",
              (OS_TASK_PTR   )appTest,
              (void         *)0u,
              (OS_PRIO       )53,
              (CPU_STK      *)&appTestStk[0u],
              (CPU_STK_SIZE  )APP_CFG_TASK_EQ_STK_SIZE/10,
              (CPU_STK_SIZE  )APP_CFG_TASK_EQ_STK_SIZE,
              (OS_MSG_QTY    )10u,
              (OS_TICK       )0u,
              (void         *)0u,
              (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
              (OS_ERR       *)&err);
  //err = fun_createTaskSimple(&appData.appTestTCB, "app test", appTest, 53, &appTestStk[0]);
  if(err != OS_ERR_NONE)
  {
    LEDR = 0;
  }
  
  OSTaskCreate( (OS_TCB       *)&appData.appBlackBoxTCB,
              (CPU_CHAR     *)"black box",
              (OS_TASK_PTR   )appBlackBox,
              (void         *)0u,
              (OS_PRIO       )54,
              (CPU_STK      *)&appBlackBoxStk[0u],
              (CPU_STK_SIZE  )APP_CFG_TASK_1024_STK_SIZE/10,
              (CPU_STK_SIZE  )APP_CFG_TASK_1024_STK_SIZE,
              (OS_MSG_QTY    )5u,
              (OS_TICK       )0u,
              (void         *)0u,
              (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
              (OS_ERR       *)&err);
  //err = fun_createTaskSimple(&appData.appTestTCB, "app test", appTest, 53, &appTestStk[0]);
	if(err != OS_ERR_NONE)
  {
    LEDR = 0;
  }
  
  OSTaskCreate( (OS_TCB       *)&appData.appRunTestTCB,
              (CPU_CHAR     *)"appRunTest",
              (OS_TASK_PTR   )appRunTest,
              (void         *)0u,
              (OS_PRIO       )55,
              (CPU_STK      *)&appRunTestStk[0u],
              (CPU_STK_SIZE  )APP_CFG_TASK_NORMAL_STK_SIZE/10,
              (CPU_STK_SIZE  )APP_CFG_TASK_NORMAL_STK_SIZE,
              (OS_MSG_QTY    )5u,
              (OS_TICK       )0u,
              (void         *)0u,
              (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
              (OS_ERR       *)&err);
  if(err != OS_ERR_NONE)
  {
    LEDR = 0;
  }
  
  err = fun_createTaskSimple(&appData.appBuzzerTCB, "buzzer", appBuzzer, 50, &appBuzzerStk[0]);
	
	
	//run test
//	  if(err != OS_ERR_NONE)
//  {
//    LEDR = 0;
//  }
//  
//  err = fun_createTaskSimple(&appData.appRunTestTCB, "run test", appRunTest, 51, &appRunTestStk[0]);
	
	
  if(err != OS_ERR_NONE)
  {
    LEDR = 0;
  }
  
  err = fun_createTaskSimple(&appData.appSendBluetoothTCB, "bluetooth send", appSendBluetooth, 52, &appSendBluetoothStk[0]);
  if(err != OS_ERR_NONE)
  {
    LEDR = 0;
  }
  
  err = fun_createTaskSimple(&appData.appWatchDog, "watch dog", appWatchDog, 15, &appWatchDogStk[0]);
  if(err != OS_ERR_NONE)
  {
    LEDR = 0;
  }
  
  //控制循环
  OSTaskCreate( (OS_TCB       *)&appData.appMainCycleTCB,
              (CPU_CHAR     *)"Main Cycle",
              (OS_TASK_PTR   )appMainCycle,
              (void         *)0u,
              (OS_PRIO       )10,
              (CPU_STK      *)&appMainCycleStk[0u],
              (CPU_STK_SIZE  )APP_CFG_TASK_EQ_STK_SIZE/10,
              (CPU_STK_SIZE  )APP_CFG_TASK_EQ_STK_SIZE,
              (OS_MSG_QTY    )10u,
              (OS_TICK       )0u,
              (void         *)0u,
              (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
              (OS_ERR       *)&err);
  if(err != OS_ERR_NONE)
  {
    LEDR = 0;
  }
  
  //位置环控制
  err = fun_createTaskSimple(&appData.appPositionCtlTCB, "position control", appPositionCtl, 12, &appPositionCtlStk[0]);
  if(err != OS_ERR_NONE)
  {
    LEDR = 0;
  }
  
  
  //指令处理器
  err = fun_createTaskSimple(&appData.appCmdHanderTCB, "cmd hander", appCmdHander, 51, &appCmdHanderStk[0]);

  if(err != OS_ERR_NONE)
  {
    LEDR = 0;
  }
	//校正模式
	  err = fun_createTaskSimple(&appData.appCorrectingModeTCB, "Correcting Mode", appCorrectingMode, 55, &appCorrectingModestk[0]);

  if(err != OS_ERR_NONE)
  {
    LEDR = 0;
  }
  
  OSTaskChangePrio(&appTaskStartTCB, 60, &err);
  appData.err = err;
  //OSTaskSuspend(&appTaskStartTCB, &err);
//	OSTaskSuspend(&appData.appMainCycleTCB, &err);
  while(DEF_ON)
  {
		 uavData.uavStatue = UAV_STATUE_IDLE;
//		    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)

////      USART_ClearITPendingBit(USART1, USART_IT_RXNE);
////      OSTaskSemPost(&appData.appRunTestTCB,OS_OPT_POST_NONE,&err);
//			LED2=0;
//OSTimeDlyHMSM(0, 0, 0, 1000, OS_OPT_TIME_HMSM_NON_STRICT, &err);
    LED2 = 0;
    OSTimeDlyHMSM(0, 0, 0, 1000, OS_OPT_TIME_HMSM_NON_STRICT, &err);
    LED2 = 1;
    OSTimeDlyHMSM(0, 0, 0, 1000, OS_OPT_TIME_HMSM_NON_STRICT, &err);
    switch(appData.stkTestSelect)
    {
      case 0:
        appData.testStkFree = 0;
        appData.testStkUsed = 0;
     
        break;
      case 1:
        OSTaskStkChk(&appData.appTestTCB, &appData.testStkFree, &appData.testStkUsed,&err);
        break;
      case 2:
        OSTaskStkChk(&appData.appMainCycleTCB, &appData.testStkFree, &appData.testStkUsed,&err);
        break;
      case 3:
        OSTaskStkChk(&appData.appPositionCtlTCB, &appData.testStkFree, &appData.testStkUsed,&err);
        break;
      case 4:
        OSTaskStkChk(&appData.appSendBluetoothTCB, &appData.testStkFree, &appData.testStkUsed,&err);
        break;
      case 5:
        OSTaskStkChk(&appData.appCmdHanderTCB, &appData.testStkFree, &appData.testStkUsed,&err);
        break;
      case 6:
        OSTaskStkChk(&appData.appBlackBoxTCB , &appData.testStkFree, &appData.testStkUsed,&err);
        break;
			case 7:
        OSTaskStkChk(&appData.appRunTestTCB , &appData.testStkFree, &appData.testStkUsed,&err);
        break;
//			case 8:
//        OSTaskStkChk(&appData.appUSART1IRQTCB , &appData.testStkFree, &appData.testStkUsed,&err);
//        break;
    }
    
  }
}

void appRunTest(void *parg)
{
  OS_ERR err;
  CPU_TS ts;
//			setMotor(800, 800, 800, 800);
//		OSTimeDly(4000, OS_OPT_TIME_DLY, &err);
//    setMotor(1400, 1400, 1400, 1400);
//		OSTimeDly(4000, OS_OPT_TIME_DLY, &err);
//		setMotor(1000, 1000, 1000, 1000);
////		OSTimeDly(4000, OS_OPT_TIME_DLY, &err);
  while(DEF_ON)
  {
//		OSTaskSuspend(&appTaskStartTCB, &err);
    OSTaskSemPend(0, OS_OPT_PEND_BLOCKING,  &ts, &err);
//		setMotor(800, 800, 800, 800);
		OSTimeDly(4000, OS_OPT_TIME_DLY, &err);
    setMotor(1200, 1200, 1200, 1200);
		OSTimeDly(4000, OS_OPT_TIME_DLY, &err);
		setMotor(1001, 1001, 1000, 1200);
		OSTimeDly(4000, OS_OPT_TIME_DLY, &err);
				setMotor(1001, 1001, 1200, 1001);
		OSTimeDly(4000, OS_OPT_TIME_DLY, &err);
				setMotor(1001, 1200, 1000, 1001);
		OSTimeDly(4000, OS_OPT_TIME_DLY, &err);
				setMotor(1200, 1001, 1001, 1001);
		OSTimeDly(4000, OS_OPT_TIME_DLY, &err);
//				setMotor(800, 800, 1200, 800);
//		OSTimeDly(4000, OS_OPT_TIME_DLY, &err);
//				setMotor(800, 800, 800, 1200);


//		setMotor(1200, 800, 800, 1200);
//		OSTimeDly(5000, OS_OPT_TIME_DLY, &err);
//		
//		
//				setMotor(800, 800, 800, 800);
//		OSTimeDly(4000, OS_OPT_TIME_DLY, &err);
//		setMotor(800, 800, 1300, 800);
//		OSTimeDly(5000, OS_OPT_TIME_DLY, &err);
//				setMotor(800, 800, 800, 800);
//		OSTimeDly(4000, OS_OPT_TIME_DLY, &err);
//		setMotor(800, 1300, 800, 800);
//		OSTimeDly(5000, OS_OPT_TIME_DLY, &err);
//				setMotor(800, 800, 800, 800);
//		OSTimeDly(4000, OS_OPT_TIME_DLY, &err);
//		setMotor(1300, 800, 800, 800);
//		OSTimeDly(5000, OS_OPT_TIME_DLY, &err);
//		setMotor(800, 800, 800, 800);
//		OSTimeDly(4000, OS_OPT_TIME_DLY, &err);


//		setMotor(800, 800, 1200, 1200);
//		OSTimeDly(4000, OS_OPT_TIME_DLY, &err);
//		setMotor(800, 800, 800, 800);
//    OSTimeDly(4000, OS_OPT_TIME_DLY, &err);
//		setMotor(800, 800, 800, 1200);
//		OSTimeDly(4000, OS_OPT_TIME_DLY, &err);
//		setMotor(800, 800, 800, 800);
  }
}
//BYTE work[1024];

static void appTest(void *p_arg) 
{
  OS_ERR err;

  //u8 saveCounter;
//  CPU_TS ts;
//  u8 i;
//  u8 testEr[10] = {4,5,6};
  LEDR = 1;
  LEDG = 1;
  LEDB = 1;
	OSTimeDly(1000, OS_OPT_TIME_DLY, &err);
  
//  appData.systemStatue = 1;
//  at25320_WREN();
//  at25320_writeBuffer(100, testEr, 10);
//  at25320_WRDI();
//  at25320_readBuffer(100, testEr, 10);

  
//  OSTimeDly(500,OS_OPT_TIME_DLY, &err);
  //OSTaskStkChk(&appData.appTestTCB, &appData.testStkFree, &appData.testStkUsed,&err);
  OSTaskSuspend(&appData.appTestTCB, &err);
  
  while(DEF_ON)
  {
    //res = f_open(&fs, "1://test2.txt", FA_OPEN_APPEND|FA_WRITE);
    
    //res = f_close(&fs);
//    LEDR = 0;
//    LEDG = 1;
//    LEDB = 1;
//    fun_usartSend(USART3, 0xa5);
    
    
//    OSTimeDly(2000, OS_OPT_TIME_DLY, &err);
//    LEDR = 1;
//    LEDG = 0;
//    LEDB = 1;
//    OSTimeDlyHMSM(0, 0, 0, 1000, OS_OPT_TIME_HMSM_NON_STRICT, &err);
//    LEDR = 1;
//    LEDG = 1;
//    LEDB = 0;
//    OSSemPend(&appData.imuUpdate, 200,  OS_OPT_PEND_BLOCKING, &ts, &err);
//    if(err != OS_ERR_NONE)
//      LEDB = 0;
//    else
//      LEDB = 1;
//    OSTimeDlyHMSM(0, 0, 0, 1000, OS_OPT_TIME_HMSM_NON_STRICT, &err);
//    TIM_SetCompare1(TIM10, 250);
//    OSTimeDlyHMSM(0, 0, 0, 1000, OS_OPT_TIME_HMSM_NON_STRICT, &err);
//    TIM_SetCompare1(TIM10, 0);
//    for(i = 0;i<3;i++)
//    {
//      OSTaskQPost(&appData.appBuzzerTCB, 0, i+1, OS_OPT_POST_NONE, &err);
//      OSTimeDlyHMSM(0, 0, 0, 3000, OS_OPT_TIME_HMSM_NON_STRICT, &err);
//    }



    
  }
}

#define DEBUG_MODE
/********************************************************************************************************
Function Name: 
Author       : xiaobo
Date         : 2013-03-
Description  : 
Inputs       : None
Outputs      : None
Notes        : 
Revision     : 
**-------------------------------------------------------------------------------------------------------
**  Version    : 
**  Modified By: xiaobo
**  Date       : 04/02/19
**  Content    : add color led control
**  Notes      : 
********************************************************************************************************/
void appWatchDog(void *p_arg)
{
  OS_ERR err;
  /*
    ----------------------------------------------------------------------------
    * Led control variable
    * ledColor: 0 close, 1 red, 2, blue, 3 green, 4 yellow, 5 purple, 6 white
    * ledSpark: 0 all time, 1 one spark, 2 two spark, 3 three spark                               
    ----------------------------------------------------------------------------
  */
  LED_STATE ledColor = LED_COLOR_CLOSE;
  LED_STATE ledSpark = LED_SPARK_NO;
  //used for state implement 
  int ledCnt = 0;
  LED_STATE ledRealState = LED_COLOR_CLOSE;
  
  uavData.uavStatue = UAV_STATUE_WAIT;
  
  while(uavData.receriverValue[2] > 1100)
  {
    OSTaskQPost(&appData.appBuzzerTCB, 0, 4, OS_OPT_POST_1, &err);
    setMotor(800,800,800,800);
    OSTimeDly(1000, OS_OPT_TIME_DLY, &err);

    OSTimeDly(1000, OS_OPT_TIME_DLY, &err);
  }
  
  if(UAV_MODE_DEBUG_MOTOR == UAV_MODE_NORMAL)
  {
    uavData.uavStatue = UAV_STATUE_WAIT;
  }else
  {
    uavData.uavStatue = UAV_STATUE_IDLE;
  }
  
//  #ifdef DEBUG_MODE
//  uavData.uavStatue = UAV_STATUE_IDLE; 
//  #else
//  uavData.uavStatue = UAV_STATUE_WAIT; 
//  #endif
  uavData.imuDevice.enable = 1;
    
//  while( DEF_ON )
//  {
////    if( (dogData.receiverDog == 0x3f) &&  (dogData.imuDog > 10) )
////    {
////      break;
////    }
////    OSTaskQPost(&appData.appBuzzerTCB, 0, 4, OS_OPT_POST_1, &err);
//    LEDB = 0;
//    OSTimeDly(100, OS_OPT_TIME_DLY, &err);
//    LEDB = 1;
//    OSTimeDly(100, OS_OPT_TIME_DLY, &err);
//  }
//  uavData.imuDevice.startStatue = 1;
  uavData.uavStatue = UAV_STATUE_IDLE;
//  OSTimeDly(1500, OS_OPT_TIME_DLY, &err);
  while(DEF_ON)
  {
    /* 把接收机的安全机制加上！ */
//    if(dogData.receiverDog != 0x3f)
//    {
//      if(uavData.uavDubugMode == UAV_MODE_NORMAL)
//      {
//        uavData.uavStatue = UAV_STATUE_ERROR_NORECEIVER; 
//      }else
//      {
//        uavData.uavStatue = UAV_STATUE_IDLE; 
//      }
//      //uavData.uavStatue = 10;
//    }
    if(dogData.imuDog < 10)
    {
      uavData.uavStatue = UAV_STATUE_ERROR_DEFAULT;
      uavData.imuDevice.errStatue = 1;
    }
    if(uavData.powerVotage < 10.5f)
    {
      if(uavData.uavDubugMode == UAV_MODE_NORMAL)
      {
        uavData.uavStatue = UAV_STATUE_ERROR_POWERLOST;
      }else
      {
        uavData.uavStatue = UAV_STATUE_IDLE; 
      }
      //OSTaskQPost(&appData.appBuzzerTCB, 0, 12, OS_OPT_POST_1, &err);
    }
    
    
    
    //************************************************************
    // Color LED show the state
    //************************************************************
    switch(uavData.uavStatue)
    {
      //start
      case UAV_STATUE_WAIT:
        ledColor = LED_COLOR_WHITE;
        ledSpark = LED_SPARK_ONE;
        break;
      
      //error state
      case UAV_STATUE_ERROR_NORECEIVER:
        ledColor = LED_COLOR_RED;
        ledSpark = LED_SPARK_ONE;
        break;
      case UAV_STATUE_ERROR_POWERLOST:
        ledColor = LED_COLOR_YELLOW;
        ledSpark = LED_SPARK_ONE;
        break;
      case UAV_STATUE_ERROR_DEFAULT:
        ledColor = LED_COLOR_YELLOW;
        ledSpark = LED_SPARK_NO;
      
      //idle
      case  UAV_STATUE_IDLE:
        ledColor = LED_COLOR_BLUE;
        ledSpark = LED_SPARK_ONE;
        break;
			
			     //correct
      case  UAV_STATUE_CORRECT:
        ledColor = LED_COLOR_BLUE;
        ledSpark = LED_SPARK_ONE;
        break;
      
      //work
      case UAV_STATUE_ATTITUDE:
        ledColor = LED_COLOR_GREEN;
        ledSpark = LED_SPARK_ONE;
        break;
      case UAV_STATUE_ALTITUDE:
        ledColor = LED_COLOR_GREEN;
        ledSpark = LED_SPARK_TWO;
        break;
      case UAV_STATUE_POSITION:
        ledColor = LED_COLOR_GREEN;
        ledSpark = LED_SPARK_THREE;
        break;
    }
    
    ledCnt++;
    
    //close first
    if( (ledCnt == 2) || (ledCnt == 5) || (ledCnt == 8))
    {
      ledRealState = LED_COLOR_CLOSE;
    }
    
    //turn on color led according to led spark state
    switch(ledSpark)
    {
      case LED_SPARK_NO:
        ledRealState = ledColor;
        break;
      case LED_SPARK_THREE:
        ledRealState = (ledCnt==7)?ledColor:ledRealState;
      case LED_SPARK_TWO:
        ledRealState = (ledCnt==4)?ledColor:ledRealState;
      case LED_SPARK_ONE:
        ledRealState = (ledCnt==1)?ledColor:ledRealState;
        break;
      default:
        ledRealState = ledColor;
    }
    
    if(ledCnt == 10)
    {
      ledCnt = 0;
    }

    //implement the color led 
    switch(ledRealState)
    {
      case LED_COLOR_CLOSE:
        LEDR = 1;
        LEDG = 1;
        LEDB = 1;
        break;
      case LED_COLOR_RED:
        LEDR = 0;
        LEDG = 1;
        LEDB = 1;
        break;      
      case LED_COLOR_GREEN:
        LEDR = 1;
        LEDG = 0;
        LEDB = 1;
        break;      
      case LED_COLOR_BLUE:
        LEDR = 1;
        LEDG = 1;
        LEDB = 0;
        break;
      case LED_COLOR_YELLOW:
        LEDR = 0;
        LEDG = 0;
        LEDB = 1;
        break;
      case LED_COLOR_PURPLE:
        LEDR = 0;
        LEDG = 1;
        LEDB = 0;
        break;
      case LED_COLOR_WHITE:
        LEDR = 0;
        LEDG = 0;
        LEDB = 0;
        break;
      default:
        LEDR = 1;
        LEDB = 1;
        LEDG = 1;
      
    }
    
    
    
    dogData.receiverDog = 0;
    dogData.imuDog = 0;
    OSTimeDly(100, OS_OPT_TIME_DLY, &err);
  }
}


//===========================================  End Of File  ===========================================//
