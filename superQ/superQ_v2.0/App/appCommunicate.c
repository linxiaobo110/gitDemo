#include "app.h"
float uavParaDefault;
u8 uavConfigDefault;
static float *uavPara[64] = {
  &uavData.angleVelocityPid[0][0],  &uavData.angleVelocityPid[0][1], &uavData.angleVelocityPid[0][2], 
  &uavData.angleVelocityPid[1][0],  &uavData.angleVelocityPid[1][1], &uavData.angleVelocityPid[1][2], 
  &uavData.angleVelocityPid[2][0],  &uavData.angleVelocityPid[2][1], &uavData.angleVelocityPid[2][2], 
  
  &uavData.anglePid[0][0],  &uavData.anglePid[0][1], &uavData.anglePid[0][2], 
  &uavData.anglePid[1][0],  &uavData.anglePid[1][1], &uavData.anglePid[1][2], 
  &uavData.anglePid[2][0],  &uavData.anglePid[2][1], &uavData.anglePid[2][2], 
  
  &uavData.posPid[0][0],  &uavData.posPid[0][1], &uavData.posPid[0][2], 
  &uavData.posPid[1][0],  &uavData.posPid[1][1], &uavData.posPid[1][2], 
  &uavData.posPid[2][0],  &uavData.posPid[2][1], &uavData.posPid[2][2], 
  
  &uavData.velocityPid[0][0],  &uavData.velocityPid[0][1], &uavData.velocityPid[0][2], 
  &uavData.velocityPid[1][0],  &uavData.velocityPid[1][1], &uavData.velocityPid[1][2], 
  &uavData.velocityPid[2][0],  &uavData.velocityPid[2][1], &uavData.velocityPid[2][2], 
  
  &uavData.otherPara[0], &uavData.otherPara[1], &uavData.otherPara[2], &uavData.otherPara[3],
  &uavData.otherPara[4], &uavData.otherPara[5], &uavData.otherPara[6], &uavData.otherPara[7],
  &uavData.otherPara[8], &uavData.otherPara[9], &uavData.otherPara[10], &uavData.otherPara[11],
  &uavData.otherPara[12], &uavData.otherPara[13], &uavData.otherPara[14], &uavData.otherPara[15],
  &uavData.otherPara[16], &uavData.otherPara[17], &uavData.otherPara[18], &uavData.otherPara[19],
  &uavParaDefault,&uavParaDefault,&uavParaDefault,&uavParaDefault,
  &uavParaDefault,&uavParaDefault,&uavParaDefault,&uavParaDefault,
};

static u8 *uavConfig[32] = {
  &uavConfigDefault, &uavConfigDefault, &uavConfigDefault, &uavConfigDefault,
  &uavConfigDefault, &uavConfigDefault, &uavConfigDefault, &uavConfigDefault,
  &uavConfigDefault, &uavConfigDefault, &uavConfigDefault, &uavConfigDefault,
  &uavConfigDefault, &uavConfigDefault, &uavConfigDefault, &uavConfigDefault,
  &uavConfigDefault, &uavConfigDefault, &uavConfigDefault, &uavConfigDefault,
  &uavConfigDefault, &uavConfigDefault, &uavConfigDefault, &uavConfigDefault,
  &uavConfigDefault, &uavConfigDefault, &uavConfigDefault, &uavConfigDefault,
  &uavData.e2promTrackNums[0], &uavData.e2promTrackNums[1], &uavData.e2promTrackNums[2], &uavData.e2promTrackNums[3]
};
//wifi模块特殊定义

#ifdef IS_PCHOST_SELECT_WIFI
  const char wifiCmdConServer[] = "AT+CIPSTART=\"TCP\",\"192.168.1.8\",9981\r\n";
  const char wifiCmdCipMode[] = "AT+CIPMODE=1\r\n";
  const char wifiCmdCipSend[] = "AT+CIPSEND\r\n";
  const char wifiCmdCipSendLenFix[] = "AT+CIPSEND=98\r\n";
  const char wifiCmdReset1[] = "+++";
  const char wifiCmdReset2[] =  "\r\nAT+RST\r\n";
  char wifiReturnBuffer[50];
  void usartSendStr(USART_TypeDef *usart, char const*p)
  {
    while((*p)!=0)
    {
      fun_usartSend(usart, *p);
      p++;
    }
  }
  void usartReadStr(USART_TypeDef *usart)
  {
    char *p;
    p = wifiReturnBuffer;
    *p = 0;
//    while(data!='\n')
//    {
//      data = fun_usartRec(USART3);
//      data =
//      p++;
//    }
    do
    {
      *p = fun_usartRec(USART3);
    }while((*p++)!='\n');
  }
