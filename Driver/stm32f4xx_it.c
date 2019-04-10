/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Templates/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.6.1
  * @date    21-October-2015
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"
#include "main.h"

/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
//void PendSV_Handler(void)
//{
//}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
//void SysTick_Handler(void)
//{
////  TimingDelay_Decrement();
//  bspData.systime++;
//}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

  void EXTI9_5_IRQHandler(void)
  {

    if(EXTI_GetITStatus(EXTI_Line8) != RESET)
    {
      EXTI_ClearITPendingBit(EXTI_Line8);
    }
    if(EXTI_GetITStatus(EXTI_Line5) != RESET)
    {
      EXTI_ClearITPendingBit(EXTI_Line5);
    }
    
  }
  void EXTI15_10_IRQHandler(void)
  {
    if(EXTI_GetITStatus(EXTI_Line15) != RESET)
    {
      EXTI_ClearITPendingBit(EXTI_Line15);
    }
  }
  
  void DMA1_Stream2_IRQHandler(void)
  {
    if(DMA_GetITStatus(DMA1_Stream2, DMA_IT_TCIF2) != RESET)
    {
      DMA_ClearITPendingBit(DMA1_Stream2, DMA_IT_TCIF2);
    }
  }
  
  //uart4 communication with sensorUAV
  void UART4_IRQHandler(void)
  {
    
    static u8 lastValue = 0;
    static u8 counter = 0;
    u8 data;
//    FUN_ISR_HEADER();
    if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)
    {
      USART_ClearITPendingBit(UART4, USART_IT_RXNE);
      data = USART_ReceiveData(UART4);
      if((lastValue == 0xFa) && (data == 0xf5))
      {
        bspData.uart4RecBuffer[0] = 0xfa;
        bspData.uart4RecBuffer[1] = 0xF5;
        counter = 2;
      }else
      {
        bspData.uart4RecBuffer[counter++] = data;
        if(counter == 30)
        {
          counter = 0;
          imuData.angle[0] = -(short)((bspData.uart4RecBuffer[4]<<8) | bspData.uart4RecBuffer[5]) * 0.01f;
          imuData.angle[1] = -(short)((bspData.uart4RecBuffer[6]<<8) | bspData.uart4RecBuffer[7]) * 0.01f;
          imuData.angle[2] = (short)((bspData.uart4RecBuffer[8]<<8) | bspData.uart4RecBuffer[9]) * 0.01f;
          imuData.angleV[0] = -(short)((bspData.uart4RecBuffer[10]<<8) | bspData.uart4RecBuffer[11]) * 0.01f;
          imuData.angleV[1] = -(short)((bspData.uart4RecBuffer[12]<<8) | bspData.uart4RecBuffer[13]) * 0.01f;
          imuData.angleV[2] = (short)((bspData.uart4RecBuffer[14]<<8) | bspData.uart4RecBuffer[15]) * 0.01f;
          uavData.powerVotage = (short)((bspData.uart4RecBuffer[16]<<8) | bspData.uart4RecBuffer[17]) * 0.01f;
          imuData.position[0] = (short)((bspData.uart4RecBuffer[18]<<8) | bspData.uart4RecBuffer[19]);
          imuData.position[1] = -(short)((bspData.uart4RecBuffer[20]<<8) | bspData.uart4RecBuffer[21]);
          imuData.position[2] = (short)((bspData.uart4RecBuffer[22]<<8) | bspData.uart4RecBuffer[23]);
          imuData.positionV[0] = (short)((bspData.uart4RecBuffer[24]<<8) | bspData.uart4RecBuffer[25])*0.1f;
          imuData.positionV[1] = -(short)((bspData.uart4RecBuffer[26]<<8) | bspData.uart4RecBuffer[27])*0.1f;
          imuData.positionV[2] = (short)((bspData.uart4RecBuffer[28]<<8) | bspData.uart4RecBuffer[29]) * 0.1f;
          dogData.imuDog++;
          

        }
      }
      lastValue = data;
    }
