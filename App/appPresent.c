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
#include "App.h"
/*---------------------------------------  D E F I N I T I O N  ---------------------------------------*/
SONAR_TEST sonarTest;
#define BUZZER_TIM TIM9
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
void appBuzzer(void *p_arg) 
{
  u8 i;
  u8 times;
  CPU_TS ts;
  OS_ERR err;
  while(DEF_ON)
  {
    OSTaskQPend(0, OS_OPT_PEND_BLOCKING, (OS_MSG_SIZE *)&times, &ts, &err);
    if(times < 10)
    {
      for(i=0;i<times;i++)
      {
        TIM_SetCompare1(BUZZER_TIM, 250);
        OSTimeDlyHMSM(0, 0, 0, 120, OS_OPT_TIME_HMSM_NON_STRICT, &err);
        TIM_SetCompare1(BUZZER_TIM, 0);
        OSTimeDlyHMSM(0, 0, 0, 80, OS_OPT_TIME_HMSM_NON_STRICT, &err);
      }
    }else if( times < 20)
    {
      times = times - 10;
      for(i=0;i<times;i++)
      {
        TIM_SetCompare1(BUZZER_TIM, 250);
        OSTimeDlyHMSM(0, 0, 0, 800, OS_OPT_TIME_HMSM_NON_STRICT, &err);
        TIM_SetCompare1(BUZZER_TIM, 0);
        OSTimeDlyHMSM(0, 0, 0, 400, OS_OPT_TIME_HMSM_NON_STRICT, &err);
      }
    }
    
  }
}

/********************************************************************************************************
Function Name: TF卡存储的任务
Author       : xiaobo
Date         : 2016-09-03
Description  : 暂阶段只支持普通包
Inputs       : None
Outputs      : None
Notes        : 
Revision     : 
********************************************************************************************************/
FRESULT res;
char tfCardBuffer[128];
 void appBlackBox(void *parg)
{
  OS_ERR err;
  int fileNumber;
  u16 i;
  //测试用
  u8 isWriteFlag;//只有这个标记置一，才开始写操作
  //读取的字节数
  u32 btr; 
  char fileName[15];
//  CPU_TS ts;
  FIL fs;
  FATFS ffs;
  FILINFO fInfo;

  DEVICE_DATA *device;
//  char temp[20];
  //delay start
  fun_delayMs(1500);
  device = &(uavData.tfCardDevice);
  
  device->enable = 1;
  device->workStatue = 1;
  //硬件初始化
  if(device->enable == 1)
  {
    if(Bsp_sdioInit() == SD_OK)
    {
      device->startStatue = 1;
    }else
    {
      device->startStatue = 0;
    }
  }else
  {
    OSTaskSuspend(&appData.appBlackBoxTCB, &err);
  }
  //稍微延时
  fun_delayMs(3000);
    
  //SDIO底层测试
  //SD_EraseTest();
  //SD_SingleBlockTest();
  //SD_MultiBlockTest();
  
  //修改配置文件及创建新文件
  if((device->enable == 1) && (device->startStatue == 1))
  {
    //挂载SD卡
    if(res == FR_OK)
    {
      res = f_mount(&ffs, "1:",1);
    }else
    {
      device->workStatue = 0;
      device->errStatue = 1;
    }
    
    //检查配置文件是否存在
    if(res == FR_OK)
    {
      res = f_stat("1://set.txt", &fInfo);
    }else
    {
      device->workStatue = 0;
      device->errStatue = 1;
    }
    
    //不存在则新建
    if(res == FR_NO_FILE)
    {
      res = f_open(&fs, "1://set.txt", FA_WRITE|FA_OPEN_ALWAYS);
      if(res == FR_OK)
      {
        //res = f_printf(&fs,"no:%d",0);
        btr = sprintf(fileName, "no:%d", 0);
        res = f_write(&fs,fileName,btr,(void *)0);
        fileNumber = 0;
      }else
      {
        device->workStatue = 0;
        device->errStatue = 1;
      }
      if(res == FR_OK)
      {
        res = f_close(&fs);
      }else
      {
        device->workStatue = 0;
        device->errStatue = 1;
      }
    }
    //存在则要先读再写
    else if(res == FR_OK)
    {
      res = f_open(&fs, "1://set.txt", FA_WRITE|FA_OPEN_ALWAYS|FA_READ);
      if(res == FR_OK)
      {
        res = f_read(&fs, tfCardBuffer, 10, &btr);
      }else
      {
        device->workStatue = 0;
        device->errStatue = 1;
      }
      if(res == FR_OK)
      {
        fileNumber = 0;
        for(i=3;i<btr;i++)
        {
          if(tfCardBuffer[i]>'0' && tfCardBuffer[i]<'9')
          {
            fileNumber = fileNumber*10 + tfCardBuffer[i] - '0';
          }
          else
          {
            break;
          }
          
        }
        res = f_lseek(&fs,0);
      }
      if(res == FR_OK)
      {
        fileNumber++;
        btr = sprintf(fileName, "no:%d", fileNumber);
        res = f_write(&fs,fileName,btr,(void *)0);
        //res = f_printf(&fs,"no:%d",fileNumber);
      }
      if(res == FR_OK)
      {
        res = f_close(&fs);
      }
      else
      {
        device->workStatue = 0;
        device->errStatue = 1;
      }
    }

    
    //把编号变成文件名
    getFileNameFromNumber(fileNumber,fileName);
    
    if(res == FR_OK)
    {
      //res = f_open(&fs, fileName, FA_WRITE|FA_CREATE_ALWAYS);
    }
    else
    {
      device->workStatue = 0;
      device->errStatue = 1;
    }
  }else
  {
    OSTaskSuspend(&appData.appBlackBoxTCB, &err);
  }
  
//  if(res == FR_OK)
//  {
//    res = f_write(&fs, "hello word,wo kaka ka xiaoxiao", 29, (void *)0);
//  }
//  if(res == FR_OK)
//  {
//    res = f_close(&fs);
//  }
  isWriteFlag = 0;
  while(1)
  {
    if((device->workStatue > 0 ) && (device->errStatue == 0) && (res == FR_OK))
    {
      if((uavData.uavStatue != UAV_STATUE_IDLE) && (uavData.uavStatue != UAV_STATUE_WAIT) && (isWriteFlag == 0))
      {
        isWriteFlag = 1;
        device->workStatue  = 2;
        //修改配置文件
        res = f_open(&fs, "1://set.txt", FA_WRITE|FA_OPEN_ALWAYS);
        if(res == FR_OK)
        {
          btr = sprintf(fileName, "no:%d", fileNumber);
          res = f_write(&fs,fileName,btr,(void *)0);
        }else
        {
          device->workStatue = 0;
          device->errStatue = 1;
        }
        OSTimeDlyHMSM(0, 0, 0, 10, OS_OPT_TIME_HMSM_NON_STRICT, &err);
        if(res == FR_OK)
        {
          res = f_close(&fs);
        }else
        {
          device->workStatue = 0;
          device->errStatue = 1;
        }
        OSTimeDlyHMSM(0, 0, 0, 10, OS_OPT_TIME_HMSM_NON_STRICT, &err);
        //打开新的文件
        getFileNameFromNumber(fileNumber,fileName);
        res = f_open(&fs, fileName, FA_WRITE|FA_CREATE_ALWAYS);
        OSTimeDlyHMSM(0, 0, 0, 10, OS_OPT_TIME_HMSM_NON_STRICT, &err);
      }    
      if(isWriteFlag == 1)
      {
        if(uavData.uavStatue == UAV_STATUE_IDLE)
        {
          isWriteFlag = 0;
          device->workStatue  = 1;
          res = f_close(&fs);
          fileNumber++;
          OSTimeDlyHMSM(0, 0, 0, 10, OS_OPT_TIME_HMSM_NON_STRICT, &err);
        }else
        {
       //   btr = sprintf(tfCardBuffer,"%d\t%d\t%d\t%d\t\n",sonarTest.sonarMea, sonarTest.natnetMea, imuData.position[2],imuData.positionV[2]);
           btr = sprintf(tfCardBuffer,"%d\t%.2f\t%d\t%.2f\n",imuData.position[0], imuData.positionV[0],imuData.position[1], imuData.positionV[1]);
          //t%d\t%d\n", sonarTest.sonarMea, sonarTest.natnetMea, imuData.position[2],imuData.positionV[2]);
          //res = f_printf(&fs,"%.1f %.1f\n",imuData.angle[0],imuData.angle[1]);
          res = f_write(&fs, tfCardBuffer, btr, (void *)0);
        }
      }
      
      if(res!= FR_OK)
      {
        device->workStatue = 0;
        device->errStatue = 2;
      }
    }
    OSTimeDlyHMSM(0, 0, 0, 25, OS_OPT_TIME_HMSM_NON_STRICT, &err);
  }
}

