/********************************************************************************************************
**                                 Copyright (c)   2014?????
**                                                 All Rights Reserved
**  
**                                 Email:729527658@qq.com   QQ:729527658
**-------------------------------------------------------------------------------------------------------
**  MCU        : STM32F407VG (STMicroelectronics)
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
#include "I2CH.h"
/*---------------------------------------  D E F I N I T I O N  ---------------------------------------*/

u8 i2cCheckEvent(I2C_TypeDef *i2c, u32 flag)
{
  u32 timeOut;
  timeOut = I2C_TIMEOUT;
  while( (!I2C_CheckEvent(i2c, flag)) && (timeOut != 0x00) )
  {
    timeOut--;
  }
  if(timeOut == 0)
  {
    LEDB = 0;
    return 1;
  }
  return 0;
}

/*----------------------------------------  F U N C T I O N S  ----------------------------------------*/
/********************************************************************************************************
Function Name: 
Author       : xiaobo
Date         : 2015-12-16
Description  : 
Inputs       : None
Outputs      : None
Notes        : 
Revision     : 
********************************************************************************************************/
u8 I2CH_readByte(I2CH_DATA i2cData, u8 regAddr)
{
//  u32 timeOut;
//  while( I2C_GetFlagStatus(i2cData.I2CX, I2C_FLAG_BUSY) && (timeOut != 0x00) )
//    timeOut--;
//  if(timeOut == 0)
//    LEDR = 0;
  //起始信号
  I2C_GenerateSTART(i2cData.I2CX, ENABLE);
  i2cCheckEvent(i2cData.I2CX, I2C_EVENT_MASTER_MODE_SELECT);
  //从机地址+写信号
  I2C_Send7bitAddress(i2cData.I2CX, i2cData.slaveAddr, I2C_Direction_Transmitter);
  i2cCheckEvent(i2cData.I2CX, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);
  /* Clear EV6 by setting again the PE bit */
  I2C_Cmd(i2cData.I2CX, ENABLE);
  //发送访问地址
  I2C_SendData(i2cData.I2CX, regAddr);
  i2cCheckEvent(i2cData.I2CX, I2C_EVENT_MASTER_BYTE_TRANSMITTED);
  //重启起始
  I2C_GenerateSTART(i2cData.I2CX, ENABLE);
  i2cCheckEvent(i2cData.I2CX, I2C_EVENT_MASTER_MODE_SELECT);
  //从机地址+读信号  
  I2C_Send7bitAddress(i2cData.I2CX, i2cData.slaveAddr, I2C_Direction_Receiver);
  i2cCheckEvent(i2cData.I2CX, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED);
  //最后一个字节应答
  I2C_AcknowledgeConfig(i2cData.I2CX, DISABLE);
  //停止信号
  I2C_GenerateSTOP(i2cData.I2CX, ENABLE);
  i2cCheckEvent(i2cData.I2CX, I2C_EVENT_MASTER_BYTE_RECEIVED);
  //读出数据
  return (I2C_ReceiveData(i2cData.I2CX));
}

/********************************************************************************************************
Function Name: 
Author       : xiaobo
Date         : 2015-12-16
Description  : 
Inputs       : None
Outputs      : None
Notes        : 
Revision     : 
********************************************************************************************************/
void I2CH_readBuffer(I2CH_DATA i2cData, u8 regAddr, u8 num, u8 *buffer) 
{
  u32 timeOut;
  u8 i;
  while( I2C_GetFlagStatus(i2cData.I2CX, I2C_FLAG_BUSY) && (timeOut != 0x00) )
  {
    timeOut--;
  }
  if(timeOut == 0)
  {
    
  }
    //LEDR = 0;
  //起始信号
  I2C_GenerateSTART(i2cData.I2CX, ENABLE);
  i2cCheckEvent(i2cData.I2CX, I2C_EVENT_MASTER_MODE_SELECT);
  //从机地址+写信号
  I2C_Send7bitAddress(i2cData.I2CX, i2cData.slaveAddr, I2C_Direction_Transmitter);
  i2cCheckEvent(i2cData.I2CX, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);
  /* Clear EV6 by setting again the PE bit */
  I2C_Cmd(i2cData.I2CX, ENABLE);
  //发送访问地址
  I2C_SendData(i2cData.I2CX, regAddr);
  i2cCheckEvent(i2cData.I2CX, I2C_EVENT_MASTER_BYTE_TRANSMITTED); 
  //重启起始
  I2C_GenerateSTART(i2cData.I2CX, ENABLE);
  i2cCheckEvent(i2cData.I2CX, I2C_EVENT_MASTER_MODE_SELECT);
  //从机地址+读信号  
  I2C_Send7bitAddress(i2cData.I2CX, i2cData.slaveAddr, I2C_Direction_Receiver);
  i2cCheckEvent(i2cData.I2CX, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED);
  for(i=0; i < num-1; i++)
  {
    I2C_AcknowledgeConfig(i2cData.I2CX, ENABLE);
    i2cCheckEvent(i2cData.I2CX, I2C_EVENT_MASTER_BYTE_RECEIVED);
    buffer[i] = I2C_ReceiveData(i2cData.I2CX);
  }
  //最后一个字节应答
  I2C_AcknowledgeConfig(i2cData.I2CX, DISABLE);
  //停止信号
  I2C_GenerateSTOP(i2cData.I2CX, ENABLE);
}

