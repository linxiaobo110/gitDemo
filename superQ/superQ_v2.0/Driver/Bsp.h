/********************************************************************************************************
**                                 Copyright (c)   2014«∂»Î Ω±»»¸
**                                                 All Rights Reserved
**  
**                                 Email:729527658@qq.com   QQ:729527658
**-------------------------------------------------------------------------------------------------------
**  MCU        : STM32F429IG (STMicroelectronics)
**  Compiler   : Keil uVision 5.10
**  Module Name: Board Support Package
**  Module Date: 2014-03-14
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
#ifndef _BSP_H_
#define _BSP_H_

/*-----------------------------------------  I N C L U D E S  -----------------------------------------*/
#include "Global.h"

/*---------------------------------------  D E F I N I T I O N  ---------------------------------------*/
typedef struct BSP_DATA
{
    __IO u32 systime;
  u32 test;
  u32 test2;
  u32 test3;
  u8 gyroData[6];
  //u8 test3[10];
  u8 i2c3Data[30];
  u8 usart2RecBuffer[50];
  u8 uart4RecBuffer[50];
  u8 usart3SendBuffer[100];
  u16 tim3Capture;
  CanTxMsg canTxMsg1;
  CanRxMsg canRxMsg1;
//    //u16 tim3Ic[4];
//    u16 tim1Ic[4];
//    u16 tim3Ic[4];
//    u8 sysStatue;
//    u16 servoMidValue;
//    u16 adc1Data[6];
//    float adc1Votage[6];
//    u16 adc3Data[4];
//    float adc3Votage[4];
//    u8 uarrtTxBuffer[20];
//    u32 TEST[5];
//    u16 spi5TxBuffer[13];
//    u16 spi5RxBuffer[13];
}BSP_DATA;

/*-------------------------------------------  M A C R O S  -------------------------------------------*/

//LEDÁÅØÂÆèÂÆö‰πâ
#define LED2   PAout(8)
#define LEDB   PDout(3)
#define LEDR   PDout(4)
#define LEDG   PDout(5)

//GYRO FRAME
#define GYRO_FRAME PBout(4)

//ËìùÁâôÂÆèÂÆö‰πâ
#define BLUETOOTH_USART   USART6   
#define DMA_USART6_TX     DMA2_Stream7

/*
*********************************************************************************************************
*                                      DWT  REGISTERS
*********************************************************************************************************
*/
#define  BSP_REG_DEM_CR                           (*(u32 *)0xE000EDFC)
#define  BSP_REG_DWT_CR                           (*(u32 *)0xE0001000)
#define  BSP_REG_DWT_CYCCNT                       (*(u32 *)0xE0001004)
#define  BSP_BIT_DEM_CR_TRCENA                    0x01000000u
#define  BSP_BIT_DWT_CR_CYCCNTENA                 0x01u

#define ITM_Port8(n)    (*((volatile unsigned char *)(0xE0000000+4*n)))

#define ITM_Port16(n)   (*((volatile unsigned short*)(0xE0000000+4*n)))

#define ITM_Port32(n)   (*((volatile unsigned long *)(0xE0000000+4*n)))
/*
********************************************************************************************************
*                                        Œª¥¯≤Ÿ◊˜∫Í∂®“Â
********************************************************************************************************
*/
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
//IOµÿ÷∑”≥…‰
#define GPIOA_ODR_Addr    (GPIOA_BASE+20)  
#define GPIOB_ODR_Addr    (GPIOB_BASE+20) 
#define GPIOC_ODR_Addr    (GPIOC_BASE+20)  
#define GPIOD_ODR_Addr    (GPIOD_BASE+20) 
#define GPIOE_ODR_Addr    (GPIOE_BASE+20)  
#define GPIOF_ODR_Addr    (GPIOF_BASE+20)   
#define GPIOG_ODR_Addr    (GPIOG_BASE+20)    

#define GPIOA_IDR_Addr    (GPIOA_BASE+16) 
#define GPIOB_IDR_Addr    (GPIOB_BASE+16) 
#define GPIOC_IDR_Addr    (GPIOC_BASE+16) 
#define GPIOD_IDR_Addr    (GPIOD_BASE+16) 
#define GPIOE_IDR_Addr    (GPIOE_BASE+16)  
#define GPIOF_IDR_Addr    (GPIOF_BASE+16)  
#define GPIOG_IDR_Addr    (GPIOG_BASE+16) 
 
//IO≤Ÿ◊˜£¨÷ª≤Ÿ◊˜“ª∏ˆŒª
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  



/*--------------------------------------  D E C L A R A T I O N  --------------------------------------*/
/* Internal Variable */

/* External Variable */
extern BSP_DATA bspData;

/* Internal Function */
static void BSP_GPIOinit(void);
static void BSP_usartInit(void);
static void BSP_timerInit(void);
static void BSP_dmaInit(void);
static void BSP_spiInit(void);
static void BSP_nvicInit(void);
static void Bsp_extiInit(void);
static void Bsp_adcInit(void);
static void BSP_i2cInit(void);
static void Bsp_canInit(void);
/* External Function */
void BSP_init(void);

//OS 
u32  BSP_CPU_ClkFreq (void);
void  CPU_TS_TmrInit (void);
u32  CPU_TS_TmrRd (void);
#endif
//===========================================  End Of File  ===========================================//