void getFileNameFromNumber(int number, char *name)
{
  u8 i = 0;
  //赋初值
  for(i=4;i<10;i++)
  {
    name[i] = 0;
  }
  i=0;
  name[4] = '0';
  name[0] = '1';
  name[1] = ':';
  name[2] = '/';
  name[3] = '/';
  
  while(number > 0)
  {
    name[i+4] = number%10 + '0';
    i++;
    number = number/10;
  }
  
  #define FILENAME_ABR 4
  //交换字母的位置，不超过3位的情况
  switch(i)
  {
    case 2:
      name[5+FILENAME_ABR] = name[0+FILENAME_ABR];
      name[0+FILENAME_ABR] = name[1+FILENAME_ABR];
      name[1+FILENAME_ABR] = name[5+FILENAME_ABR];
      break;
    case 3:
      name[5+FILENAME_ABR] = name[0+FILENAME_ABR];
      name[0+FILENAME_ABR] = name[2+FILENAME_ABR];
      name[2+FILENAME_ABR] = name[5+FILENAME_ABR];
      break;
    case 4:
      name[5+FILENAME_ABR] = name[0+FILENAME_ABR];
      name[0+FILENAME_ABR] = name[3+FILENAME_ABR];
      name[3+FILENAME_ABR] = name[5+FILENAME_ABR];
      name[5+FILENAME_ABR] = name[1+FILENAME_ABR];
      name[1+FILENAME_ABR] = name[2+FILENAME_ABR];
      name[2+FILENAME_ABR] = name[5+FILENAME_ABR];
      break;
  } 
}


//===========================================  End Of File  ===========================================//