//    FUN_ISR_FOOT();
  }
  
  
  #define ROLL_CHANNEL 3
  #define PITCH_CHANNEL 1
  #define OIL_CHANNEL 2
  #define YAW_CHANNEL 0
  #define FUN_CHANNEL 4
  #define SWITCH_CHANNEL 5
  //usart2 receiver
  u16 receiverBuf[6];
  void USART2_IRQHandler(void)
  {
    u8 *buf = bspData.usart2RecBuffer;
//    static u8 lastValue = 0;
    static u8 counter = 0;
    u8 data;
//    FUN_ISR_HEADER();
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
      USART_ClearITPendingBit(USART2, USART_IT_RXNE);
      data = USART_ReceiveData(USART2);
      //USART_SendData(USART3, data);
      if(data == 0x0f)
      {
        bspData.usart2RecBuffer[0] = 0x0f;
        counter = 1;
      }else
      {
        bspData.usart2RecBuffer[counter++] = data;
        if(counter == 25)
        {
          counter = 0;
          receiverBuf[0] = (buf[1] | ((u16)buf[2]<<8)) & 0x07ff;
          receiverBuf[1] = ((buf[2]>>3) | ((u16)buf[3]<<5)) & 0x07ff;
          receiverBuf[2] = ((buf[3]>>6) | ((u16)buf[4]<<2)| ((u16)buf[5]<<10)) & 0x07ff;
          receiverBuf[3] = ((buf[5]>>1) | ((u16)buf[6]<<7)) & 0x07ff;
          receiverBuf[4] = ((buf[6]>>4) | ((u16)buf[7]<<4)) & 0x07ff;
          receiverBuf[5] = ((buf[7]>>7) | ((u16)buf[8]<<1)| ((u16)buf[9]<<9)) & 0x07ff;
          
          //is the receiver value effect?
          if(buf[0] != 0x0f)
          {
            //errorw
          }
          
          //cp to uavData if normal
          uavData.receriverValue[0] = receiverBuf[ROLL_CHANNEL];
          uavData.receriverValue[1] = receiverBuf[PITCH_CHANNEL];
          uavData.receriverValue[2] = receiverBuf[OIL_CHANNEL]+300;
          uavData.receriverValue[3] = receiverBuf[YAW_CHANNEL];
          uavData.receriverValue[5] = receiverBuf[FUN_CHANNEL];
          uavData.receriverValue[4] = receiverBuf[SWITCH_CHANNEL];
          dogData.receiverDog++;
          

        }
      }
//      lastValue = data;
    }
//    FUN_ISR_FOOT();
  }
  
  //摇杆通道定义
