/********************************************************************************************************
**                                 Copyright (c)   2014嵌入式比赛
**                                                 All Rights Reserved
**  
**                                 Email:729527658@qq.com   QQ:729527658
**-------------------------------------------------------------------------------------------------------
**  MCU        : STM32F429ZI (STMicroelectronics)
**  Compiler   : Keil uVision 5.10
**  Module Name: Board Support Package
**  Module Date: 2014-0314
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
#include "Bsp.h"
/*---------------------------------------  D E F I N I T I O N  ---------------------------------------*/
#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
      set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
BSP_DATA bspData;

/*----------------------------------------  F U N C T I O N S  ----------------------------------------*/
	/********************************************************************************************************
Function Name: BSP_init
Author       : xiaobo
Date         : 2013-03-14
Description  : 
Inputs       : None
Outputs      : None
Notes        : 
Revision     : 
********************************************************************************************************/
void BSP_init(void)
{
    BSP_GPIOinit();
    BSP_usartInit();
    BSP_timerInit();
    BSP_nvicInit();
    BSP_dmaInit();
    Bsp_adcInit();
    BSP_spiInit();
    BSP_i2cInit();
    Bsp_extiInit();
    Bsp_canInit();
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);
}
/********************************************************************************************************
Function Name: BSP_GPIOinit
Author       : xiaobo
Date         : 2013-03-14
Description  : 
Inputs       : None
Outputs      : None
Notes        : 
Revision     : 
********************************************************************************************************/
static void BSP_GPIOinit(void)
{
  //定义数据结构
  GPIO_InitTypeDef gpioInitStructure;
  
	//时钟
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
//  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	
/*
 *****************************************************************
  example of GPIO
 *****************************************************************
  //普通IO 输出
//  gpioInitStructure.GPIO_OType = GPIO_OType_PP;
//  gpioInitStructure.GPIO_PuPd = GPIO_PuPd_UP;
//  gpioInitStructure.GPIO_Mode = GPIO_Mode_OUT;
//  gpioInitStructure.GPIO_Speed = GPIO_Speed_2MHz;
//  gpioInitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14; 
//  GPIO_Init(GPIOG, &gpioInitStructure);
  
   //功能复用
//  gpioInitStructure.GPIO_OType = GPIO_OType_PP;
//  gpioInitStructure.GPIO_PuPd = GPIO_PuPd_UP;
//  gpioInitStructure.GPIO_Mode = GPIO_Mode_AF;
//  gpioInitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  gpioInitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; 
//  GPIO_Init(GPIOA, &gpioInitStructure);
  
  //AD
//  gpioInitStructure.GPIO_OType = GPIO_OType_PP;
//  gpioInitStructure.GPIO_Mode = GPIO_Mode_AN;
//  gpioInitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
//  gpioInitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//  gpioInitStructure.GPIO_Speed = GPIO_Speed_25MHz;
//  GPIO_Init(GPIOB, &gpioInitStructure);
 ******************************************************************
*/
  //LED PD3 PD4 PD5 PA8
  gpioInitStructure.GPIO_OType = GPIO_OType_OD;
  gpioInitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  gpioInitStructure.GPIO_Mode = GPIO_Mode_OUT;
  gpioInitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  gpioInitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5; 
  GPIO_Init(GPIOD, &gpioInitStructure);
  gpioInitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_Init(GPIOA, &gpioInitStructure);
  
  //USART4 to sensorUAV PA0 PA1
  gpioInitStructure.GPIO_Mode = GPIO_Mode_AF;
  gpioInitStructure.GPIO_OType = GPIO_OType_PP;
  gpioInitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
  gpioInitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  gpioInitStructure.GPIO_Speed = GPIO_Speed_25MHz;
  GPIO_Init(GPIOA, &gpioInitStructure);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_UART4);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_UART4);