/********************************************************************************************************
Function Name: 
Author       : xiaobo
Date         : 2015-12-16
Description  : 
Inputs       : None
Outputs      : None
Notes        : 
Revision     : 
********************************************************************************************************/
void I2CH_writeByte(I2CH_DATA i2cData, u8 regAddr, u8 data)
{
//  u32 timeOut;
//  while( I2C_GetFlagStatus(i2cData.I2CX, I2C_FLAG_BUSY) && (timeOut != 0x00) )
//    timeOut--;
//  if(timeOut == 0)
//    LEDR = 0;
  //起始信号
  I2C_GenerateSTART(i2cData.I2CX, ENABLE);
  i2cCheckEvent(i2cData.I2CX, I2C_EVENT_MASTER_MODE_SELECT);
  //从机地址+写信号
  I2C_Send7bitAddress(i2cData.I2CX, i2cData.slaveAddr, I2C_Direction_Transmitter);
  i2cCheckEvent(i2cData.I2CX, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);
  //发送访问地址
  I2C_SendData(i2cData.I2CX, regAddr);
  i2cCheckEvent(i2cData.I2CX, I2C_EVENT_MASTER_BYTE_TRANSMITTED);
  //发送数据
  I2C_SendData(i2cData.I2CX, data);
  i2cCheckEvent(i2cData.I2CX, I2C_EVENT_MASTER_BYTE_TRANSMITTED);
  //停止信号
  I2C_GenerateSTOP(i2cData.I2CX, ENABLE);
}

/********************************************************************************************************
Function Name: 
Author       : xiaobo
Date         : 2015-12-18
Description  : 使用DMA的方式读取I2C设备的数据，这个函数调用后会启动DMA读取
Inputs       : None
Outputs      : None
Notes        : 
Revision     : 
********************************************************************************************************/
u8 I2CH_dmaReadStart(I2CH_DATA i2cData, u8 regAddr, u8 num, u8 *buffer)
{
  //起始信号
  I2C_GenerateSTART(i2cData.I2CX, ENABLE);
  i2cCheckEvent(i2cData.I2CX, I2C_EVENT_MASTER_MODE_SELECT);
  //从机地址+写信号
  I2C_Send7bitAddress(i2cData.I2CX, i2cData.slaveAddr, I2C_Direction_Transmitter);
  i2cCheckEvent(i2cData.I2CX, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);
  /* Clear EV6 by setting again the PE bit */
  I2C_Cmd(i2cData.I2CX, ENABLE);
  //发送访问地址
  I2C_SendData(i2cData.I2CX, regAddr);
  i2cCheckEvent(i2cData.I2CX, I2C_EVENT_MASTER_BYTE_TRANSMITTED); 
  //应答自动产生
  //I2C_DMALastTransferCmd(I2C3, ENABLE);
  //重启起始
  I2C_GenerateSTART(i2cData.I2CX, ENABLE);
  i2cCheckEvent(i2cData.I2CX, I2C_EVENT_MASTER_MODE_SELECT);
  //从机地址+读信号  
  I2C_Send7bitAddress(i2cData.I2CX, i2cData.slaveAddr, I2C_Direction_Receiver);
  i2cCheckEvent(i2cData.I2CX, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED);
  //配置应答位
  I2C_AcknowledgeConfig(i2cData.I2CX, ENABLE);
  //启动DMA
  i2cData.dmaStream->NDTR = num;
  i2cData.dmaStream->M0AR = (u32)buffer;
  I2C_DMACmd(i2cData.I2CX, ENABLE);
  DMA_Cmd(i2cData.dmaStream, ENABLE);
  //自动应答
  //I2C_DMALastTransferCmd(I2C3, DISABLE);
  return 0;
}

/********************************************************************************************************
Function Name: 
Author       : xiaobo
Date         : 2015-12-18
Description  : 使用DMA读取I2C设备的数据，这个函数调用后就会等待读取数据完成
Inputs       : None
Outputs      : None
Notes        : 
Revision     : 
********************************************************************************************************/
u8 I2CH_dmaReadWaitFlag(I2CH_DATA i2cData)
{
  u32
  //等待DMA传输完成
  timeOut = I2C_TIMEOUT;
  while( (DMA_GetCmdStatus(i2cData.dmaStream) != ENABLE) && (timeOut != 0x00) )
  {
    timeOut--;
  }
  if(timeOut == 0)
  {
    //LEDB = 0;
    LEDB = 0;
  }
  //等待完成标志位
  timeOut = I2C_TIMEOUT;
  while( (DMA_GetFlagStatus(i2cData.dmaStream, i2cData.dmaTcFlag) == RESET) && (timeOut != 0x00) )
  {
    timeOut--;
  }
  if(timeOut == 0)
  {
    //LEDB = 0;
    LEDB = 0;
  }
  I2C_AcknowledgeConfig(i2cData.I2CX, DISABLE);//配置应答位
  I2C_GenerateSTOP(i2cData.I2CX, ENABLE);
 // DMA_Cmd(DMA1_Stream2, DISABLE);
  i2cData.DmaClearFlags();
  DMA_ClearFlag(DMA1_Stream2, DMA_FLAG_TCIF2 | DMA_FLAG_FEIF2 | DMA_FLAG_DMEIF2 | DMA_FLAG_TEIF2 | DMA_FLAG_HTIF2);
  return 0;
}

//===========================================  End Of File  ===========================================//