#endif

#define BUFFER1_OFFSET 44
#define BUFFER2_OFFSET 88
#define BUFFER3_OFFSET 132
u8 wifiBuffer[176];
/********************************************************************************************************
Function Name: 通过串口向上位机发送数据
Author       : xiaobo
Date         : 2016-01-10
Description  : 
Inputs       : None
Outputs      : None
Notes        : 
Revision     : 
********************************************************************************************************/
void appSendBluetooth(void *parg)
{
  u8 i;
  OS_ERR err;
  CPU_TS ts;
  u8 paraCounter = 0;
  short tempShort;
  u32 crcTemp;
  DEVICE_DATA *device,*deviceTemp; 
  u8 buffer[44];
  //OSTaskSuspend(&appData.appSendBluetoothTCB,&err);
  //************************************************************
  // 初始化复杂硬件设备
  //************************************************************
  #ifdef IS_PCHOST_SELECT_WIFI
  uavData.wifiReady = 0;
  device = &(uavData.wifiDevice);
  device->enable = 1;
  device->startStatue = 0;
  device->errStatue = WIFI_ERR_NONE;
  if(device->enable)
  {
    //重启
    usartSendStr(USART3, wifiCmdReset1);
    OSTimeDly(50,OS_OPT_TIME_DLY, &err);
    usartSendStr(USART3, wifiCmdReset2);
    //等待连接路由器
    OSTaskSemPend(10000,OS_OPT_PEND_BLOCKING,&ts,&err);
    
    //连接服务器
    if(err == OS_ERR_NONE)
    {
      OSTimeDly(200,OS_OPT_TIME_DLY, &err);
      usartSendStr(USART3, wifiCmdConServer);
    }else
    {
      device->errStatue = WIFI_ERR_CONROUTER;
    }
    
    //设置透传模式
    if(device->errStatue == WIFI_ERR_NONE)
    {
      OSTaskSemPend(3000,OS_OPT_PEND_BLOCKING,&ts,&err);
      if(err == OS_ERR_NONE)
      {
        OSTimeDly(100,OS_OPT_TIME_DLY, &err);
        usartSendStr(USART3, wifiCmdCipMode);
      }else
      {
        device->errStatue = WIFI_ERR_CONSERVER;
      }
    }
    
    //开启透传 
    if(device->errStatue == WIFI_ERR_NONE)
    {
      OSTaskSemPend(3000,OS_OPT_PEND_BLOCKING,&ts,&err);
      if(err == OS_ERR_NONE)
      {
        OSTimeDly(100,OS_OPT_TIME_DLY, &err);
        usartSendStr(USART3, wifiCmdCipSend);
      }else
      {
        device->errStatue = WIFI_ERR_MODESET;
      }
    }
    
    //收到透传启动指令
    if(device->errStatue == WIFI_ERR_NONE)
    {
      OSTaskSemPend(3000,OS_OPT_PEND_BLOCKING,&ts,&err);
      if(err == OS_ERR_NONE)
      {
        OSTimeDly(100,OS_OPT_TIME_DLY, &err);
        uavData.wifiReady = 1;
      }
    }
  }
  if(device->errStatue == WIFI_ERR_NONE)
  {
    uavData.wifiDevice.startStatue = 1;
  }else
  {
    uavData.wifiDevice.startStatue = 0;
    OSTaskSuspend(&appData.appSendBluetoothTCB, &err);
    USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);
  }
  
  //OSTaskSuspend(&appData.appSendBluetoothTCB, &err);
  #endif
  while(DEF_ON)
  {
    if( (device->enable) && (device->errStatue == WIFI_ERR_NONE) )
    {
      /****************************************
      * 状态包
      ****************************************/
      /* 包头 */
      buffer[0] = 0xff;
      buffer[1] = 0xa5;
      /* 指令 */
      buffer[2] = 0x42;
      /* 包长 */
      buffer[3] = 36;
      /* 无人机状态 */
      buffer[4] = uavData.uavStatue;
      /* 电池电压 */
      buffer[5] = (u8)( (uavData.powerVotage + 0.05f) * 10);
      /* 摇杆 */ 
      buffer[6] = uavData.receriverValue[0] >> 8;
      buffer[7] = uavData.receriverValue[0] & 0xff;
      buffer[8] = uavData.receriverValue[1] >> 8;
      buffer[9] = uavData.receriverValue[1] & 0xff;
      buffer[10] = uavData.receriverValue[2] >> 8;
      buffer[11] = uavData.receriverValue[2] & 0xff;
      buffer[12] = uavData.receriverValue[3] >> 8;
      buffer[13] = uavData.receriverValue[3] & 0xff;
      /* 电机 */ 
      buffer[14] = uavData.motorPwm[0] >> 8;
      buffer[15] = uavData.motorPwm[0] & 0xff;
      buffer[16] = uavData.motorPwm[1] >> 8;
      buffer[17] = uavData.motorPwm[1] & 0xff;
      buffer[18] = uavData.motorPwm[2] >> 8;
      buffer[19] = uavData.motorPwm[2] & 0xff;
      buffer[20] = uavData.motorPwm[3] >> 8;
      buffer[21] = uavData.motorPwm[3] & 0xff;
      /* 时间 */ 
      if(uavData.timeOrDate == 0)
      {
        buffer[22] = uavData.sysHour;
        buffer[23] = uavData.sysMin;
        buffer[24] = uavData.sysSecond;
      }
      
      /* 调试模式选择/时间选择 */
      buffer[25] = uavData.uavDubugMode<<4;
      //buffer[25] = 0;
      /* 外设信息 */
      deviceTemp = &uavData.imuDevice;
      buffer[26] = (deviceTemp->errStatue<<4) | (deviceTemp->workStatue<<2) | (deviceTemp->startStatue<<1) | deviceTemp->enable;
      deviceTemp = &uavData.altitudeSensorDevice;
      buffer[27] = (deviceTemp->errStatue<<4) | (deviceTemp->workStatue<<2) | (deviceTemp->startStatue<<1) | deviceTemp->enable;
      deviceTemp = &uavData.positionSensorDevice;
      buffer[28] = (deviceTemp->errStatue<<4) | (deviceTemp->workStatue<<2) | (deviceTemp->startStatue<<1) | deviceTemp->enable;
      deviceTemp = &uavData.e2promDevice;
      buffer[29] = (deviceTemp->errStatue<<4) | (deviceTemp->workStatue<<2) | (deviceTemp->startStatue<<1) | deviceTemp->enable;
      deviceTemp = &uavData.tfCardDevice;
      buffer[30] = (deviceTemp->errStatue<<4) | (deviceTemp->workStatue<<2) | (deviceTemp->startStatue<<1) | deviceTemp->enable;
      deviceTemp = &uavData.ledBellDevice;
      buffer[31] = (deviceTemp->errStatue<<4) | (deviceTemp->workStatue<<2) | (deviceTemp->startStatue<<1) | deviceTemp->enable;
      deviceTemp = &uavData.overflowDevice;
      buffer[32] = (deviceTemp->errStatue<<4) | (deviceTemp->workStatue<<2) | (deviceTemp->startStatue<<1) | deviceTemp->enable;
      deviceTemp = &uavData.wifiDevice;
      buffer[33] = (deviceTemp->errStatue<<4) | (deviceTemp->workStatue<<2) | (deviceTemp->startStatue<<1) | deviceTemp->enable;
      buffer[34] = 0;
      buffer[35] = 0;
      buffer[36] = 0;
      buffer[37] = 0;
      buffer[38] = 0;
      buffer[39] = 0;
      /* CRC校验 */
      crcTemp = crc32Calculate(buffer+4,0,36);
//      CRC_ResetDR();
//      crcTemp = CRC_CalcBlockCRC((u32 *)(buffer+4), 9);
      buffer[40] = crcTemp>>24;
      buffer[41] = (crcTemp>>16)&0xff;
      buffer[42] = (crcTemp>>8)&0xff;
      buffer[43] = crcTemp&0xff;
      /* 填充缓冲区 */
      for(i=0;i<44;i++)
      {
        wifiBuffer[i] = buffer[i];
      }
      
      /****************************************
      * 姿态信息包
      ****************************************/
      /* 包头 */
      buffer[0] = 0xff;
      buffer[1] = 0xa5;
      /* 指令 */
      buffer[2] = 0x44;
      /* 包长 */
      buffer[3] = 36;
      
      /* 角度X-Y-Z设定-测量-积分 */
      for(i=0;i<3;i++)
      {
        tempShort = (short)(uavData.refAngle[i] * 100);
        buffer[4 + i*6] = tempShort >> 8;
        buffer[5 + i*6] = tempShort & 0xff;
        tempShort = (short)(imuData.angle[i] * 100);
        buffer[6 + i*6] = tempShort >> 8;
        buffer[7 + i*6] = tempShort & 0xff;
        tempShort = (short)(uavData.specialAngle2[i] * 100);
        buffer[8 + i*6] = tempShort >> 8;
        buffer[9 + i*6] = tempShort & 0xff;
      }
      
      /* 角速度X-Y-Z设定-测量-积分 */
      for(i=0;i<3;i++)
      {
        tempShort = (short)(uavData.refAngleVelocity[i] * 100);
        buffer[22 + i*6] = tempShort >> 8;
        buffer[23 + i*6] = tempShort & 0xff;
        tempShort = (short)(imuData.angleV[i] * 100);
        buffer[24 + i*6] = tempShort >> 8;
        buffer[25 + i*6] = tempShort & 0xff;
        tempShort = (short)(uavData.specialAngle1[i] * 100);
        buffer[26 + i*6] = tempShort >> 8;
        buffer[27 + i*6] = tempShort & 0xff;
      }
      /* CRC校验 */
     crcTemp = crc32Calculate(buffer+4,0,36);
//      CRC_ResetDR();
//      crcTemp = CRC_CalcBlockCRC((u32 *)(buffer+4), 9);
      buffer[40] = crcTemp>>24;
      buffer[41] = (crcTemp>>16)&0xff;
      buffer[42] = (crcTemp>>8)&0xff;
      buffer[43] = crcTemp&0xff;
      /* 填充缓冲区 */
      for(i=0;i<44;i++)
      {
        wifiBuffer[i + BUFFER1_OFFSET] = buffer[i];
      }
      
      
      /****************************************
      * 位置信息包
      ****************************************/
      /* 包头 */
      buffer[0] = 0xff;
      buffer[1] = 0xa5;
      /* 指令 */
      buffer[2] = 0x46;
      /* 包长 */
      buffer[3] = 36;
      /* 位置X-Y-Z设定-测量-积分 */
      for(i=0;i<3;i++)
      {
        tempShort = (short)(uavData.refPosition[i]);
        buffer[4 + i*6] = tempShort >> 8;
        buffer[5 + i*6] = tempShort & 0xff;
        tempShort = (short)(uavData.position[i]);
        buffer[6 + i*6] = tempShort >> 8;
        buffer[7 + i*6] = tempShort & 0xff;
        tempShort = (short)(uavData.specialPosition[i]);
        buffer[8 + i*6] = tempShort >> 8;
        buffer[9 + i*6] = tempShort & 0xff;
      }
      /* 速度X-Y-Z设定-测量-积分 */
      for(i=0;i<3;i++) 
      {
        tempShort = (short)(uavData.refVelocity[i] * 10);
        buffer[22 + i*6] = tempShort >> 8;
        buffer[23 + i*6] = tempShort & 0xff;
        tempShort = (short)(imuData.positionV[i] * 10);
        buffer[24 + i*6] = tempShort >> 8;
        buffer[25 + i*6] = tempShort & 0xff;
        tempShort = (short)(uavData.specialVelocity[i]);
        buffer[26 + i*6] = tempShort >> 8;
        buffer[27 + i*6] = tempShort & 0xff;
      }
      /* CRC校验 */
      crcTemp = crc32Calculate(buffer+4,0,36);
//      CRC_ResetDR();
//      crcTemp = CRC_CalcBlockCRC((u32 *)(buffer+4), 9);
      buffer[40] = crcTemp>>24;
      buffer[41] = (crcTemp>>16)&0xff;
      buffer[42] = (crcTemp>>8)&0xff;
      buffer[43] = crcTemp&0xff;
      /* 填充缓冲区 */
      for(i=0;i<44;i++)
      {
        wifiBuffer[i + BUFFER2_OFFSET] = buffer[i];
      }
      
      
      /****************************************
      * 参数信息包
      ****************************************/
      /* 包头 */
      buffer[0] = 0xff;
      buffer[1] = 0xa5;
      /* 指令 */
      buffer[2] = 0x48;
      /* 包长 */
      buffer[3] = 36;
      /* 参数ID */
      for(i=0;i<8;i++)
      {
        buffer[4+i] = paraCounter + i;
      }
      /* 参数值 */
      for(i=0;i<8;i++)
      {
        tempShort = (*uavPara[paraCounter + i]) * 100;
        buffer[12 + i*2] = tempShort >> 8;
        buffer[13 + i*2] = tempShort & 0xff;
      }
      //每次发送的参数ID变化
      paraCounter += 8;
      if(paraCounter >= 64)
        paraCounter = 0;
      /* CRC校验 */
      crcTemp = crc32Calculate(buffer+4,0,36);
//      CRC_ResetDR();
//      crcTemp = CRC_CalcBlockCRC((u32 *)(buffer+4), 9);
      buffer[40] = crcTemp>>24;
      buffer[41] = (crcTemp>>16)&0xff;
      buffer[42] = (crcTemp>>8)&0xff;
      buffer[43] = crcTemp&0xff;
      /* 填充缓冲区 */
      for(i=0;i<28;i++)
      {
        wifiBuffer[i + BUFFER3_OFFSET] = buffer[i];
      }
      //清零标记
      if(appData.monitorHasSend == 1)
      {
        appData.monitorHasSend = 0;
        wifiBuffer[28 + BUFFER3_OFFSET]  = 1;
      }else
      {
        wifiBuffer[28 + BUFFER3_OFFSET]  = 0;
      }

      DMA1_Stream3->NDTR = 176;
      DMA1_Stream3->M0AR = (u32)wifiBuffer;
      DMA_ClearFlag(DMA1_Stream3, DMA_FLAG_TCIF3);//查询发送需要此条
      DMA_Cmd(DMA1_Stream3, ENABLE);
      USART_DMACmd(USART3, USART_DMAReq_Tx, ENABLE);
      OSTimeDlyHMSM(0, 0, 0, 33, OS_OPT_TIME_HMSM_NON_STRICT, &err);
      OSSemPost(&appData.monitorFlag, OS_OPT_POST_1, &err);
    }else
    {
      if(device->errStatue != WIFI_ERR_NONE)
      {
        OSTaskSuspend(&appData.appSendBluetoothTCB, &err);
      }else
      {
        OSTimeDlyHMSM(0, 0, 0, 500, OS_OPT_TIME_HMSM_NON_STRICT, &err);
      }
    }
    
  }

}