//  
  //buzzer PE5 TIM9_CH1
  gpioInitStructure.GPIO_Mode = GPIO_Mode_AF;
  gpioInitStructure.GPIO_OType = GPIO_OType_PP;
  gpioInitStructure.GPIO_Pin = GPIO_Pin_5;
  gpioInitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  gpioInitStructure.GPIO_Speed = GPIO_Speed_25MHz;
  GPIO_Init(GPIOE, &gpioInitStructure);
  GPIO_PinAFConfig( GPIOE, GPIO_PinSource5, GPIO_AF_TIM9);
  
  //ESC PD12-PD15  TIM4 CH1-CH4
  gpioInitStructure.GPIO_Mode = GPIO_Mode_AF;
  gpioInitStructure.GPIO_OType = GPIO_OType_PP;
  gpioInitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  gpioInitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  gpioInitStructure.GPIO_Speed = GPIO_Speed_25MHz;
  GPIO_Init(GPIOD, &gpioInitStructure);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_TIM4);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_TIM4);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_TIM4);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_TIM4);
  
  //AT25320 PA4-PA7 SPI2
  gpioInitStructure.GPIO_Mode = GPIO_Mode_AF;
  gpioInitStructure.GPIO_OType = GPIO_OType_PP;
  gpioInitStructure.GPIO_Pin =  GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  gpioInitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  gpioInitStructure.GPIO_Speed = GPIO_Speed_25MHz;
  GPIO_Init(GPIOB, &gpioInitStructure);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_SPI2);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_SPI2);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_SPI2);
  gpioInitStructure.GPIO_Mode = GPIO_Mode_OUT;
  gpioInitStructure.GPIO_OType = GPIO_OType_PP;
  gpioInitStructure.GPIO_Pin =  GPIO_Pin_4;
  gpioInitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  gpioInitStructure.GPIO_Speed = GPIO_Speed_25MHz;
  GPIO_Init(GPIOA, &gpioInitStructure);
  
  //SDIO PC8,PC9,PC10,PC11,PC12,PD2
  gpioInitStructure.GPIO_Mode = GPIO_Mode_AF;
  gpioInitStructure.GPIO_OType = GPIO_OType_PP;
  gpioInitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
  gpioInitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  gpioInitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_Init(GPIOC, &gpioInitStructure);
  gpioInitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_Init(GPIOD, &gpioInitStructure);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_SDIO);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource9, GPIO_AF_SDIO);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_SDIO);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_SDIO);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_SDIO);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource2, GPIO_AF_SDIO);
  
  //receiver u2-rx pa3
  gpioInitStructure.GPIO_Mode = GPIO_Mode_AF;
  gpioInitStructure.GPIO_OType = GPIO_OType_PP;
  gpioInitStructure.GPIO_Pin = GPIO_Pin_3;
  gpioInitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  gpioInitStructure.GPIO_Speed = GPIO_Speed_25MHz;
  GPIO_Init(GPIOA, &gpioInitStructure);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);
  
  //BLUETEETH PD8-PD9 USART3 
  gpioInitStructure.GPIO_Mode = GPIO_Mode_AF;
  gpioInitStructure.GPIO_OType = GPIO_OType_PP;
  gpioInitStructure.GPIO_Pin =  GPIO_Pin_8 | GPIO_Pin_9;
  gpioInitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  gpioInitStructure.GPIO_Speed = GPIO_Speed_25MHz;
  GPIO_Init(GPIOD, &gpioInitStructure);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_USART3);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_USART3);
  
//  
//  //receiver 
//  //IN1-IN4: PE9 PE11 PE13 PE14 TIM1 CH1 CH2 CH3 CH4
//  //IN5-IN6: PB10 PB11 TIM2 CH3 CH4
//  gpioInitStructure.GPIO_Mode = GPIO_Mode_AF;
//  //gpioInitStructure.GPIO_OType = GPIO_OType_PP;
//  gpioInitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_11 | GPIO_Pin_13 | GPIO_Pin_14;
//  gpioInitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//  gpioInitStructure.GPIO_Speed = GPIO_Speed_25MHz;
//  GPIO_Init(GPIOE, &gpioInitStructure);
//  GPIO_PinAFConfig(GPIOE, GPIO_PinSource9, GPIO_AF_TIM1);
//  GPIO_PinAFConfig(GPIOE, GPIO_PinSource11, GPIO_AF_TIM1);
//  GPIO_PinAFConfig(GPIOE, GPIO_PinSource13, GPIO_AF_TIM1);
//  GPIO_PinAFConfig(GPIOE, GPIO_PinSource14, GPIO_AF_TIM1);
//  gpioInitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
//  GPIO_Init(GPIOB, &gpioInitStructure);
//  GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_TIM2);
//  GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_TIM2);
//  
//  //sonar PB1 TIM3-CH4
//  gpioInitStructure.GPIO_Mode = GPIO_Mode_AF;
//  //gpioInitStructure.GPIO_OType = GPIO_OType_PP;
//  gpioInitStructure.GPIO_Pin = GPIO_Pin_1;
//  gpioInitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//  gpioInitStructure.GPIO_Speed = GPIO_Speed_25MHz;
//  GPIO_Init(GPIOB, &gpioInitStructure);
//  GPIO_PinAFConfig(GPIOB, GPIO_PinSource1, GPIO_AF_TIM3);
//  

