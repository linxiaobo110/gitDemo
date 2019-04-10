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
#ifndef _I2CH_H
#define _I2CH_H

/*-----------------------------------------  I N C L U D E S  -----------------------------------------*/
#include "Global.h"

/*---------------------------------------  D E F I N I T I O N  ---------------------------------------*/
typedef struct I2CH_DATA
{
  I2C_TypeDef *I2CX;
  u8 slaveAddr;
  DMA_Stream_TypeDef *dmaStream;
  u32 dmaTcFlag;
  void (*DmaClearFlags)(void);
}I2CH_DATA;

#define I2C_TIMEOUT    (u32)168000

/*-------------------------------------------  M A C R O S  -------------------------------------------*/


/*--------------------------------------  D E C L A R A T I O N  --------------------------------------*/
/* Internal Variable */

/* External Variable */

/* Internal Function */

/* External Function */
u8 i2cCheckEvent(I2C_TypeDef *i2c, u32 flag);
 u8 I2CH_readByte(I2CH_DATA i2cData, u8 regAddr);
void I2CH_readBuffer(I2CH_DATA i2cData, u8 regAddr, u8 num, u8 *buffer) ;
void I2CH_writeByte(I2CH_DATA i2cData, u8 regAddr, u8 data);
u8 I2CH_dmaReadStart(I2CH_DATA i2cData, u8 regAddr, u8 num, u8 *buffer);
u8 I2CH_dmaReadWaitFlag(I2CH_DATA i2cData);
#endif
//===========================================  End Of File  ===========================================//