void displayMonitor(char *p)
{
  u8 i = 0;
  OS_ERR err;
  CPU_TS ts;
  appData.monitorHasSend = 1;
  OSSemPend(&appData.monitorFlag, 0, OS_OPT_PEND_BLOCKING, &ts, &err);
  while( (i<11) && (*p)!=0)
  {
    wifiBuffer[28 + i + BUFFER3_OFFSET] = *p;
    p++;
    i++;
  }
  if(i<11)
  {
    wifiBuffer[i] = 0;
  }
}

u8 saveBuffer1[32];
u8 saveBuffer2[32];
/********************************************************************************************************
Function Name: 指令解析器，E2PROM读写
Author       : xiaobo
Date         : 2016-01-10
Description  : 
Inputs       : None
Outputs      : None
Notes        : 
Revision     : 
********************************************************************************************************/
void appCmdHander(void *parg)
{
  CPU_TS ts;
  OS_ERR err;
  OS_MSG_SIZE msgSize;
  u8 *ptrMem, choice;
  u16 recValue;
  u8 i;
  u8 checkSum;
  short temp;
  DEVICE_DATA *device;
  device = &(uavData.e2promDevice);
  
  device->enable = 1;
  if(device->enable)
  {
    at25320_readBuffer(0, saveBuffer1, 32);
    for(i=0;i<16;i++)
    {
      temp = saveBuffer1[ i*2 ];
      temp = (temp<<8) + saveBuffer1[ i*2 + 1];
      *uavPara[i] = temp * 0.01f;
    }
    
    at25320_readBuffer(32, saveBuffer1, 32);
    for(i=0;i<16;i++)
    {
      temp = saveBuffer1[ i*2 ];
      temp = (temp<<8) + saveBuffer1[ i*2 + 1];
      *uavPara[i + 16] = temp * 0.01f;
    }
    
    at25320_readBuffer(64, saveBuffer1, 32);
    for(i=0;i<16;i++)
    {
      temp = saveBuffer1[ i*2 ];
      temp = (temp<<8) + saveBuffer1[ i*2 + 1];
      *uavPara[i + 32] = temp * 0.01f;
    }
    
    at25320_readBuffer(96, saveBuffer1, 32);
    for(i=0;i<16;i++)
    {
      temp = saveBuffer1[ i*2 ];
      temp = (temp<<8) + saveBuffer1[ i*2 + 1];
      *uavPara[i + 48] = temp * 0.01f;
    }
    
    //读取系统配置
    at25320_readBuffer(128, saveBuffer1, 32);
    for(i=0;i<32;i++)
    {
      *uavConfig[i] = saveBuffer1[i];
    }
    if( (uavData.e2promTrackNums[0] == 0xa1) && (uavData.e2promTrackNums[1] == 0xa2) && (uavData.e2promTrackNums[2] == 0xb1) && (uavData.e2promTrackNums[3] == 0xb2) )
    {
      device->errStatue = 0;
    }else
    {
      //标记检测不符错误
      device->errStatue = 2;
      //写入标记
      uavData.e2promTrackNums[0] = 0xa1;
      uavData.e2promTrackNums[1] = 0xa2;
      uavData.e2promTrackNums[2] = 0xb1;
      uavData.e2promTrackNums[3] = 0xb2;
    }
    if(device->errStatue == 0)
    {
      device->startStatue = 1;
    }else
    {
      device->startStatue = 0;
    }
  }
  //等待启动完成
  while(uavData.uavStatue == 0)
  {
    OSTimeDly(100, OS_OPT_TIME_DLY, &err);    
  }
  while(DEF_ON)
  {
    //接收到来自上位机的指令，开始处理
    ptrMem = OSTaskQPend(0, OS_OPT_PEND_BLOCKING, &msgSize, &ts, &err);
    checkSum = 0;
    for(i=0;i<7;i++)
    {
      checkSum += *(ptrMem+i);
    }
    //若校验通过
    if(checkSum == *(ptrMem+7))
    {
        if(  *(ptrMem+1) == 0x41 )
        {
    //      uavData.position[0] = (short)((*(ptrMem+2)<<7) | *(ptrMem+3)) - 8000;
    //      uavData.position[1] = (short)((*(ptrMem+4)<<7) | *(ptrMem+5)) - 8000;
    //      imuData.positionV[1] = (short)((*(ptrMem+6)<<7) | *(ptrMem+7)) - 8000;
        }
        //下位机参数修改指令
        else if( *(ptrMem+1) == 0x43 )
        {
          if(uavData.uavStatue == 2)
          {
            choice = *(ptrMem + 2);
            recValue = *(u16 *)(ptrMem + 4);
            //printf("rec%d %d\n",choice,recValue);
            //roll kp ki kd
            if(choice < 27)
            {
              *uavPara[choice] = recValue*0.01f;
            }else if(choice < 36)
            {
              *uavPara[choice] = recValue*0.01f;
              //appData.paraS[choice - 31] = (int)recValue - 32500;
            }else if(choice < 64)
            {
              //appData.paraB[choice - 35] = recValue;
              *uavPara[choice] = recValue*0.01f;
            }else
            {
                
              
            }
          }else
          {
            OSTaskQPost(&appData.appBuzzerTCB, 0, 11, OS_OPT_POST_1, &err);
          }
          
        }
        //下位机参数保存至flash指令
        else if( *(ptrMem+1) == 0x45 )
        {
          //OSTaskSemPost(&appData.appBlackBoxTCB, OS_OPT_POST_NONE, &err);
          if((uavData.uavStatue == 2) && (*(ptrMem+2)==0x33))
          {
            //参数保存
            for(i=0;i<16;i++)
            {
              temp = (short)(*uavPara[i]*100);
              saveBuffer1[ i*2 ] = temp >> 8;
              saveBuffer1[ i*2 + 1] = temp & 0xff;
            }
            at25320_WREN();
            at25320_writeBuffer(0, saveBuffer1, 32);
            at25320_WRDI();
            
            for(i=0;i<16;i++)
            {
              temp = (short)(*uavPara[i + 16]*100);
              saveBuffer1[ i*2 ] = temp >> 8;
              saveBuffer1[ i*2 + 1] = temp & 0xff;
            }
            OSTimeDly(20, OS_OPT_TIME_DLY, &err);
            at25320_WREN();
            at25320_writeBuffer(32, saveBuffer1, 32);
            at25320_WRDI();
            
            for(i=0;i<16;i++)
            {
              temp = (short)(*uavPara[i + 32]*100);
              saveBuffer1[ i*2 ] = temp >> 8;
              saveBuffer1[ i*2 + 1] = temp & 0xff;
            }
            OSTimeDly(20, OS_OPT_TIME_DLY, &err);
            at25320_WREN();
            at25320_writeBuffer(64, saveBuffer1, 32);
            at25320_WRDI();
            
            for(i=0;i<16;i++)
            {
              temp = (short)(*uavPara[i + 48]*100);
              saveBuffer1[ i*2 ] = temp >> 8;
              saveBuffer1[ i*2 + 1] = temp & 0xff;
            }
            OSTimeDly(20, OS_OPT_TIME_DLY, &err);
            at25320_WREN();
            at25320_writeBuffer(96, saveBuffer1, 32);
            at25320_WRDI();
            
            //系统配置保存
            for(i=0;i<32;i++)
            {
              saveBuffer1[i] = *uavConfig[i];
            }
            OSTimeDly(20, OS_OPT_TIME_DLY, &err);
            at25320_WREN();
            at25320_writeBuffer(128, saveBuffer1, 32);
            at25320_WRDI();
            OSTaskQPost(&appData.appBuzzerTCB, 0, 2, OS_OPT_POST_1, &err);
          }else
          {
            OSTaskQPost(&appData.appBuzzerTCB, 0, 11, OS_OPT_POST_1, &err);
          }
        }
        //传感器校正指令
        else if( *(ptrMem+1) == 0x47 )
        {
    //      if(inertiaData.gyroCalFlag == 40000 && inertiaData.magnCalFlag == 40000)
    //      {
    //        switch(*(ptrMem+2))
    //        {
    //          case 0:
    //            LED1 = 0;
    //            inertiaData.gyroCalFlag = 0;
    //            break;
    //          case 1:
    //            LED1 = 0;
    //            inertiaData.magnCalFlag = 0;
    //            break;
    //        }
    //      }
        }
        //时间同步指令
        else if( *(ptrMem+1) == 0x49 )
        {
            if(*(ptrMem + 2) == 0)
            {
              uavData.sysHour = *(ptrMem + 3);
              uavData.sysMin = *(ptrMem + 4);
              uavData.sysSecond = *(ptrMem + 5);
            }
        }
        //调试模式/数据包选择
        else if( *(ptrMem+1) == 0x51 )
        {
          uavData.uavDubugMode = (UAV_DEBUG_MODE)*(ptrMem+2);
        }
    }
 
    //释放空间
    OSMemPut(&appData.byte8Mem, ptrMem, &err);
    if(err != OS_ERR_NONE)
    {
      device->errStatue = 4;
    }
  }
  
}

//===========================================  End Of File  ===========================================//