//  

//  

//  
//    //USART2 PA2 PA3
//  gpioInitStructure.GPIO_Mode = GPIO_Mode_AF;
//  gpioInitStructure.GPIO_OType = GPIO_OType_PP;
//  gpioInitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
//  gpioInitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//  gpioInitStructure.GPIO_Speed = GPIO_Speed_25MHz;
//  GPIO_Init(GPIOA, &gpioInitStructure);
//  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART2);
//  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART2);
//  
//  //CAN鎺ュ彛 涓庝富鏉块�氫俊 PD0-PD1
////  gpioInitStructure.GPIO_Mode = GPIO_Mode_AF;
////  gpioInitStructure.GPIO_OType = GPIO_OType_PP;
////  gpioInitStructure.GPIO_PuPd = GPIO_PuPd_UP;
////  gpioInitStructure.GPIO_Speed = GPIO_Speed_50MHz;
////  gpioInitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
////  GPIO_Init(GPIOD, &gpioInitStructure);
////  GPIO_PinAFConfig(GPIOD, GPIO_PinSource0 , GPIO_AF_CAN1);
////  GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_CAN1);


}
/********************************************************************************************************
Function Name: BSP_usart
Author       : xiaobo
Date         : 2013-03-14
Description  : 
Inputs       : None
Outputs      : None
Notes        : 
Revision     : 
********************************************************************************************************/
static void BSP_usartInit(void)
{
  //定义数据结构
  USART_InitTypeDef usartInitstructure;
/*
 *****************************************************************
  example of USART
 *****************************************************************
  usartInitstructure.USART_BaudRate = 115200;
  usartInitstructure.USART_WordLength = USART_WordLength_8b;
  usartInitstructure.USART_StopBits = USART_StopBits_1;
  usartInitstructure.USART_Parity = USART_Parity_No;
  usartInitstructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  usartInitstructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  
  //USART1
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);  //时钟使能
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);   //功能复用 
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);
  USART_Init(USART1,&usartInitstructure);                 //串口功能初始化
  USART_ITConfig(USART1, USART_IT_TC, ENABLE);            //串口中断
  USART_Cmd(USART1, ENABLE);                              //串口使能
  ******************************************************************
*/
  usartInitstructure.USART_BaudRate = 115200;
  usartInitstructure.USART_WordLength = USART_WordLength_8b;
  usartInitstructure.USART_StopBits = USART_StopBits_1;
  usartInitstructure.USART_Parity = USART_Parity_No;
  usartInitstructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  usartInitstructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  
  //USART4 communicate with sensorUAV
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);  //时讚使艤
  USART_Init(UART4,&usartInitstructure);                 //援酄氋ε溤凳价�
  USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);            //援酄氉愖�
  USART_Cmd(UART4, ENABLE);                              //援酄毷古�
  
  
  
  //USART3
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);  //时讚使艤
  USART_Init(USART3,&usartInitstructure);                 //援酄氋ε溤凳价�
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);            //援酄氉愖�
  USART_Cmd(USART3, ENABLE);                              //援酄毷古�
  
    //USART1
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);  //时讚使艤
  USART_Init(USART1,&usartInitstructure);                 //援酄氋ε溤凳价�
  //USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);            //援酄氉愖�
  USART_Cmd(USART1, ENABLE);                              //援酄毷古�
  
  //USART2 receiver
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);  //时讚使艤
  usartInitstructure.USART_BaudRate = 100000;
  usartInitstructure.USART_StopBits = USART_StopBits_2;
  usartInitstructure.USART_Parity = USART_Parity_Even;
  usartInitstructure.USART_WordLength = USART_WordLength_9b;
  USART_Init(USART2,&usartInitstructure);                 //援酄氋ε溤凳价�
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);            //援酄氉愖�
  USART_Cmd(USART2, ENABLE);                              //援酄毷古�
}


