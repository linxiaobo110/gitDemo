/********************************************************************************************************
**                                 Copyright (c)          MEI14
**                                                 All Rights Reserved
**  
**                                 Email:729527658@qq.com   QQ:729527658
**-------------------------------------------------------------------------------------------------------
**  MCU        : STM32F407VG (STMicroelectronics)
**  Compiler   : Keil uVision 5.10
**  Module Name: common functions
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
/*-----------------------------------------  I N C L U D E S  -----------------------------------------*/
#include "function.h"
/*---------------------------------------  D E F I N I T I O N  ---------------------------------------*/
//static OS_SEM printfSem;
//FUN_DATA funData;
/*----------------------------------------  F U N C T I O N S  ----------------------------------------*/
/********************************************************************************************************
Function Name: 
Author       : 
Date         : 2013-03-
Description  : 
Inputs       : None
Outputs      : None
Notes        : 
Revision     : 
********************************************************************************************************/


/********************************************************************************************************
Function Name: 
Author       : 
Date         : 2013-03-
Description  : 
Inputs       : None
Outputs      : None
Notes        : 
Revision     : 
********************************************************************************************************/
int fun_printf(USART_TypeDef* com, char * plog,...)
{
  int nret = 0;
  va_list args;
  char data[30];
  char *p;
  if (plog == NULL) {
    return nret;
  }	
  va_start(args, plog);
  nret = vsprintf(data,plog,args);
  p = data;
  
  while(*p)
  {
    fun_usartSend(com, *p++);

    //USART_SendData(USART1, *p++);
  } 
  va_end(args);
  return nret;
}
/********************************************************************************************************
Function Name: 
Author       : 
Date         : 2013-03-
Description  : 
Inputs       : None
Outputs      : None
Notes        : 
Revision     : 
********************************************************************************************************/
void fun_usartSend(USART_TypeDef *com ,u8 data)
{
  while(USART_GetFlagStatus(com, USART_FLAG_TC) == RESET)
  {}
  USART_SendData(com, (u8) data);
}

/********************************************************************************************************
Function Name: 
Author       : 
Date         : 2013-03-
Description  : 
Inputs       : None
Outputs      : None
Notes        : 
Revision     : 
********************************************************************************************************/
void fun_nvicSetNormal(u8 chanel, u8 mPrio, u8 subPrio)
{
  NVIC_InitTypeDef nvicInitStructure;
  nvicInitStructure.NVIC_IRQChannel = chanel;
  nvicInitStructure.NVIC_IRQChannelPreemptionPriority = mPrio;
  nvicInitStructure.NVIC_IRQChannelSubPriority = subPrio;
  nvicInitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&nvicInitStructure);
}


/********************************************************************************************************
Function Name: 
Author       : 
Date         : 2013-03-
Description  : 
Inputs       : None
Outputs      : None
Notes        : 
Revision     : 
********************************************************************************************************/
u8 fun_usartRec(USART_TypeDef *com)
{
  while(USART_GetFlagStatus(com, USART_FLAG_RXNE) == RESET)
  {}
  return USART_ReceiveData(com);
}

/********************************************************************************************************
Function Name: 
Author       : 
Date         : 2013-03-
Description  : 
Inputs       : None
Outputs      : None
Notes        : 
Revision     : 
********************************************************************************************************/
u16 fun_spiSwap(SPI_TypeDef* spix,u16 data)
{
  /* Loop while DR register in not emplty */
  while (SPI_I2S_GetFlagStatus(spix, SPI_I2S_FLAG_TXE) == RESET)      //?????????
  {

  }			  
  /* Send byte through the SPI1 peripheral */
  SPI_I2S_SendData(spix, data);                                    //????SPI1??????

  /* Wait to receive a byte */
  while (SPI_I2S_GetFlagStatus(spix, SPI_I2S_FLAG_RXNE) == RESET)   //??????????
  {

  }	  						    
  /* Return the byte read from the SPI bus */
  return SPI_I2S_ReceiveData(spix);                  
}
/********************************************************************************************************
Function Name: 
Author       : 
Date         : 2013-08-05
Description  : 
Inputs       : None
Outputs      : None
Notes        : 
Revision     : 
********************************************************************************************************/
OS_ERR fun_createTaskSimple(OS_TCB* tcb, CPU_CHAR* taskName,OS_TASK_PTR task, OS_PRIO prio, CPU_STK* stk)
{
    OS_ERR err;
    OSTaskCreate( (OS_TCB       *)tcb,
                (CPU_CHAR     *)"simple task",
                (OS_TASK_PTR   )task,
                (void         *)0u,
                (OS_PRIO       )prio,
                (CPU_STK      *)stk,
                (CPU_STK_SIZE  )APP_CFG_TASK_NORMAL_STK_SIZE/10,
                (CPU_STK_SIZE  )APP_CFG_TASK_NORMAL_STK_SIZE,
                (OS_MSG_QTY    )5u,
                (OS_TICK       )0u,
                (void         *)0u,
                (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                (OS_ERR       *)&err);
    return err;
}
#define CLK_US_NUMS  168
void fun_delayUs(u32 us)
{
  u32 ends = us*CLK_US_NUMS + BSP_REG_DWT_CYCCNT - 10;
  if(ends >  BSP_REG_DWT_CYCCNT)
  {
    while(BSP_REG_DWT_CYCCNT < ends)
      ;
  }else
  {
    //Խ�����ֵ
    while(BSP_REG_DWT_CYCCNT > ends)
      ;
    while(BSP_REG_DWT_CYCCNT < ends)
      ;
  }
  
}
/********************************************************************************************************
Function Name: 
Author       : 
Date         : 2015-03-12
Description  : 毫秒延时函数，由操作系统实现
Inputs       : None
Outputs      : None
Notes        : 
Revision     : 
********************************************************************************************************/
u32 fun_delayMs(u32 ms)
{
  OS_ERR err;
  OSTimeDlyHMSM(0,0,0,ms,OS_OPT_TIME_HMSM_NON_STRICT,&err);
  return err;
}
//===========================================  End Of File  ===========================================//
