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
/*-----------------------------------------  I N C L U D E S  -----------------------------------------*/
#include "at25320.h"
/*---------------------------------------  D E F I N I T I O N  ---------------------------------------*/
#define spiSwapData(data) fun_spiSwap(AT25320_SPI, data);

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
static u8 at25320_swapByte(u8 data)
{
  u8 result;
  AT25320_CS = 0;
  fun_delayUs(1);
  result = spiSwapData(data);
  //fun_delayUs(1);
  AT25320_CS = 1;
  fun_delayUs(1);
  return result;
}

void at25320_WREN(void)
{
  AT25320_CS = 0;
  fun_delayUs(1);
  at25320_swapByte(0x06);
  AT25320_CS = 1;
}

void at25320_WRDI(void)
{
  AT25320_CS = 0;
  fun_delayUs(1);
  at25320_swapByte(0x04);
  AT25320_CS = 1;
}

void at25320_writeBuffer(u16 addr, u8* buffer, u16 number)
{
  AT25320_CS = 0;
  fun_delayUs(3);
  spiSwapData(0x02);//commond
  spiSwapData(addr>>8);
  spiSwapData(addr&0xff);
  while(number--)
  {
    spiSwapData(*buffer++);
  }    
  //fun_delayUs(1);
  AT25320_CS = 1;
}

void at25320_readBuffer(u16 addr, u8* buffer, u16 number)
{
  AT25320_CS = 0;
  fun_delayUs(3);
  spiSwapData(0x03);//commond
  spiSwapData(addr>>8);
  spiSwapData(addr&0xff);
  while(number--)
  {
    *buffer++ = spiSwapData(0xff);
  }    
  //fun_delayUs(1);
  AT25320_CS = 1;
}


//===========================================  End Of File  ===========================================//