static void BSP_i2cInit(void)
{

}

static void BSP_dmaInit(void)
{
  //定义数据结构
  DMA_InitTypeDef dmaInitStructure;
//  //初始化时钟
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
  
//  //ADC 地址  
//  #define ADC1_DR_ADDRESS                 ((u32)0x4001204C)
//  #define ADC3_DR_ADDRESS                ((u32)0x4001224C) 
  #define I2C1_DR_ADDRESS ((u32)0x40005410)
  #define I2C2_DR_ADDRESS ((u32)0x40005810)
  #define I2C3_DR_ADDRESS ((u32)0x40005C10)
  
 /*
 *****************************************************************
  example of DMA for AD
 *****************************************************************
  // DMA2 Stream0 channe0 configuration *************************************
  dmaInitStructure.DMA_Channel = DMA_Channel_0;  
  dmaInitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC1_DR_ADDRESS;//外设地址
  dmaInitStructure.DMA_Memory0BaseAddr = (uint32_t)&bspData.adcData[0];//AD值存储地址
  dmaInitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;//传输方向：外设到内存
  dmaInitStructure.DMA_BufferSize = 2;//转换数量
  dmaInitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设地址不变
  dmaInitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//内存地址自增
  dmaInitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//外设数据格式
  dmaInitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//内存数据格式
  dmaInitStructure.DMA_Mode = DMA_Mode_Circular;//循环工作
  dmaInitStructure.DMA_Priority = DMA_Priority_High;//优先级
  dmaInitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;     //FIF不使用    
  dmaInitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
  dmaInitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  dmaInitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init(DMA2_Stream0, &dmaInitStructure);  //初始化DMA
  DMA_Cmd(DMA2_Stream0, ENABLE);    //DMA使能
  ******************************************************************
*/
  //USART3 DMA1 STREAM3 CHANEL4
 DMA_ClearFlag(DMA1_Stream3, DMA_FLAG_TCIF3 | DMA_FLAG_FEIF3 | DMA_FLAG_DMEIF3 | DMA_FLAG_TEIF3 | DMA_FLAG_HTIF3);
 DMA_Cmd(DMA1_Stream3, DISABLE);
 DMA_DeInit(DMA1_Stream3);
 dmaInitStructure.DMA_Channel = DMA_Channel_4;  
 dmaInitStructure.DMA_PeripheralBaseAddr = (u32)(&USART3->DR);//微狮謽址
 dmaInitStructure.DMA_Memory0BaseAddr = (uint32_t)bspData.usart3SendBuffer;//AD值咋寓謽址
 dmaInitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;//垣摔捉胁煤微狮战艢咋
 dmaInitStructure.DMA_BufferSize = 20;//转刍私
 dmaInitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//微狮謽址一窑
 dmaInitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//艢咋謽址財斩
 dmaInitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//微狮私邼俦式
 dmaInitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//艢咋私邼俦式
 dmaInitStructure.DMA_Mode = DMA_Mode_Normal;//循鄯伽胤
 dmaInitStructure.DMA_Priority = DMA_Priority_Low;//詤袌芏
 dmaInitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;     //FIF一使詢    
 dmaInitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
 dmaInitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
 dmaInitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
 DMA_Init(DMA1_Stream3, &dmaInitStructure);  //缘始郫DMA
}
/********************************************************************************************************
Function Name: BSP_timer
Author       : xiaobo
Date         : 2013-03-14
Description  : 
Inputs       : None
Outputs      : None
Notes        : 
Revision     : 
********************************************************************************************************/
static void BSP_timerInit(void)
{
  //数据类型定义
  TIM_TimeBaseInitTypeDef timBaseInitStructure;//基本定时对象
  TIM_ICInitTypeDef timIcInitStructure;//输入捕获对象
  TIM_OCInitTypeDef timOcInitStructure;//输出波形对象
/*
 *****************************************************************
  example of timer
 *****************************************************************
  //基本时钟初始化
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  timBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  timBaseInitStructure.TIM_Prescaler = 84 - 1;
  timBaseInitStructure.TIM_Period = 20000 - 1;
  timBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
  timBaseInitStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM3, &timBaseInitStructure);
  
  //输入捕获初始化
  timIcInitStructure.TIM_Channel = TIM_Channel_1;
  timIcInitStructure.TIM_ICPolarity = TIM_ICPolarity_BothEdge;
  timIcInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  timIcInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  timIcInitStructure.TIM_ICFilter = 0x0;
  TIM_ICInit(TIM3, &timIcInitStructure);
  timIcInitStructure.TIM_Channel = TIM_Channel_2;
  
  //输出PWM波初始化
  timOcInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  timOcInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  timOcInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  timOcInitStructure.TIM_Pulse = 1500;
  TIM_OC1Init(TIM3, &timOcInitStructure);
  
  TIM_Cmd(TIM3, ENABLE);//输出使能
  TIM_ITConfig(TIM3, TIM_IT_CC1 | TIM_IT_CC2, ENABLE);//定时器中断设置
 ******************************************************************
*/
  //Bell TIM9-Ch1
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9, ENABLE);
  timBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  timBaseInitStructure.TIM_Prescaler = 84 - 1;
  timBaseInitStructure.TIM_Period = 500 - 1;
  timBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
  timBaseInitStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM9, &timBaseInitStructure);
  
  timOcInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  timOcInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  timOcInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  timOcInitStructure.TIM_Pulse = 0;
  TIM_OC1Init(TIM9, &timOcInitStructure);
  
  TIM_OC1PreloadConfig(TIM9, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM9, ENABLE);
	
	TIM_Cmd(TIM9, ENABLE);
  
