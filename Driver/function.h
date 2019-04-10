/********************************************************************************************************
**                                 Copyright (c)          MEI14
**                                                 All Rights Reserved
**  
**                                 Email:729527658@qq.com   QQ:729527658
**-------------------------------------------------------------------------------------------------------
**  MCU        : STM32F407VG (STMicroelectronics)
**  Compiler   : Keil uVision 5.10
**  Module Name: 
**  Module Date: 2014-
**  Module Auth: 
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
#ifndef _FUNCTION_H
#define _FUNCTION_H

/*-----------------------------------------  I N C L U D E S  -----------------------------------------*/
#include "Global.h"

/*---------------------------------------  D E F I N I T I O N  ---------------------------------------*/


/*-------------------------------------------  M A C R O S  -------------------------------------------*/
 /*  中断开头模板  */
    /* CPU状态变量  */ 
    /*  保存CPU状态并进入临界段  */
    /*  优先级处理  */
    /*  退出临界段  */
#define FUN_ISR_HEADER()  { CPU_SR_ALLOC(); CPU_CRITICAL_ENTER(); OSIntEnter(); CPU_CRITICAL_EXIT();}
 /*  中断结束模板  */
#define FUN_ISR_FOOT()    { OSIntExit(); }
/*--------------------------------------  D E C L A R A T I O N  --------------------------------------*/
/* Internal Variable */

/* External Variable */
//extern FUN_DATA funData;
/* Internal Function */

/* External Function */
int fun_printf(USART_TypeDef* com, char * plog,...);
void   fun_usartSend(USART_TypeDef *com ,u8 data);
u8     fun_usartRec(USART_TypeDef *com);
//OS_ERR fun_init(void);
u16 fun_spiSwap(SPI_TypeDef* spix,u16 data);
OS_ERR fun_createTaskSimple(OS_TCB* tcb, CPU_CHAR* taskName,OS_TASK_PTR task, OS_PRIO prio, CPU_STK* stk);
void fun_nvicSetNormal(u8 chanel, u8 mPrio, u8 subPrio);
void fun_delayUs(u32 us);
u32 fun_delayMs(u32 ms);
#endif
//===========================================  End Of File  ===========================================//
