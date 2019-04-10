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
#include "Main.h"
/*---------------------------------------  D E F I N I T I O N  ---------------------------------------*/
OS_TCB appTaskStartTCB;
static CPU_STK appTaskStartStk[APP_CFG_TASK_START_STK_SIZE];

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

void delay_ms(u32 ms)
{
  ms = ms + bspData.systime;
  while( bspData.systime < ms)
    ;
}

//  BSP_init();
//  SysTick_Config(SystemCoreClock / 1000);
//  LED2 = 1;

//  while(1)
//  {
//    LEDR = 0;
//    LEDG = 1;
//    LEDB = 1;
//    LED2 = 0;
//    delay_ms(1000);
//    LEDR = 1;
//    LEDG = 0;
//    LEDB = 1;
//    LED2 = 1;
//    delay_ms(1000);
//    LEDR = 1;
//    LEDG = 1;
//    LEDB = 0;
//    delay_ms(1000);
//  }

int main(void)
{
  OS_ERR err;
	OSInit(&err);
  OSTaskCreate( (OS_TCB       *)&appTaskStartTCB,
                (CPU_CHAR     *)"App Task Start",
                (OS_TASK_PTR   )appTaskStart,
                (void         *)0u,
                (OS_PRIO       )APP_CFG_TASK_START_PRIO,
                (CPU_STK      *)&appTaskStartStk[0u],
                (CPU_STK_SIZE  )APP_CFG_TASK_START_STK_SIZE/10,
                (CPU_STK_SIZE  )APP_CFG_TASK_START_STK_SIZE,
                (OS_MSG_QTY    )5u,
                (OS_TICK       )0u,
                (void         *)0u,
                (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                (OS_ERR       *)&err);   
  
	OSStart(&err);
	return 0;
}


//===========================================  End Of File  ===========================================//