//  /*
//   ---------------------------
//   *RECEIVER TIM1 CH1-CH4 TIM2 CH3 CH4
//   ---------------------------   
//  */
//  //TIM1
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
//  timBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
//  timBaseInitStructure.TIM_Prescaler = 180 - 1;
//  timBaseInitStructure.TIM_Period = 0xffff - 1;
//  timBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
//  //timInitStructure.TIM_RepetitionCounter = 0;
//  TIM_TimeBaseInit(TIM1, &timBaseInitStructure);
//  
//  timIcInitStructure.TIM_Channel = TIM_Channel_1;
//  timIcInitStructure.TIM_ICPolarity = TIM_ICPolarity_BothEdge;
//  timIcInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
//  timIcInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
//  timIcInitStructure.TIM_ICFilter = 0x0;
//  TIM_ICInit(TIM1, &timIcInitStructure);
//  
//  timIcInitStructure.TIM_Channel = TIM_Channel_2;
//  TIM_ICInit(TIM1, &timIcInitStructure);
//  
//  timIcInitStructure.TIM_Channel = TIM_Channel_3;
//  TIM_ICInit(TIM1, &timIcInitStructure);
//  
//  timIcInitStructure.TIM_Channel = TIM_Channel_4;
//  TIM_ICInit(TIM1, &timIcInitStructure);
//  
//  TIM_Cmd(TIM1, ENABLE);
//  TIM_ITConfig(TIM1, TIM_IT_CC1 | TIM_IT_CC2 | TIM_IT_CC3 | TIM_IT_CC4, ENABLE);
//  
//  //TIM2
//  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
//  timBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
//  timBaseInitStructure.TIM_Prescaler = 90 - 1;
//  timBaseInitStructure.TIM_Period = 0xffff - 1;
//  timBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
//  //timInitStructure.TIM_RepetitionCounter = 0;
//  TIM_TimeBaseInit(TIM2, &timBaseInitStructure);
//  
//  timIcInitStructure.TIM_Channel = TIM_Channel_3;
//  timIcInitStructure.TIM_ICPolarity = TIM_ICPolarity_BothEdge;
//  timIcInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
//  timIcInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
//  timIcInitStructure.TIM_ICFilter = 0x0;
//  TIM_ICInit(TIM2, &timIcInitStructure);
//  
//  timIcInitStructure.TIM_Channel = TIM_Channel_4;
//  TIM_ICInit(TIM2, &timIcInitStructure);
//  
//  TIM_Cmd(TIM2, ENABLE);
//  TIM_ITConfig(TIM2, TIM_IT_CC3 | TIM_IT_CC4, ENABLE);
  
  //TIM3
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  timBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
  timBaseInitStructure.TIM_Prescaler = 84 - 1;
  timBaseInitStructure.TIM_Period = 0xffff - 1;
  timBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  //timInitStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM3, &timBaseInitStructure);
  
  timIcInitStructure.TIM_Channel = TIM_Channel_4;
  timIcInitStructure.TIM_ICPolarity = TIM_ICPolarity_BothEdge;
  timIcInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  timIcInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  timIcInitStructure.TIM_ICFilter = 0x0;
  TIM_ICInit(TIM3, &timIcInitStructure);
  
  TIM_Cmd(TIM3, ENABLE);
  TIM_ITConfig(TIM3, TIM_IT_CC4, ENABLE);
  
  /*
   ---------------------------
   *ESC TIM4 CH1-CH4
   ---------------------------   
  */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
  timBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  timBaseInitStructure.TIM_Prescaler = 84 - 1;
  timBaseInitStructure.TIM_Period = 5000 - 1;
  timBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
  timBaseInitStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM4, &timBaseInitStructure);
  
  timOcInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  timOcInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  timOcInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  timOcInitStructure.TIM_Pulse = 800;
  TIM_OC1Init(TIM4, &timOcInitStructure);
  
  TIM_OC2Init(TIM4, &timOcInitStructure);
  TIM_OC3Init(TIM4, &timOcInitStructure);
  TIM_OC4Init(TIM4, &timOcInitStructure);
  
  TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM4, ENABLE);
  TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM4, ENABLE);
}