//  #define ROLL_CHANNEL 2
//  #define PITCH_CHANNEL 4
//  #define OIL_CHANNEL 0
//  #define YAW_CHANNEL 5
//  #define FUN_CHANNEL 1
//  #define SWITCH_CHANNEL 3

  void TIM1_CC_IRQHandler(void)
  {
    if(TIM_GetITStatus(TIM1, TIM_IT_CC1) == SET)
    {
      TIM_ClearITPendingBit(TIM1, TIM_IT_CC1);
    }
  }
  void TIM2_IRQHandler(void)
  {
    if(TIM_GetITStatus(TIM2, TIM_IT_CC3) == SET)
    {
      TIM_ClearITPendingBit(TIM2, TIM_IT_CC3);
    }
  }
  
  void TIM4_IRQHandler(void)
  {
    OS_ERR err;
    static u8 counter=0,timeCounter = 0;
    FUN_ISR_HEADER();
    if(TIM_GetITStatus(TIM4, TIM_IT_Update) == SET)
    {
      TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
      OSTaskSemPost(&appData.appMainCycleTCB, OS_OPT_POST_NONE, &err);
      counter++;
      if(counter == 5)
      {
        OSTaskSemPost(&appData.appPositionCtlTCB, OS_OPT_POST_NONE, &err);
        counter = 0;
      }
      
      timeCounter++;
      if(timeCounter == 200)
      {
        timeCounter = 0;
        uavData.sysSecond++;
        if(uavData.sysSecond >= 60)
        {
          uavData.sysSecond = 0;
          uavData.sysMin++;
        }
        if(uavData.sysMin >= 60)
        {
          uavData.sysMin = 0;
          uavData.sysHour++;
        }
      }
      
    }
    
    
    FUN_ISR_FOOT();
  }
  
  void TIM3_IRQHandler(void)
  {    
    u32 captureValue;
    static u32 ic4ValueLast;
    if(TIM_GetITStatus(TIM3, TIM_IT_CC4) == SET)
    {
    TIM_ClearITPendingBit(TIM3, TIM_IT_CC4);
    captureValue = TIM_GetCapture4(TIM3);
    if(PBin(1))
    {

    }else
    {
      if(captureValue < ic4ValueLast)
      {
        bspData.tim3Capture = 0xffff - ic4ValueLast+ captureValue;
      }else
      {
        bspData.tim3Capture = captureValue - ic4ValueLast;
      }  
    }
      ic4ValueLast = captureValue;
    }
  }
  
  //来自上位机蓝牙的数据
  //wifi初始化信号过程
  const char wifiRecGotIp[] = "WIFI GOT IP";
  const char wifiRecConnect[] = "CONNECT";
  const char wifiRecMode[] = "OK";
  const char wifiRecSend[] = ">";
  u8 wifiRecCounter = 0;
	  u8 wifiMatchCounter = 0;
  u8 cmpWifiStr(u8 cmd,u8 data)
  {
    const char *p;
    switch(cmd)
    {
      case 0:
        p = wifiRecGotIp;
        break;
      case 1:
        p = wifiRecConnect;
        break;
      case 2:
        p = wifiRecMode;
        break;
      case 3:
        p = wifiRecSend;
        break;
    }
    if(p[wifiRecCounter]==data)
    {
      wifiMatchCounter++;
    }else
    {
      wifiMatchCounter = 0;
    }
    if(p[wifiMatchCounter] == 0)
    {
      wifiMatchCounter = 0;
      return 1;
      
    }else
    {
      return 0;
    }
  }
  u8 *usart3PtrMem;
  void USART1_IRQHandler(void)
  {
    u8 *ptrMem,data;
    static u8 counter= 0, wifiRecFlag = 0;
    OS_ERR err;
    FUN_ISR_HEADER();
    ptrMem = usart3PtrMem;
//		LED2=0;
//		OSTimeDlyHMSM(0, 0, 0, 1000, OS_OPT_TIME_HMSM_NON_STRICT, &err);
//		while(DEF_ON)
//		{
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
      USART_ClearITPendingBit(USART1, USART_IT_RXNE);
      data = USART_ReceiveData(USART1);
//      USART_SendData(USART3, data);
      if(uavData.wifiReady == 0)
      {
        if(cmpWifiStr(wifiRecFlag,data) == 1)
        {
          wifiRecFlag++;
          OSTaskSemPost(&appData.appSendBluetoothTCB, OS_OPT_POST_1, &err);
        }
        if(wifiRecFlag==4)
        {
          wifiRecFlag = 0;
        }
      }else if(uavData.uavStatue != 0)

//			{
//				if(uavData.wifiReady == 1)
//{
//	      OSTaskSemPost(&appData.appRunTestTCB,OS_OPT_POST_NONE,&err);
//}
//			}
			
			
 { 
        if(data == 0xff)
        {
          usart3PtrMem = OSMemGet(&appData.byte8Mem, &err);
          ptrMem = usart3PtrMem;
//					OSTaskQPost(&appData.appCmdHanderTCB, ptrMem, 8, OS_OPT_POST_FIFO, &err);

          if(err == OS_ERR_NONE)  
          {
            *ptrMem = data;
            counter=1;//
          }
        }else
        {
          *(ptrMem+counter) = data;
          counter++;
          if(counter == 8)
          {
            OSTaskQPost(&appData.appCmdHanderTCB, ptrMem, 8, OS_OPT_POST_FIFO, &err);
            ptrMem = NULL;
            counter = 0;
          }
        }
      }

    FUN_ISR_FOOT();
   
  }
}
  
  void CAN1_RX0_IRQHandler(void)
  {
    CAN_Receive(CAN1, CAN_FIFO0, &bspData.canRxMsg1);
  }
  
  
  void DMA2_Stream3_IRQHandler(void)
  {
    FUN_ISR_HEADER();
    SD_ProcessDMAIRQ();
    FUN_ISR_FOOT();
  }
  
  void SDIO_IRQHandler(void)
  {
    FUN_ISR_HEADER();
    SD_ProcessIRQSrc();
    FUN_ISR_FOOT();
  }
/**
  * @}
  */ 


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