/********************************************************************************************************
Function Name: BSP_canInit
Author       : xiaobo
Date         : 2016-08-25
Description  : 
Inputs       : None
Outputs      : None
Notes        : 
Revision     : 
********************************************************************************************************/
static void Bsp_canInit(void)
{
  CAN_InitTypeDef canInitStructure;
  CAN_FilterInitTypeDef canFilterInitStructure;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
  CAN_DeInit(CAN1);
  canInitStructure.CAN_TTCM = DISABLE;
  canInitStructure.CAN_ABOM = DISABLE;
  canInitStructure.CAN_AWUM = DISABLE;
  canInitStructure.CAN_NART = DISABLE;
  canInitStructure.CAN_RFLM = DISABLE;
  canInitStructure.CAN_TXFP = DISABLE;
  canInitStructure.CAN_Mode = CAN_Mode_Normal;
  canInitStructure.CAN_SJW = CAN_SJW_1tq;
  
  canInitStructure.CAN_BS1 = CAN_BS1_6tq;
  canInitStructure.CAN_BS2 = CAN_BS2_7tq;
  canInitStructure.CAN_Prescaler = 6;
  CAN_Init(CAN1, &canInitStructure);
  
  canFilterInitStructure.CAN_FilterNumber = 0;
  canFilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
  canFilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
  canFilterInitStructure.CAN_FilterIdHigh = 0x1234;
  canFilterInitStructure.CAN_FilterIdLow = 0x1234;
  canFilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
  canFilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
  canFilterInitStructure.CAN_FilterFIFOAssignment = 0;
  
  canFilterInitStructure.CAN_FilterActivation = ENABLE;
  CAN_FilterInit(&canFilterInitStructure);
  
//  CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);
}

/********************************************************************************************************
Function Name: BSP_timer
Author       : xiaobo
Date         : 2013-03-14
Description  : 
Inputs       : None
Outputs      : None
Notes        : 
Revision     : 
********************************************************************************************************/
static void BSP_nvicInit(void)
{
  // 中断方案选择
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  // 中断初始化
  fun_nvicSetNormal(UART4_IRQn, 0, 1);//commmunite with sensorUAVs
  fun_nvicSetNormal(USART2_IRQn, 0, 1);//commmunite with sensorUAVs
  fun_nvicSetNormal(TIM1_CC_IRQn, 0, 1);//interupt of receiver
  fun_nvicSetNormal(TIM2_IRQn, 0, 1);//interupt of receiver
  fun_nvicSetNormal(TIM3_IRQn, 1, 0);//interupt of sonar
  fun_nvicSetNormal(TIM4_IRQn, 0, 0);//MAIN CYCLE
  fun_nvicSetNormal(USART3_IRQn, 2, 0);//MPU6050涓柇;MPU9150涓柇
  fun_nvicSetNormal(DMA2_Stream3_IRQn, 1, 0);
  fun_nvicSetNormal(SDIO_IRQn, 1, 0);
  //fun_nvicSetNormal(CAN1_RX0_IRQn, 2, 1);
}

/********************************************************************************************************
Function Name: BSP_timer
Author       : xiaobo
Date         : 2013-03-14
Description  : 
Inputs       : None
Outputs      : None
Notes        : 
Revision     : 
********************************************************************************************************/
static void BSP_spiInit(void)
{
  //数据类型定义
  SPI_InitTypeDef spiInitStructure;
/*
 *****************************************************************
  example of SPI
 *****************************************************************
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI5, ENABLE);//初始化时钟
  spiInitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//双线全双工
  spiInitStructure.SPI_Mode = SPI_Mode_Master;//主机模式
  spiInitStructure.SPI_DataSize = SPI_DataSize_8b;//数据格式
  spiInitStructure.SPI_CPOL = SPI_CPOL_High;//时钟极性
  spiInitStructure.SPI_CPHA = SPI_CPHA_2Edge;//触发沿
  spiInitStructure.SPI_NSS = SPI_NSS_Soft;//片选信号软件控制
  spiInitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;//波特率分频
  spiInitStructure.SPI_FirstBit = SPI_FirstBit_MSB;//大小头
  //spiInitStructure.SPI_CRCPolynomial = 7;//CRC校验位数
  SPI_Init(SPI5, &spiInitStructure);//初始化 
  SPI_Cmd(SPI5, ENABLE);//使能
 ******************************************************************
*/
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);//
//  spiInitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//
//  spiInitStructure.SPI_Mode = SPI_Mode_Master;//
//  spiInitStructure.SPI_DataSize = SPI_DataSize_8b;//
//  spiInitStructure.SPI_CPOL = SPI_CPOL_High;//
//  spiInitStructure.SPI_CPHA = SPI_CPHA_2Edge;//
//  spiInitStructure.SPI_NSS = SPI_NSS_Soft;//
//  spiInitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;//
//  spiInitStructure.SPI_FirstBit = SPI_FirstBit_MSB;//
//  spiInitStructure.SPI_CRCPolynomial = 7;//
//  SPI_Init(SPI1, &spiInitStructure);// 
//  SPI_Cmd(SPI1, ENABLE);//
  //AT25320
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);//
  spiInitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//
  spiInitStructure.SPI_Mode = SPI_Mode_Master;//
  spiInitStructure.SPI_DataSize = SPI_DataSize_8b;//
  spiInitStructure.SPI_CPOL = SPI_CPOL_Low;//
  spiInitStructure.SPI_CPHA = SPI_CPHA_1Edge;//
  spiInitStructure.SPI_NSS = SPI_NSS_Soft;//
  spiInitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;//
  spiInitStructure.SPI_FirstBit = SPI_FirstBit_MSB;//
  spiInitStructure.SPI_CRCPolynomial = 7;//
  SPI_Init(SPI2, &spiInitStructure);// 
  SPI_Cmd(SPI2, ENABLE);//

}



static void Bsp_extiInit(void)
{
  //数据类型定义
//  EXTI_InitTypeDef exti;
/*
 *****************************************************************
  example of EXTI
 *****************************************************************
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//时钟初始化
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource12);//外部中断管脚选择
  exti.EXTI_LineCmd = ENABLE;
  exti.EXTI_Mode = EXTI_Mode_Interrupt;
  exti.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
  exti.EXTI_Line = EXTI_Line12;
  EXTI_Init(&exti);
 ******************************************************************
*/

}

static void Bsp_adcInit(void)
{
  //数据类型定义
//  ADC_InitTypeDef       ADC_InitStructure;
//  ADC_CommonInitTypeDef ADC_CommonInitStructure;
/*
 *****************************************************************
  example of ADC
 *****************************************************************
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  // ADC Common Init *********************************************************
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div8;
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
  ADC_CommonInit(&ADC_CommonInitStructure);

  // ADC3 Init ***************************************************************
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion = 2;
  ADC_Init(ADC1, &ADC_InitStructure);

  // ADC3 regular channel7 configuration ************************************
  ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_3Cycles);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 2, ADC_SampleTime_3Cycles);
  
 // Enable DMA request after last transfer (Single-ADC mode) 
  ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);

  // Enable ADC3 DMA 
  ADC_DMACmd(ADC1, ENABLE);

  // Enable ADC3 
  ADC_Cmd(ADC1, ENABLE);
  
  // Start ADC1 Software Conversion
  ADC_SoftwareStartConv(ADC1);
 ******************************************************************
*/
  
 
}

/********************************************************************************************************
Function Name: printf
Author       : xiaobo
Date         : 2013-03-14
Description  : 
Inputs       : None
Outputs      : None
Notes        : 
Revision     : 
********************************************************************************************************/
PUTCHAR_PROTOTYPE
{
  while(ITM_Port8(0) == 0);
  ITM_Port8(0) = ch;
  return ch;
}

/*********************************************************************************************************
*                                            BSP_CPU_ClkFreq()
*
* Description : Read CPU registers to determine the CPU clock frequency of the chip.
*
* Argument(s) : none.
*
* Return(s)   : The CPU clock frequency, in Hz.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_INT32U  BSP_CPU_ClkFreq (void)
{
    RCC_ClocksTypeDef  rcc_clocks;
    RCC_GetClocksFreq(&rcc_clocks);

    return ((CPU_INT32U)rcc_clocks.SYSCLK_Frequency);
}

//DWT时钟初始化
#if (CPU_CFG_TS_TMR_EN == DEF_ENABLED)
void  CPU_TS_TmrInit (void)
{
  CPU_INT32U  fclk_freq;
  fclk_freq = BSP_CPU_ClkFreq();

  BSP_REG_DEM_CR     |= (CPU_INT32U)BSP_BIT_DEM_CR_TRCENA;    /* Enable Cortex-M4's DWT CYCCNT reg.                   */
  BSP_REG_DWT_CYCCNT  = (CPU_INT32U)0u;
  BSP_REG_DWT_CR     |= (CPU_INT32U)BSP_BIT_DWT_CR_CYCCNTENA;
  //鐢ㄦ搷浣滅郴缁熸椂璇锋墦寮�
  CPU_TS_TmrFreqSet((CPU_TS_TMR_FREQ)fclk_freq);
    //SysTick_Config(1000000);
}
#endif

#if (CPU_CFG_TS_TMR_EN == DEF_ENABLED)
u32  CPU_TS_TmrRd (void)
{
    CPU_TS_TMR  ts_tmr_cnts;


    ts_tmr_cnts = (CPU_TS_TMR)BSP_REG_DWT_CYCCNT;

    return (ts_tmr_cnts);
}
#endif

#if (CPU_CFG_TS_32_EN == DEF_ENABLED)
CPU_INT64U  CPU_TS32_to_uSec (CPU_TS32  ts_cnts)
{
    CPU_INT64U  ts_us;
    CPU_INT64U  fclk_freq;


    fclk_freq = BSP_CPU_ClkFreq();
    ts_us     = ts_cnts / (fclk_freq / DEF_TIME_NBR_uS_PER_SEC);

    return (ts_us);
}
#endif

#if (CPU_CFG_TS_64_EN == DEF_ENABLED)
CPU_INT64U  CPU_TS64_to_uSec (CPU_TS64  ts_cnts)
{
    CPU_INT64U  ts_us;
    CPU_INT64U  fclk_freq;


    fclk_freq = BSP_CPU_ClkFreq();
    ts_us     = ts_cnts / (fclk_freq / DEF_TIME_NBR_uS_PER_SEC);

    return (ts_us);
}
#endif
//===========================================  End Of File  ===========================================//
