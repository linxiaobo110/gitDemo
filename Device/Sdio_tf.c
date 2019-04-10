/********************************************************************************************************
**                                 Copyright (c)          MEI14
**                                                 All Rights Reserved
**  
**                                 Email:729527658@qq.com   QQ:729527658
**-------------------------------------------------------------------------------------------------------
**  MCU        : STM32F429ZI (STMicroelectronics)
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
#include "Sdio_tf.h"
/*---------------------------------------  D E F I N I T I O N  ---------------------------------------*/


static u32 cardType = SDIO_STD_CAPACITY_SD_CARD_V1_1;
static u32 CSD_Tab[4], CID_Tab[4], RCA = 0;
static uint8_t SDSTATUS_Tab[16];
SD_CardInfo sdCardInfo;
//用于SD卡传输的标志
__IO uint32_t StopCondition = 0;
__IO SD_Error TransferError = SD_OK;
__IO uint32_t TransferEnd = 0, DMAEndOfTransfer = 0;



SDIO_InitTypeDef sdioInitStructure;
SDIO_CmdInitTypeDef sdioCmdInitStructure;
SDIO_DataInitTypeDef sdioDataInitStructure;

/*----------------------------------------  F U N C T I O N S  ----------------------------------------*/
/********************************************************************************************************
Function Name: 
Author       : xiaobo
Date         : 2015-03-10
Description  : 
Inputs       : None
Outputs      : None
Notes        : 
Revision     : 
********************************************************************************************************/
SD_Error Bsp_sdioInit(void)
{
  __IO SD_Error errorstatus = SD_OK;
  
  /*!< Disable SDIO Clock */
  SDIO_ClockCmd(DISABLE);
  
  /*!< Set Power State to OFF */
  SDIO_SetPowerState(SDIO_PowerState_OFF);
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SDIO, ENABLE);
    /* Enable the DMA2 Clock */
  RCC_AHB1PeriphClockCmd(SD_SDIO_DMA_CLK, ENABLE);
  SDIO_DeInit();
  errorstatus = SD_powerOn();
  printf("powerOn %u\n",errorstatus);
  if(errorstatus != SD_OK)
  {
    return errorstatus;
  }
  
  errorstatus = SD_InitializeCards();
  printf("iniCard %u\n",errorstatus);
  if(errorstatus != SD_OK)
  {
    return (errorstatus);
  }
  
  /*!< Configure the SDIO peripheral */
  /*!< SDIO_CK = SDIOCLK / (SDIO_TRANSFER_CLK_DIV + 2) */
  /*!< on STM32F4xx devices, SDIOCLK is fixed to 48MHz */
  sdioInitStructure.SDIO_ClockDiv = SDIO_TRANSFER_CLK_DIV;
  sdioInitStructure.SDIO_ClockEdge = SDIO_ClockEdge_Rising;
  sdioInitStructure.SDIO_ClockBypass = SDIO_ClockBypass_Disable;
  sdioInitStructure.SDIO_ClockPowerSave = SDIO_ClockPowerSave_Disable;
  sdioInitStructure.SDIO_BusWide = SDIO_BusWide_1b;
  sdioInitStructure.SDIO_HardwareFlowControl = SDIO_HardwareFlowControl_Disable;
  SDIO_Init(&sdioInitStructure);
  
  /*----------------- Read CSD/CID MSD registers ------------------*/
  errorstatus = SD_getCardInfo(&sdCardInfo);
  printf("cardInfo %u\n",errorstatus);
  if(errorstatus == SD_OK)
  {
    /*----------------- Select Card --------------------------------*/
    errorstatus = SD_selectDeselect((u32)(sdCardInfo.RCA << 16));
  }
  printf("select %u\n",errorstatus);
  if(errorstatus == SD_OK)
  {
    errorstatus = SD_enableWideBusOperation(SDIO_BusWide_4b);
  }
  printf("widedBus %u\n",errorstatus);
  return errorstatus;
}

/********************************************************************************************************
Function Name: 
Author       : xiaobo
Date         : 2015-03-10
Description  : 
Inputs       : None
Outputs      : None
Notes        : 
Revision     : 
********************************************************************************************************/
SD_Error SD_powerOn(void)
{
  __IO SD_Error errorstatus = SD_OK;
  u32 response = 0, count = 0, validvotage = 0;
  u32 SDType = SD_STD_CAPACITY;
  
  /*!< Power ON Sequence -----------------------------------------------------*/
  /*!< Configure the SDIO peripheral */
  /*!< SDIO_CK = SDIOCLK / (SDIO_INIT_CLK_DIV + 2) */
  /*!< on STM32F4xx devices, SDIOCLK is fixed to 48MHz */
  /*!< SDIO_CK for initialization should not exceed 400 KHz */  
  sdioInitStructure.SDIO_ClockDiv = SDIO_INIT_CLK_DIV;
  sdioInitStructure.SDIO_ClockEdge = SDIO_ClockEdge_Rising;
  sdioInitStructure.SDIO_ClockBypass = SDIO_ClockBypass_Disable;
  sdioInitStructure.SDIO_ClockPowerSave = SDIO_ClockPowerSave_Disable;
  sdioInitStructure.SDIO_BusWide = SDIO_BusWide_1b;
  sdioInitStructure.SDIO_HardwareFlowControl = SDIO_HardwareFlowControl_Disable;
  SDIO_Init(&sdioInitStructure);
  
  SDIO_SetPowerState(SDIO_PowerState_ON);
  SDIO_ClockCmd(ENABLE);
  
  /*!< CMD0: GO_IDLE_STATE ---------------------------------------------------*/
  /*!< No CMD response required */
  sdioCmdInitStructure.SDIO_Argument = 0x0;
  sdioCmdInitStructure.SDIO_CmdIndex = SD_CMD_GO_IDLE_STATE;
  sdioCmdInitStructure.SDIO_Response = SDIO_Response_No;
  sdioCmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  sdioCmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  
//  for(i=0; i < 4; i++)
//  {
//    SDIO_SendCommand(&sdioCmdInitStructure);
//    errorstatus = cmdError();
//    printf("1 %u\n",errorstatus);
//  }
  
  SDIO_SendCommand(&sdioCmdInitStructure);
  errorstatus = cmdError();
  printf("powerOn1 %u\n",errorstatus);
  if(errorstatus != SD_OK)
  {
    return (errorstatus);
  }
  
  /*!< CMD8: SEND_IF_COND ----------------------------------------------------*/
  /*!< Send CMD8 to verify SD card interface operating condition */
  /*!< Argument: - [31:12]: Reserved (shall be set to '0')
               - [11:8]: Supply Voltage (VHS) 0x1 (Range: 2.7-3.6 V)
               - [7:0]: Check Pattern (recommended 0xAA) */
  /*!< CMD Response: R7 */
  sdioCmdInitStructure.SDIO_Argument = SD_CHECK_PATTERN;
  sdioCmdInitStructure.SDIO_CmdIndex = SDIO_SEND_IF_COND;
  sdioCmdInitStructure.SDIO_Response = SDIO_Response_Short;
  sdioCmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  sdioCmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&sdioCmdInitStructure);
  
  errorstatus = cmdResp7Error();
  printf("powerOn2 %u\n",errorstatus);
  if(errorstatus == SD_OK)
  {
    cardType = SDIO_STD_CAPACITY_SD_CARD_V2_0;
    SDType = SD_HIGH_CAPACITY;
  }
  else
  {
    /*!< CMD55 */
    sdioCmdInitStructure.SDIO_Argument = 0x00;
    sdioCmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_CMD;
    sdioCmdInitStructure.SDIO_Response = SDIO_Response_Short;
    sdioCmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    sdioCmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&sdioCmdInitStructure);
    errorstatus = CmdResp1Error(SD_CMD_APP_CMD);
  }
  /*!< CMD55 */
  sdioCmdInitStructure.SDIO_Argument = 0x00;
  sdioCmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_CMD;
  sdioCmdInitStructure.SDIO_Response = SDIO_Response_Short;
  sdioCmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  sdioCmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&sdioCmdInitStructure);
  errorstatus = CmdResp1Error(SD_CMD_APP_CMD);
  
  /*!< If errorstatus is Command TimeOut, it is a MMC card */
  /*!< If errorstatus is SD_OK it is a SD card: SD card 2.0 (voltage range mismatch)
     or SD card 1.x */
  if(errorstatus == SD_OK)
  {
    /*!< SD CARD */
    /*!< Send ACMD41 SD_APP_OP_COND with Argument 0x80100000 */
    while( (!validvotage) && (count < SD_MAX_VOLT_TRIAL))
    {
      /*!< SEND CMD55 APP_CMD with RCA as 0 */
      sdioCmdInitStructure.SDIO_Argument = 0x00;
      sdioCmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_CMD;
      sdioCmdInitStructure.SDIO_Response = SDIO_Response_Short;
      sdioCmdInitStructure.SDIO_Wait = SDIO_Wait_No;
      sdioCmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
      SDIO_SendCommand(&sdioCmdInitStructure);
      
      errorstatus = CmdResp1Error(SD_CMD_APP_CMD);
      if(errorstatus != SD_OK)
      {
        return errorstatus;
      }
      
      sdioCmdInitStructure.SDIO_Argument = SD_VOLTAGE_WINDOW_SD | SDType;
      sdioCmdInitStructure.SDIO_CmdIndex = SD_CMD_SD_APP_OP_COND;
      sdioCmdInitStructure.SDIO_Response = SDIO_Response_Short;
      sdioCmdInitStructure.SDIO_Wait = SDIO_Wait_No;
      sdioCmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
      SDIO_SendCommand(&sdioCmdInitStructure);
      
      errorstatus = cmdResp3Error();
      if(errorstatus != SD_OK)
      {
        return (errorstatus);
      }
      
      response = SDIO_GetResponse(SDIO_RESP1);
      validvotage = (((response >> 31) == 1)?1:0);
      count++;
    }
    if(count >= SD_MAX_VOLT_TRIAL)
    {
      errorstatus = SD_INVALID_VOLTRANGE;
      return (errorstatus);
    }
    if(response &= SD_HIGH_CAPACITY)
    {
      cardType = SDIO_HIGH_CAPACITY_SD_CARD;
    }
  }
  return (errorstatus);
}

/********************************************************************************************************
Function Name: 
Author       : xiaobo
Date         : 2015-03-10
Description  : 
Inputs       : None
Outputs      : None
Notes        : 
Revision     : 
********************************************************************************************************/
SD_Error SD_powerOff(void)
{
  SD_Error errorstatus = SD_OK;
  SDIO_SetPowerState(SDIO_PowerState_OFF);
  return errorstatus;
}

/********************************************************************************************************
Function Name: 
Author       : xiaobo
Date         : 2015-03-10
Description  : 
Inputs       : None
Outputs      : None
Notes        : 
Revision     : 
********************************************************************************************************/
SD_Error SD_InitializeCards(void)
{
  SD_Error errorstatus = SD_OK;
  u16 rca = 0x01;
  
  if(SDIO_GetPowerState() == SDIO_PowerState_OFF)
  {
    errorstatus = SD_REQUEST_NOT_APPLICABLE;
    return errorstatus;
  }
  
  if(SDIO_SECURE_DIGITAL_IO_CARD != cardType)
  {
    /*!< Send CMD2 ALL_SEND_CID */
    sdioCmdInitStructure.SDIO_Argument = 0x00;
    sdioCmdInitStructure.SDIO_CmdIndex = SD_CMD_ALL_SEND_CID;
    sdioCmdInitStructure.SDIO_Response = SDIO_Response_Long;
    sdioCmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    sdioCmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&sdioCmdInitStructure);
      
    errorstatus = cmdResp2Error();
    if(errorstatus != SD_OK)
    {
      return errorstatus;
    }
    
    CID_Tab[0] = SDIO_GetResponse(SDIO_RESP1);
    CID_Tab[1] = SDIO_GetResponse(SDIO_RESP2);
    CID_Tab[2] = SDIO_GetResponse(SDIO_RESP3);
    CID_Tab[3] = SDIO_GetResponse(SDIO_RESP4);
  }
  if( (SDIO_STD_CAPACITY_SD_CARD_V1_1 == cardType) || (SDIO_STD_CAPACITY_SD_CARD_V2_0 == cardType) || (SDIO_SECURE_DIGITAL_IO_COMBO_CARD == cardType)
    || (SDIO_HIGH_CAPACITY_SD_CARD == cardType))
  {
    /*!< Send CMD3 SET_REL_ADDR with argument 0 */
    /*!< SD Card publishes its RCA. */
    sdioCmdInitStructure.SDIO_Argument = 0x00;
    sdioCmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_REL_ADDR;
    sdioCmdInitStructure.SDIO_Response = SDIO_Response_Short;
    sdioCmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    sdioCmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&sdioCmdInitStructure);
    
    errorstatus = cmdResp6Error(SD_CMD_SET_REL_ADDR, &rca);
    
    if(SD_OK != errorstatus)
    {
      return errorstatus;
    }
  }
    
  if(SDIO_SECURE_DIGITAL_IO_CARD != cardType)
  {
    RCA = rca;
    /*!< Send CMD9 SEND_CSD with argument as card's RCA */
    sdioCmdInitStructure.SDIO_Argument = (u32)(rca <<16);
    sdioCmdInitStructure.SDIO_CmdIndex = SD_CMD_SEND_CSD;
    sdioCmdInitStructure.SDIO_Response = SDIO_Response_Long;
    sdioCmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    sdioCmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&sdioCmdInitStructure);
    
    errorstatus = cmdResp2Error();
    if(SD_OK != errorstatus)
    {
      return errorstatus;
    }
    
    CSD_Tab[0] = SDIO_GetResponse(SDIO_RESP1);
    CSD_Tab[1] = SDIO_GetResponse(SDIO_RESP2);
    CSD_Tab[2] = SDIO_GetResponse(SDIO_RESP3);
    CSD_Tab[3] = SDIO_GetResponse(SDIO_RESP4);
  }
  
  errorstatus = SD_OK;
  return errorstatus;
}

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
SD_Error SD_getCardInfo(SD_CardInfo *cardinfo)
{
  SD_Error errorstatus = SD_OK;
  u8 tmp = 0;
  
  cardinfo->CardType = (u8)cardType;
  cardinfo->RCA = (u16)RCA;
  
  /*!< Byte 0 */
  tmp = (uint8_t)((CSD_Tab[0] & 0xFF000000) >> 24);
  cardinfo->SD_csd.CSDStruct = (tmp & 0xC0) >> 6;
  cardinfo->SD_csd.SysSpecVersion = (tmp & 0x3C) >> 2;
  cardinfo->SD_csd.Reserved1 = tmp & 0x03;

  /*!< Byte 1 */
  tmp = (uint8_t)((CSD_Tab[0] & 0x00FF0000) >> 16);
  cardinfo->SD_csd.TAAC = tmp;

  /*!< Byte 2 */
  tmp = (uint8_t)((CSD_Tab[0] & 0x0000FF00) >> 8);
  cardinfo->SD_csd.NSAC = tmp;

  /*!< Byte 3 */
  tmp = (uint8_t)(CSD_Tab[0] & 0x000000FF);
  cardinfo->SD_csd.MaxBusClkFrec = tmp;

  /*!< Byte 4 */
  tmp = (uint8_t)((CSD_Tab[1] & 0xFF000000) >> 24);
  cardinfo->SD_csd.CardComdClasses = tmp << 4;

  /*!< Byte 5 */
  tmp = (uint8_t)((CSD_Tab[1] & 0x00FF0000) >> 16);
  cardinfo->SD_csd.CardComdClasses |= (tmp & 0xF0) >> 4;
  cardinfo->SD_csd.RdBlockLen = tmp & 0x0F;

  /*!< Byte 6 */
  tmp = (uint8_t)((CSD_Tab[1] & 0x0000FF00) >> 8);
  cardinfo->SD_csd.PartBlockRead = (tmp & 0x80) >> 7;
  cardinfo->SD_csd.WrBlockMisalign = (tmp & 0x40) >> 6;
  cardinfo->SD_csd.RdBlockMisalign = (tmp & 0x20) >> 5;
  cardinfo->SD_csd.DSRImpl = (tmp & 0x10) >> 4;
  cardinfo->SD_csd.Reserved2 = 0; /*!< Reserved */
  
  if((cardType == SDIO_STD_CAPACITY_SD_CARD_V1_1) || (cardType == SDIO_STD_CAPACITY_SD_CARD_V2_0))
  {
    cardinfo->SD_csd.DeviceSize = (tmp & 0x03) << 10;

    /*!< Byte 7 */
    tmp = (uint8_t)(CSD_Tab[1] & 0x000000FF);
    cardinfo->SD_csd.DeviceSize |= (tmp) << 2;

    /*!< Byte 8 */
    tmp = (uint8_t)((CSD_Tab[2] & 0xFF000000) >> 24);
    cardinfo->SD_csd.DeviceSize |= (tmp & 0xC0) >> 6;

    cardinfo->SD_csd.MaxRdCurrentVDDMin = (tmp & 0x38) >> 3;
    cardinfo->SD_csd.MaxRdCurrentVDDMax = (tmp & 0x07);

    /*!< Byte 9 */
    tmp = (uint8_t)((CSD_Tab[2] & 0x00FF0000) >> 16);
    cardinfo->SD_csd.MaxWrCurrentVDDMin = (tmp & 0xE0) >> 5;
    cardinfo->SD_csd.MaxWrCurrentVDDMax = (tmp & 0x1C) >> 2;
    cardinfo->SD_csd.DeviceSizeMul = (tmp & 0x03) << 1;
    /*!< Byte 10 */
    tmp = (uint8_t)((CSD_Tab[2] & 0x0000FF00) >> 8);
    cardinfo->SD_csd.DeviceSizeMul |= (tmp & 0x80) >> 7;
    
    cardinfo->CardCapacity = (cardinfo->SD_csd.DeviceSize + 1) ;
    cardinfo->CardCapacity *= (1 << (cardinfo->SD_csd.DeviceSizeMul + 2));
    cardinfo->CardBlockSize = 1 << (cardinfo->SD_csd.RdBlockLen);
    cardinfo->CardCapacity *= cardinfo->CardBlockSize;
  }
  else if(cardType == SDIO_HIGH_CAPACITY_SD_CARD)
  {
    /*!< Byte 7 */
    tmp = (uint8_t)(CSD_Tab[1] & 0x000000FF);
    cardinfo->SD_csd.DeviceSize = (tmp & 0x3F) << 16;

    /*!< Byte 8 */
    tmp = (uint8_t)((CSD_Tab[2] & 0xFF000000) >> 24);

    cardinfo->SD_csd.DeviceSize |= (tmp << 8);

    /*!< Byte 9 */
    tmp = (uint8_t)((CSD_Tab[2] & 0x00FF0000) >> 16);

    cardinfo->SD_csd.DeviceSize |= (tmp);

    /*!< Byte 10 */
    tmp = (uint8_t)((CSD_Tab[2] & 0x0000FF00) >> 8);
    
    cardinfo->CardCapacity = ((uint64_t)cardinfo->SD_csd.DeviceSize + 1) * 512 * 1024;
    cardinfo->CardBlockSize = 512;   
  }
  
  cardinfo->SD_csd.EraseGrSize = (tmp & 0x40) >> 6;
  cardinfo->SD_csd.EraseGrMul = (tmp & 0x3F) << 1;

  /*!< Byte 11 */
  tmp = (uint8_t)(CSD_Tab[2] & 0x000000FF);
  cardinfo->SD_csd.EraseGrMul |= (tmp & 0x80) >> 7;
  cardinfo->SD_csd.WrProtectGrSize = (tmp & 0x7F);

  /*!< Byte 12 */
  tmp = (uint8_t)((CSD_Tab[3] & 0xFF000000) >> 24);
  cardinfo->SD_csd.WrProtectGrEnable = (tmp & 0x80) >> 7;
  cardinfo->SD_csd.ManDeflECC = (tmp & 0x60) >> 5;
  cardinfo->SD_csd.WrSpeedFact = (tmp & 0x1C) >> 2;
  cardinfo->SD_csd.MaxWrBlockLen = (tmp & 0x03) << 2;

  /*!< Byte 13 */
  tmp = (uint8_t)((CSD_Tab[3] & 0x00FF0000) >> 16);
  cardinfo->SD_csd.MaxWrBlockLen |= (tmp & 0xC0) >> 6;
  cardinfo->SD_csd.WriteBlockPaPartial = (tmp & 0x20) >> 5;
  cardinfo->SD_csd.Reserved3 = 0;
  cardinfo->SD_csd.ContentProtectAppli = (tmp & 0x01);

  /*!< Byte 14 */
  tmp = (uint8_t)((CSD_Tab[3] & 0x0000FF00) >> 8);
  cardinfo->SD_csd.FileFormatGrouop = (tmp & 0x80) >> 7;
  cardinfo->SD_csd.CopyFlag = (tmp & 0x40) >> 6;
  cardinfo->SD_csd.PermWrProtect = (tmp & 0x20) >> 5;
  cardinfo->SD_csd.TempWrProtect = (tmp & 0x10) >> 4;
  cardinfo->SD_csd.FileFormat = (tmp & 0x0C) >> 2;
  cardinfo->SD_csd.ECC = (tmp & 0x03);

  /*!< Byte 15 */
  tmp = (uint8_t)(CSD_Tab[3] & 0x000000FF);
  cardinfo->SD_csd.CSD_CRC = (tmp & 0xFE) >> 1;
  cardinfo->SD_csd.Reserved4 = 1;


  /*!< Byte 0 */
  tmp = (uint8_t)((CID_Tab[0] & 0xFF000000) >> 24);
  cardinfo->SD_cid.ManufacturerID = tmp;

  /*!< Byte 1 */
  tmp = (uint8_t)((CID_Tab[0] & 0x00FF0000) >> 16);
  cardinfo->SD_cid.OEM_AppliID = tmp << 8;

  /*!< Byte 2 */
  tmp = (uint8_t)((CID_Tab[0] & 0x000000FF00) >> 8);
  cardinfo->SD_cid.OEM_AppliID |= tmp;

  /*!< Byte 3 */
  tmp = (uint8_t)(CID_Tab[0] & 0x000000FF);
  cardinfo->SD_cid.ProdName1 = tmp << 24;

  /*!< Byte 4 */
  tmp = (uint8_t)((CID_Tab[1] & 0xFF000000) >> 24);
  cardinfo->SD_cid.ProdName1 |= tmp << 16;

  /*!< Byte 5 */
  tmp = (uint8_t)((CID_Tab[1] & 0x00FF0000) >> 16);
  cardinfo->SD_cid.ProdName1 |= tmp << 8;

  /*!< Byte 6 */
  tmp = (uint8_t)((CID_Tab[1] & 0x0000FF00) >> 8);
  cardinfo->SD_cid.ProdName1 |= tmp;

  /*!< Byte 7 */
  tmp = (uint8_t)(CID_Tab[1] & 0x000000FF);
  cardinfo->SD_cid.ProdName2 = tmp;

  /*!< Byte 8 */
  tmp = (uint8_t)((CID_Tab[2] & 0xFF000000) >> 24);
  cardinfo->SD_cid.ProdRev = tmp;

  /*!< Byte 9 */
  tmp = (uint8_t)((CID_Tab[2] & 0x00FF0000) >> 16);
  cardinfo->SD_cid.ProdSN = tmp << 24;

  /*!< Byte 10 */
  tmp = (uint8_t)((CID_Tab[2] & 0x0000FF00) >> 8);
  cardinfo->SD_cid.ProdSN |= tmp << 16;

  /*!< Byte 11 */
  tmp = (uint8_t)(CID_Tab[2] & 0x000000FF);
  cardinfo->SD_cid.ProdSN |= tmp << 8;

  /*!< Byte 12 */
  tmp = (uint8_t)((CID_Tab[3] & 0xFF000000) >> 24);
  cardinfo->SD_cid.ProdSN |= tmp;

  /*!< Byte 13 */
  tmp = (uint8_t)((CID_Tab[3] & 0x00FF0000) >> 16);
  cardinfo->SD_cid.Reserved1 |= (tmp & 0xF0) >> 4;
  cardinfo->SD_cid.ManufactDate = (tmp & 0x0F) << 8;

  /*!< Byte 14 */
  tmp = (uint8_t)((CID_Tab[3] & 0x0000FF00) >> 8);
  cardinfo->SD_cid.ManufactDate |= tmp;

  /*!< Byte 15 */
  tmp = (uint8_t)(CID_Tab[3] & 0x000000FF);
  cardinfo->SD_cid.CID_CRC = (tmp & 0xFE) >> 1;
  cardinfo->SD_cid.Reserved2 = 1;
  
  return(errorstatus);
}

SD_Error SD_selectDeselect(uint64_t addr)
{
  SD_Error errorstatus = SD_OK;
  
  /*!< Send CMD7 SDIO_SEL_DESEL_CARD */
  sdioCmdInitStructure.SDIO_Argument = (u32)addr;
  sdioCmdInitStructure.SDIO_CmdIndex = SD_CMD_SEL_DESEL_CARD;
  sdioCmdInitStructure.SDIO_Response = SDIO_Response_Short;
  sdioCmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  sdioCmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&sdioCmdInitStructure);
  
  errorstatus = CmdResp1Error(SD_CMD_SEL_DESEL_CARD);
  return errorstatus;
}

SD_Error SD_enableWideBusOperation(u32 wideMode)
{
  SD_Error errorstatus = SD_OK;
  
  /*!< MMC Card doesn't support this feature */
  if(SDIO_MULTIMEDIA_CARD == cardType)
  {
    errorstatus = SD_UNSUPPORTED_FEATURE;
    return errorstatus;
  }
  else if((SDIO_STD_CAPACITY_SD_CARD_V1_1 == cardType) || (SDIO_STD_CAPACITY_SD_CARD_V2_0 == cardType) || (SDIO_HIGH_CAPACITY_SD_CARD == cardType))
  {
    if(SDIO_BusWide_8b == wideMode)
    {
      errorstatus = SD_UNSUPPORTED_FEATURE;
      return errorstatus;
    }else if(SDIO_BusWide_4b == wideMode)
    {
      errorstatus = sdEnWideBus(ENABLE);
      
      if(SD_OK == errorstatus)
      {
        /*!< Configure the SDIO peripheral */
        sdioInitStructure.SDIO_ClockDiv = SDIO_TRANSFER_CLK_DIV; 
        sdioInitStructure.SDIO_ClockEdge = SDIO_ClockEdge_Rising;
        sdioInitStructure.SDIO_ClockBypass = SDIO_ClockBypass_Disable;
        sdioInitStructure.SDIO_ClockPowerSave = SDIO_ClockPowerSave_Disable;
        sdioInitStructure.SDIO_BusWide = SDIO_BusWide_4b;
        sdioInitStructure.SDIO_HardwareFlowControl = SDIO_HardwareFlowControl_Disable;
        SDIO_Init(&sdioInitStructure);
      }
    }
    else
    {
      errorstatus = sdEnWideBus(DISABLE);

      if (SD_OK == errorstatus)
      {
        /*!< Configure the SDIO peripheral */
        sdioInitStructure.SDIO_ClockDiv = SDIO_TRANSFER_CLK_DIV; 
        sdioInitStructure.SDIO_ClockEdge = SDIO_ClockEdge_Rising;
        sdioInitStructure.SDIO_ClockBypass = SDIO_ClockBypass_Disable;
        sdioInitStructure.SDIO_ClockPowerSave = SDIO_ClockPowerSave_Disable;
        sdioInitStructure.SDIO_BusWide = SDIO_BusWide_1b;
        sdioInitStructure.SDIO_HardwareFlowControl = SDIO_HardwareFlowControl_Disable;
        SDIO_Init(&sdioInitStructure);
      }
    }
  }
  return errorstatus;
}

static SD_Error sdEnWideBus(FunctionalState newState)
{
  SD_Error errorstatus = SD_OK;

  uint32_t scr[2] = {0, 0};

  if (SDIO_GetResponse(SDIO_RESP1) & SD_CARD_LOCKED)
  {
    errorstatus = SD_LOCK_UNLOCK_FAILED;
    return(errorstatus);
  }
  
   /*!< Get SCR Register */
   errorstatus = FindSCR(RCA,scr);
  
  if(errorstatus != SD_OK)
  {
    return errorstatus;
  }
  
  /*!< If wide bus operation to be enabled */
  if (newState == ENABLE)
  {
    /*!< If requested card supports wide bus operation */
    if ((scr[1] & SD_WIDE_BUS_SUPPORT) != SD_ALLZERO)
    {
      /*!< Send CMD55 APP_CMD with argument as card's RCA.*/
      sdioCmdInitStructure.SDIO_Argument = (uint32_t) RCA << 16;
      sdioCmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_CMD;
      sdioCmdInitStructure.SDIO_Response = SDIO_Response_Short;
      sdioCmdInitStructure.SDIO_Wait = SDIO_Wait_No;
      sdioCmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
      SDIO_SendCommand(&sdioCmdInitStructure);

      errorstatus = CmdResp1Error(SD_CMD_APP_CMD);

      if (errorstatus != SD_OK)
      {
        return(errorstatus);
      }

      /*!< Send ACMD6 APP_CMD with argument as 2 for wide bus mode */
      sdioCmdInitStructure.SDIO_Argument = 0x2;
      sdioCmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_SD_SET_BUSWIDTH;
      sdioCmdInitStructure.SDIO_Response = SDIO_Response_Short;
      sdioCmdInitStructure.SDIO_Wait = SDIO_Wait_No;
      sdioCmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
      SDIO_SendCommand(&sdioCmdInitStructure);

      errorstatus = CmdResp1Error(SD_CMD_APP_SD_SET_BUSWIDTH);

      if (errorstatus != SD_OK)
      {
        return(errorstatus);
      }
      return(errorstatus);
    }
    else
    {
      errorstatus = SD_REQUEST_NOT_APPLICABLE;
      return(errorstatus);
    }
  }   /*!< If wide bus operation to be disabled */
  else
  {
    /*!< If requested card supports 1 bit mode operation */
    if ((scr[1] & SD_SINGLE_BUS_SUPPORT) != SD_ALLZERO)
    {
      /*!< Send CMD55 APP_CMD with argument as card's RCA.*/
      sdioCmdInitStructure.SDIO_Argument = (uint32_t) RCA << 16;
      sdioCmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_CMD;
      sdioCmdInitStructure.SDIO_Response = SDIO_Response_Short;
      sdioCmdInitStructure.SDIO_Wait = SDIO_Wait_No;
      sdioCmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
      SDIO_SendCommand(&sdioCmdInitStructure);


      errorstatus = CmdResp1Error(SD_CMD_APP_CMD);

      if (errorstatus != SD_OK)
      {
        return(errorstatus);
      }

      /*!< Send ACMD6 APP_CMD with argument as 2 for wide bus mode */
      sdioCmdInitStructure.SDIO_Argument = 0x00;
      sdioCmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_SD_SET_BUSWIDTH;
      sdioCmdInitStructure.SDIO_Response = SDIO_Response_Short;
      sdioCmdInitStructure.SDIO_Wait = SDIO_Wait_No;
      sdioCmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
      SDIO_SendCommand(&sdioCmdInitStructure);

      errorstatus = CmdResp1Error(SD_CMD_APP_SD_SET_BUSWIDTH);

      if (errorstatus != SD_OK)
      {
        return(errorstatus);
      }

      return(errorstatus);
    }
    else
    {
      errorstatus = SD_REQUEST_NOT_APPLICABLE;
      return(errorstatus);
    }
  }
}

/**
  * @brief  Find the SD card SCR register value.
  * @param  rca: selected card address.
  * @param  pscr: pointer to the buffer that will contain the SCR value.
  * @retval SD_Error: SD Card Error code.
  */
static SD_Error FindSCR(uint16_t rca, uint32_t *pscr)
{
  uint32_t index = 0;
  SD_Error errorstatus = SD_OK;
  uint32_t tempscr[2] = {0, 0};

  /*!< Set Block Size To 8 Bytes */
  /*!< Send CMD55 APP_CMD with argument as card's RCA */
  sdioCmdInitStructure.SDIO_Argument = (uint32_t)8;
  sdioCmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_BLOCKLEN;
  sdioCmdInitStructure.SDIO_Response = SDIO_Response_Short;
  sdioCmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  sdioCmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&sdioCmdInitStructure);

  errorstatus = CmdResp1Error(SD_CMD_SET_BLOCKLEN);

  if (errorstatus != SD_OK)
  {
    return(errorstatus);
  }

  /*!< Send CMD55 APP_CMD with argument as card's RCA */
  sdioCmdInitStructure.SDIO_Argument = (uint32_t) RCA << 16;
  sdioCmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_CMD;
  sdioCmdInitStructure.SDIO_Response = SDIO_Response_Short;
  sdioCmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  sdioCmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&sdioCmdInitStructure);

  errorstatus = CmdResp1Error(SD_CMD_APP_CMD);

  if (errorstatus != SD_OK)
  {
    return(errorstatus);
  }
  sdioDataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT;
  sdioDataInitStructure.SDIO_DataLength = 8;
  sdioDataInitStructure.SDIO_DataBlockSize = SDIO_DataBlockSize_8b;
  sdioDataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToSDIO;
  sdioDataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
  sdioDataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
  SDIO_DataConfig(&sdioDataInitStructure);


  /*!< Send ACMD51 SD_APP_SEND_SCR with argument as 0 */
  sdioCmdInitStructure.SDIO_Argument = 0x0;
  sdioCmdInitStructure.SDIO_CmdIndex = SD_CMD_SD_APP_SEND_SCR;
  sdioCmdInitStructure.SDIO_Response = SDIO_Response_Short;
  sdioCmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  sdioCmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&sdioCmdInitStructure);

  errorstatus = CmdResp1Error(SD_CMD_SD_APP_SEND_SCR);

  if (errorstatus != SD_OK)
  {
    return(errorstatus);
  }

  while (!(SDIO->STA & (SDIO_FLAG_RXOVERR | SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_DBCKEND | SDIO_FLAG_STBITERR)))
  {
    if (SDIO_GetFlagStatus(SDIO_FLAG_RXDAVL) != RESET)
    {
      *(tempscr + index) = SDIO_ReadData();
      index++;
    }
  }

  if (SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT) != RESET)
  {
    SDIO_ClearFlag(SDIO_FLAG_DTIMEOUT);
    errorstatus = SD_DATA_TIMEOUT;
    return(errorstatus);
  }
  else if (SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL) != RESET)
  {
    SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL);
    errorstatus = SD_DATA_CRC_FAIL;
    return(errorstatus);
  }
  else if (SDIO_GetFlagStatus(SDIO_FLAG_RXOVERR) != RESET)
  {
    SDIO_ClearFlag(SDIO_FLAG_RXOVERR);
    errorstatus = SD_RX_OVERRUN;
    return(errorstatus);
  }
  else if (SDIO_GetFlagStatus(SDIO_FLAG_STBITERR) != RESET)
  {
    SDIO_ClearFlag(SDIO_FLAG_STBITERR);
    errorstatus = SD_START_BIT_ERR;
    return(errorstatus);
  }

  /*!< Clear all the static flags */
  SDIO_ClearFlag(SDIO_STATIC_FLAGS);

  *(pscr + 1) = ((tempscr[0] & SD_0TO7BITS) << 24) | ((tempscr[0] & SD_8TO15BITS) << 8) | ((tempscr[0] & SD_16TO23BITS) >> 8) | ((tempscr[0] & SD_24TO31BITS) >> 24);

  *(pscr) = ((tempscr[1] & SD_0TO7BITS) << 24) | ((tempscr[1] & SD_8TO15BITS) << 8) | ((tempscr[1] & SD_16TO23BITS) >> 8) | ((tempscr[1] & SD_24TO31BITS) >> 24);

  return(errorstatus);
}

static SD_Error cmdError(void)
{
  SD_Error errorstatus = SD_OK;
  u32 timeout;
  timeout = SDIO_CMD0TIMEOUT;
  while( (timeout > 0) && (SDIO_GetFlagStatus(SDIO_FLAG_CMDSENT) == RESET))
  {
    timeout--;
  }
  if(timeout == 0)
  {
    errorstatus = SD_CMD_RSP_TIMEOUT;
    return (errorstatus);
  }
  SDIO_ClearFlag(SDIO_STATIC_FLAGS);
  return (errorstatus);
}

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
static SD_Error cmdResp7Error(void)
{
  SD_Error errorstatus = SD_OK;
  u32 status;
  u32 timeout = SDIO_CMD0TIMEOUT;
  status = SDIO->STA;
  
  while(!(status & (SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CMDREND | SDIO_FLAG_CTIMEOUT)) && (timeout > 0))
  {
    timeout--;
    status = SDIO->STA;
  }
  if( (timeout == 0) || (status & SDIO_FLAG_CTIMEOUT) )
  {
    errorstatus = SD_CMD_RSP_TIMEOUT;
    SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);
    return (errorstatus);
  }
  
  if(status & SDIO_FLAG_CMDREND)
  {
    errorstatus = SD_OK;
    SDIO_ClearFlag(SDIO_FLAG_CMDREND);
    return errorstatus;
  }
  return errorstatus;
}

/**
  * @brief  Checks for error conditions for R1 response.
  * @param  cmd: The sent command index.
  * @retval SD_Error: SD Card Error code.
  */
static SD_Error CmdResp1Error(uint8_t cmd)
{
  SD_Error errorstatus = SD_OK;
  uint32_t status;
  uint32_t response_r1;

  status = SDIO->STA;

  while (!(status & (SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CMDREND | SDIO_FLAG_CTIMEOUT)))
  {
    status = SDIO->STA;
  }

  if (status & SDIO_FLAG_CTIMEOUT)
  {
    errorstatus = SD_CMD_RSP_TIMEOUT;
    SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);
    return(errorstatus);
  }
  else if (status & SDIO_FLAG_CCRCFAIL)
  {
    errorstatus = SD_CMD_CRC_FAIL;
    SDIO_ClearFlag(SDIO_FLAG_CCRCFAIL);
    return(errorstatus);
  }

  /*!< Check response received is of desired command */
  if (SDIO_GetCommandResponse() != cmd)
  {
    errorstatus = SD_ILLEGAL_CMD;
    return(errorstatus);
  }

  /*!< Clear all the static flags */
  SDIO_ClearFlag(SDIO_STATIC_FLAGS);

  /*!< We have received response, retrieve it for analysis  */
  response_r1 = SDIO_GetResponse(SDIO_RESP1);

  if ((response_r1 & SD_OCR_ERRORBITS) == SD_ALLZERO)
  {
    return(errorstatus);
  }

  if (response_r1 & SD_OCR_ADDR_OUT_OF_RANGE)
  {
    return(SD_ADDR_OUT_OF_RANGE);
  }

  if (response_r1 & SD_OCR_ADDR_MISALIGNED)
  {
    return(SD_ADDR_MISALIGNED);
  }

  if (response_r1 & SD_OCR_BLOCK_LEN_ERR)
  {
    return(SD_BLOCK_LEN_ERR);
  }

  if (response_r1 & SD_OCR_ERASE_SEQ_ERR)
  {
    return(SD_ERASE_SEQ_ERR);
  }

  if (response_r1 & SD_OCR_BAD_ERASE_PARAM)
  {
    return(SD_BAD_ERASE_PARAM);
  }

  if (response_r1 & SD_OCR_WRITE_PROT_VIOLATION)
  {
    return(SD_WRITE_PROT_VIOLATION);
  }

  if (response_r1 & SD_OCR_LOCK_UNLOCK_FAILED)
  {
    return(SD_LOCK_UNLOCK_FAILED);
  }

  if (response_r1 & SD_OCR_COM_CRC_FAILED)
  {
    return(SD_COM_CRC_FAILED);
  }

  if (response_r1 & SD_OCR_ILLEGAL_CMD)
  {
    return(SD_ILLEGAL_CMD);
  }

  if (response_r1 & SD_OCR_CARD_ECC_FAILED)
  {
    return(SD_CARD_ECC_FAILED);
  }

  if (response_r1 & SD_OCR_CC_ERROR)
  {
    return(SD_CC_ERROR);
  }

  if (response_r1 & SD_OCR_GENERAL_UNKNOWN_ERROR)
  {
    return(SD_GENERAL_UNKNOWN_ERROR);
  }

  if (response_r1 & SD_OCR_STREAM_READ_UNDERRUN)
  {
    return(SD_STREAM_READ_UNDERRUN);
  }

  if (response_r1 & SD_OCR_STREAM_WRITE_OVERRUN)
  {
    return(SD_STREAM_WRITE_OVERRUN);
  }

  if (response_r1 & SD_OCR_CID_CSD_OVERWRIETE)
  {
    return(SD_CID_CSD_OVERWRITE);
  }

  if (response_r1 & SD_OCR_WP_ERASE_SKIP)
  {
    return(SD_WP_ERASE_SKIP);
  }

  if (response_r1 & SD_OCR_CARD_ECC_DISABLED)
  {
    return(SD_CARD_ECC_DISABLED);
  }

  if (response_r1 & SD_OCR_ERASE_RESET)
  {
    return(SD_ERASE_RESET);
  }

  if (response_r1 & SD_OCR_AKE_SEQ_ERROR)
  {
    return(SD_AKE_SEQ_ERROR);
  }
  return(errorstatus);
}

/**
  * @brief  Checks for error conditions for R1 response.
  * @param  cmd: The sent command index.
  * @retval SD_Error: SD Card Error code.
  */
static SD_Error cmdResp3Error(void)
{
  SD_Error errorstatus = SD_OK;
  u32 status;
  
  status = SDIO->STA;
  
  while(!(status & (SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CMDREND | SDIO_FLAG_CTIMEOUT)))
  {
    status = SDIO->STA;
  }
  
  if(status & SDIO_FLAG_CTIMEOUT)
  {
    errorstatus = SD_CMD_RSP_TIMEOUT;
    SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);
    return (errorstatus);
  }
  SDIO_ClearFlag(SDIO_STATIC_FLAGS);
  return errorstatus;
}

/**
  * @brief  Checks for error conditions for R2 (CID or CSD) response.
  * @param  None
  * @retval SD_Error: SD Card Error code.
  */
static SD_Error cmdResp2Error(void)
{
  SD_Error errorstatus = SD_OK;
  u32 status;
  
  status = SDIO->STA;
  
  while(!(status & (SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CMDREND | SDIO_FLAG_CTIMEOUT)))
  {
    status = SDIO->STA;
  }
  
  if(status & SDIO_FLAG_CTIMEOUT)
  {
    errorstatus = SD_CMD_RSP_TIMEOUT;
    SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);
    return (errorstatus);
  }
  /*!< Clear all the static flags */
  SDIO_ClearFlag(SDIO_STATIC_FLAGS);
  return errorstatus;
}

/**
  * @brief  Checks for error conditions for R6 (RCA) response.
  * @param  cmd: The sent command index.
  * @param  prca: pointer to the variable that will contain the SD card relative 
  *         address RCA. 
  * @retval SD_Error: SD Card Error code.
  */
static SD_Error cmdResp6Error(u8 cmd, u16 *prca)
{
  SD_Error errorstatus = SD_OK;
  u32 status;
  u32 response_r1;
  
  status = SDIO->STA;
  
  while(!(status & (SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CMDREND | SDIO_FLAG_CTIMEOUT)))
  {
    status = SDIO->STA;
  }
  
  if(status & SDIO_FLAG_CTIMEOUT)
  {
    errorstatus = SD_CMD_RSP_TIMEOUT;
    SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);
    return (errorstatus);
  }else if(status & SDIO_FLAG_CCRCFAIL)
  {
    errorstatus = SD_CMD_CRC_FAIL;
    SDIO_ClearFlag(SDIO_FLAG_CCRCFAIL);
  }
  
  /*!< Check response received is of desired command */
  if(SDIO_GetCommandResponse() != cmd)
  {
    errorstatus = SD_ILLEGAL_CMD;
    return errorstatus;
  }
  
  /*!< Clear all the static flags */
  SDIO_ClearFlag(SDIO_STATIC_FLAGS);
  
  /*!< We have received response, retrieve it.  */
  response_r1 = SDIO_GetResponse(SDIO_RESP1);
  
  if (SD_ALLZERO == (response_r1 & (SD_R6_GENERAL_UNKNOWN_ERROR | SD_R6_ILLEGAL_CMD | SD_R6_COM_CRC_FAILED)))
  {
    *prca = (uint16_t) (response_r1 >> 16);
    return(errorstatus);
  }

  if (response_r1 & SD_R6_GENERAL_UNKNOWN_ERROR)
  {
    return(SD_GENERAL_UNKNOWN_ERROR);
  }

  if (response_r1 & SD_R6_ILLEGAL_CMD)
  {
    return(SD_ILLEGAL_CMD);
  }

  if (response_r1 & SD_R6_COM_CRC_FAILED)
  {
    return(SD_COM_CRC_FAILED);
  }
  
  return errorstatus;
}

///////////////////////////////////
//以下开始是读写擦除的函数
///////////////////////////////////

/**
  * @brief  Checks if the SD card is in programming state.
  * @param  pstatus: pointer to the variable that will contain the SD card state.
  * @retval SD_Error: SD Card Error code.
  */
static SD_Error IsCardProgramming(uint8_t *pstatus)
{
  SD_Error errorstatus = SD_OK;
  __IO uint32_t respR1 = 0, status = 0;

  sdioCmdInitStructure.SDIO_Argument = (uint32_t) RCA << 16;
  sdioCmdInitStructure.SDIO_CmdIndex = SD_CMD_SEND_STATUS;
  sdioCmdInitStructure.SDIO_Response = SDIO_Response_Short;
  sdioCmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  sdioCmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&sdioCmdInitStructure);

  status = SDIO->STA;
  while (!(status & (SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CMDREND | SDIO_FLAG_CTIMEOUT)))
  {
    status = SDIO->STA;
  }

  if (status & SDIO_FLAG_CTIMEOUT)
  {
    errorstatus = SD_CMD_RSP_TIMEOUT;
    SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);
    return(errorstatus);
  }
  else if (status & SDIO_FLAG_CCRCFAIL)
  {
    errorstatus = SD_CMD_CRC_FAIL;
    SDIO_ClearFlag(SDIO_FLAG_CCRCFAIL);
    return(errorstatus);
  }

  status = (uint32_t)SDIO_GetCommandResponse();

  /*!< Check response received is of desired command */
  if (status != SD_CMD_SEND_STATUS)
  {
    errorstatus = SD_ILLEGAL_CMD;
    return(errorstatus);
  }

  /*!< Clear all the static flags */
  SDIO_ClearFlag(SDIO_STATIC_FLAGS);


  /*!< We have received response, retrieve it for analysis  */
  respR1 = SDIO_GetResponse(SDIO_RESP1);

  /*!< Find out card status */
  *pstatus = (uint8_t) ((respR1 >> 9) & 0x0000000F);

  if ((respR1 & SD_OCR_ERRORBITS) == SD_ALLZERO)
  {
    return(errorstatus);
  }

  if (respR1 & SD_OCR_ADDR_OUT_OF_RANGE)
  {
    return(SD_ADDR_OUT_OF_RANGE);
  }

  if (respR1 & SD_OCR_ADDR_MISALIGNED)
  {
    return(SD_ADDR_MISALIGNED);
  }

  if (respR1 & SD_OCR_BLOCK_LEN_ERR)
  {
    return(SD_BLOCK_LEN_ERR);
  }

  if (respR1 & SD_OCR_ERASE_SEQ_ERR)
  {
    return(SD_ERASE_SEQ_ERR);
  }

  if (respR1 & SD_OCR_BAD_ERASE_PARAM)
  {
    return(SD_BAD_ERASE_PARAM);
  }

  if (respR1 & SD_OCR_WRITE_PROT_VIOLATION)
  {
    return(SD_WRITE_PROT_VIOLATION);
  }

  if (respR1 & SD_OCR_LOCK_UNLOCK_FAILED)
  {
    return(SD_LOCK_UNLOCK_FAILED);
  }

  if (respR1 & SD_OCR_COM_CRC_FAILED)
  {
    return(SD_COM_CRC_FAILED);
  }

  if (respR1 & SD_OCR_ILLEGAL_CMD)
  {
    return(SD_ILLEGAL_CMD);
  }

  if (respR1 & SD_OCR_CARD_ECC_FAILED)
  {
    return(SD_CARD_ECC_FAILED);
  }

  if (respR1 & SD_OCR_CC_ERROR)
  {
    return(SD_CC_ERROR);
  }

  if (respR1 & SD_OCR_GENERAL_UNKNOWN_ERROR)
  {
    return(SD_GENERAL_UNKNOWN_ERROR);
  }

  if (respR1 & SD_OCR_STREAM_READ_UNDERRUN)
  {
    return(SD_STREAM_READ_UNDERRUN);
  }

  if (respR1 & SD_OCR_STREAM_WRITE_OVERRUN)
  {
    return(SD_STREAM_WRITE_OVERRUN);
  }

  if (respR1 & SD_OCR_CID_CSD_OVERWRIETE)
  {
    return(SD_CID_CSD_OVERWRITE);
  }

  if (respR1 & SD_OCR_WP_ERASE_SKIP)
  {
    return(SD_WP_ERASE_SKIP);
  }

  if (respR1 & SD_OCR_CARD_ECC_DISABLED)
  {
    return(SD_CARD_ECC_DISABLED);
  }

  if (respR1 & SD_OCR_ERASE_RESET)
  {
    return(SD_ERASE_RESET);
  }

  if (respR1 & SD_OCR_AKE_SEQ_ERROR)
  {
    return(SD_AKE_SEQ_ERROR);
  }

  return(errorstatus);
}

/**
  * @brief  Allows to erase memory area specified for the given card.
  * @param  startaddr: the start address.
  * @param  endaddr: the end address.
  * @retval SD_Error: SD Card Error code.
  */
SD_Error SD_Erase(uint64_t startaddr, uint64_t endaddr)
{
  SD_Error errorstatus = SD_OK;
  uint32_t delay = 0;
  __IO uint32_t maxdelay = 0;
  uint8_t cardstate = 0;

  /*!< Check if the card coomnd class supports erase command */
  if (((CSD_Tab[1] >> 20) & SD_CCCC_ERASE) == 0)
  {
    errorstatus = SD_REQUEST_NOT_APPLICABLE;
    return(errorstatus);
  }

  maxdelay = 120000 / ((SDIO->CLKCR & 0xFF) + 2);

  if (SDIO_GetResponse(SDIO_RESP1) & SD_CARD_LOCKED)
  {
    errorstatus = SD_LOCK_UNLOCK_FAILED;
    return(errorstatus);
  }

  if (cardType == SDIO_HIGH_CAPACITY_SD_CARD)
  {
    startaddr /= 512;
    endaddr /= 512;
  }
  
  /*!< According to sd-card spec 1.0 ERASE_GROUP_START (CMD32) and erase_group_end(CMD33) */
  if ((SDIO_STD_CAPACITY_SD_CARD_V1_1 == cardType) || (SDIO_STD_CAPACITY_SD_CARD_V2_0 == cardType) || (SDIO_HIGH_CAPACITY_SD_CARD == cardType))
  {
    /*!< Send CMD32 SD_ERASE_GRP_START with argument as addr  */
    sdioCmdInitStructure.SDIO_Argument =(uint32_t)startaddr;
    sdioCmdInitStructure.SDIO_CmdIndex = SD_CMD_SD_ERASE_GRP_START;
    sdioCmdInitStructure.SDIO_Response = SDIO_Response_Short;
    sdioCmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    sdioCmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&sdioCmdInitStructure);

    errorstatus = CmdResp1Error(SD_CMD_SD_ERASE_GRP_START);
    if (errorstatus != SD_OK)
    {
      return(errorstatus);
    }

    /*!< Send CMD33 SD_ERASE_GRP_END with argument as addr  */
    sdioCmdInitStructure.SDIO_Argument = (uint32_t)endaddr;
    sdioCmdInitStructure.SDIO_CmdIndex = SD_CMD_SD_ERASE_GRP_END;
    sdioCmdInitStructure.SDIO_Response = SDIO_Response_Short;
    sdioCmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    sdioCmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&sdioCmdInitStructure);

    errorstatus = CmdResp1Error(SD_CMD_SD_ERASE_GRP_END);
    if (errorstatus != SD_OK)
    {
      return(errorstatus);
    }
  }

  /*!< Send CMD38 ERASE */
  sdioCmdInitStructure.SDIO_Argument = 0;
  sdioCmdInitStructure.SDIO_CmdIndex = SD_CMD_ERASE;
  sdioCmdInitStructure.SDIO_Response = SDIO_Response_Short;
  sdioCmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  sdioCmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&sdioCmdInitStructure);

  errorstatus = CmdResp1Error(SD_CMD_ERASE);

  if (errorstatus != SD_OK)
  {
    return(errorstatus);
  }

  for (delay = 0; delay < maxdelay; delay++)
  {}

  /*!< Wait till the card is in programming state */
  errorstatus = IsCardProgramming(&cardstate);
  delay = SD_DATATIMEOUT;
  while ((delay > 0) && (errorstatus == SD_OK) && ((SD_CARD_PROGRAMMING == cardstate) || (SD_CARD_RECEIVING == cardstate)))
  {
    errorstatus = IsCardProgramming(&cardstate);
    delay--;
  }

  return(errorstatus);
}

/**
  * @brief  Allows to read one block from a specified address in a card. The Data
  *         transfer can be managed by DMA mode or Polling mode. 
  * @note   This operation should be followed by two functions to check if the 
  *         DMA Controller and SD Card status.
  *          - SD_ReadWaitOperation(): this function insure that the DMA
  *            controller has finished all data transfer.
  *          - SD_GetStatus(): to check that the SD Card has finished the 
  *            data transfer and it is ready for data.            
  * @param  readbuff: pointer to the buffer that will contain the received data
  * @param  ReadAddr: Address from where data are to be read.  
  * @param  BlockSize: the SD card Data block size. The Block size should be 512.
  * @retval SD_Error: SD Card Error code.
  */
SD_Error SD_ReadBlock(uint8_t *readbuff, uint64_t ReadAddr, uint16_t BlockSize)
{
  SD_Error errorstatus = SD_OK;
#if defined (SD_POLLING_MODE) 
  uint32_t count = 0, *tempbuff = (uint32_t *)readbuff;
#endif

  TransferError = SD_OK;
  TransferEnd = 0;
  StopCondition = 0;

  SDIO->DCTRL = 0x0;

#if defined (SD_DMA_MODE)
  SDIO_ITConfig(SDIO_IT_DCRCFAIL | SDIO_IT_DTIMEOUT | SDIO_IT_DATAEND | SDIO_IT_RXOVERR | SDIO_IT_STBITERR, ENABLE);
  SDIO_DMACmd(ENABLE);
  SD_LowLevel_DMA_RxConfig((uint32_t *)readbuff, BlockSize);
#endif

  if (cardType == SDIO_HIGH_CAPACITY_SD_CARD)
  {
    BlockSize = 512;
    ReadAddr /= 512;
  }

  /* Set Block Size for Card */ 
  sdioCmdInitStructure.SDIO_Argument = (uint32_t) BlockSize;
  sdioCmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_BLOCKLEN;
  sdioCmdInitStructure.SDIO_Response = SDIO_Response_Short;
  sdioCmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  sdioCmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&sdioCmdInitStructure);

  errorstatus = CmdResp1Error(SD_CMD_SET_BLOCKLEN);

  if (SD_OK != errorstatus)
  {
    return(errorstatus);
  }

  sdioDataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT;
  sdioDataInitStructure.SDIO_DataLength = BlockSize;
  sdioDataInitStructure.SDIO_DataBlockSize = (uint32_t) 9 << 4;
  sdioDataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToSDIO;
  sdioDataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
  sdioDataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
  SDIO_DataConfig(&sdioDataInitStructure);

  /*!< Send CMD17 READ_SINGLE_BLOCK */
  sdioCmdInitStructure.SDIO_Argument = (uint32_t)ReadAddr;
  sdioCmdInitStructure.SDIO_CmdIndex = SD_CMD_READ_SINGLE_BLOCK;
  sdioCmdInitStructure.SDIO_Response = SDIO_Response_Short;
  sdioCmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  sdioCmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&sdioCmdInitStructure);

  errorstatus = CmdResp1Error(SD_CMD_READ_SINGLE_BLOCK);

  if (errorstatus != SD_OK)
  {
    return(errorstatus);
  }

#if defined (SD_POLLING_MODE)  
  /*!< In case of single block transfer, no need of stop transfer at all.*/
  /*!< Polling mode */
  while (!(SDIO->STA &(SDIO_FLAG_RXOVERR | SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_DBCKEND | SDIO_FLAG_STBITERR)))
  {
    if (SDIO_GetFlagStatus(SDIO_FLAG_RXFIFOHF) != RESET)
    {
      for (count = 0; count < 8; count++)
      {
        *(tempbuff + count) = SDIO_ReadData();
      }
      tempbuff += 8;
    }
  }

  if (SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT) != RESET)
  {
    SDIO_ClearFlag(SDIO_FLAG_DTIMEOUT);
    errorstatus = SD_DATA_TIMEOUT;
    return(errorstatus);
  }
  else if (SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL) != RESET)
  {
    SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL);
    errorstatus = SD_DATA_CRC_FAIL;
    return(errorstatus);
  }
  else if (SDIO_GetFlagStatus(SDIO_FLAG_RXOVERR) != RESET)
  {
    SDIO_ClearFlag(SDIO_FLAG_RXOVERR);
    errorstatus = SD_RX_OVERRUN;
    return(errorstatus);
  }
  else if (SDIO_GetFlagStatus(SDIO_FLAG_STBITERR) != RESET)
  {
    SDIO_ClearFlag(SDIO_FLAG_STBITERR);
    errorstatus = SD_START_BIT_ERR;
    return(errorstatus);
  }
  count = SD_DATATIMEOUT;
  while ((SDIO_GetFlagStatus(SDIO_FLAG_RXDAVL) != RESET) && (count > 0))
  {
    *tempbuff = SDIO_ReadData();
    tempbuff++;
    count--;
  }

  /*!< Clear all the static flags */
  SDIO_ClearFlag(SDIO_STATIC_FLAGS);

#endif

  return(errorstatus);
}

/**
  * @brief  Configures the DMA2 Channel4 for SDIO Tx request.
  * @param  BufferSRC: pointer to the source buffer
  * @param  BufferSize: buffer size
  * @retval None
  */
void SD_LowLevel_DMA_TxConfig(uint32_t *BufferSRC, uint32_t BufferSize)
{
  DMA_InitTypeDef SDDMA_InitStructure;

  DMA_ClearFlag(SD_SDIO_DMA_STREAM, SD_SDIO_DMA_FLAG_FEIF | SD_SDIO_DMA_FLAG_DMEIF | SD_SDIO_DMA_FLAG_TEIF | SD_SDIO_DMA_FLAG_HTIF | SD_SDIO_DMA_FLAG_TCIF);

  /* DMA2 Stream3  or Stream6 disable */
  DMA_Cmd(SD_SDIO_DMA_STREAM, DISABLE);

  /* DMA2 Stream3  or Stream6 Config */
  DMA_DeInit(SD_SDIO_DMA_STREAM);

  SDDMA_InitStructure.DMA_Channel = SD_SDIO_DMA_CHANNEL;
  SDDMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)SDIO_FIFO_ADDRESS;
  SDDMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)BufferSRC;
  SDDMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
  SDDMA_InitStructure.DMA_BufferSize = BufferSize;
  SDDMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  SDDMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  SDDMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
  SDDMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
  SDDMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  SDDMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  SDDMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
  SDDMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  SDDMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_INC4;
  SDDMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_INC4;
  DMA_Init(SD_SDIO_DMA_STREAM, &SDDMA_InitStructure);
  DMA_ITConfig(SD_SDIO_DMA_STREAM, DMA_IT_TC, ENABLE);
  DMA_FlowControllerConfig(SD_SDIO_DMA_STREAM, DMA_FlowCtrl_Peripheral);

  /* DMA2 Stream3  or Stream6 enable */
  DMA_Cmd(SD_SDIO_DMA_STREAM, ENABLE);
}

/**
  * @brief  Configures the DMA2 Channel4 for SDIO Rx request.
  * @param  BufferDST: pointer to the destination buffer
  * @param  BufferSize: buffer size
  * @retval None
  */
void SD_LowLevel_DMA_RxConfig(uint32_t *BufferDST, uint32_t BufferSize)
{
  DMA_InitTypeDef SDDMA_InitStructure;

  DMA_ClearFlag(SD_SDIO_DMA_STREAM, SD_SDIO_DMA_FLAG_FEIF | SD_SDIO_DMA_FLAG_DMEIF | SD_SDIO_DMA_FLAG_TEIF | SD_SDIO_DMA_FLAG_HTIF | SD_SDIO_DMA_FLAG_TCIF);

  /* DMA2 Stream3  or Stream6 disable */
  DMA_Cmd(SD_SDIO_DMA_STREAM, DISABLE);

  /* DMA2 Stream3 or Stream6 Config */
  DMA_DeInit(SD_SDIO_DMA_STREAM);

  SDDMA_InitStructure.DMA_Channel = SD_SDIO_DMA_CHANNEL;
  SDDMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)SDIO_FIFO_ADDRESS;
  SDDMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)BufferDST;
  SDDMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
  SDDMA_InitStructure.DMA_BufferSize = BufferSize;
  SDDMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  SDDMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  SDDMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
  SDDMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
  SDDMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  SDDMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  SDDMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
  SDDMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  SDDMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_INC4;
  SDDMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_INC4;
  DMA_Init(SD_SDIO_DMA_STREAM, &SDDMA_InitStructure);
  DMA_ITConfig(SD_SDIO_DMA_STREAM, DMA_IT_TC, ENABLE);
  DMA_FlowControllerConfig(SD_SDIO_DMA_STREAM, DMA_FlowCtrl_Peripheral);

  /* DMA2 Stream3 or Stream6 enable */
  DMA_Cmd(SD_SDIO_DMA_STREAM, ENABLE);
}

/**
  * @brief  Allows to read blocks from a specified address  in a card.  The Data
  *         transfer can be managed by DMA mode or Polling mode. 
  * @note   This operation should be followed by two functions to check if the 
  *         DMA Controller and SD Card status.
  *          - SD_ReadWaitOperation(): this function insure that the DMA
  *            controller has finished all data transfer.
  *          - SD_GetStatus(): to check that the SD Card has finished the 
  *            data transfer and it is ready for data.   
  * @param  readbuff: pointer to the buffer that will contain the received data.
  * @param  ReadAddr: Address from where data are to be read.
  * @param  BlockSize: the SD card Data block size. The Block size should be 512.
  * @param  NumberOfBlocks: number of blocks to be read.
  * @retval SD_Error: SD Card Error code.
  */
SD_Error SD_ReadMultiBlocks(uint8_t *readbuff, uint64_t ReadAddr, uint16_t BlockSize, uint32_t NumberOfBlocks)
{
  SD_Error errorstatus = SD_OK;
  TransferError = SD_OK;
  TransferEnd = 0;
  StopCondition = 1;
	
  SDIO->DCTRL = 0x0;

  SDIO_ITConfig(SDIO_IT_DCRCFAIL | SDIO_IT_DTIMEOUT | SDIO_IT_DATAEND | SDIO_IT_RXOVERR | SDIO_IT_STBITERR, ENABLE);
  SD_LowLevel_DMA_RxConfig((uint32_t *)readbuff, (NumberOfBlocks * BlockSize));
  SDIO_DMACmd(ENABLE);

  if (cardType == SDIO_HIGH_CAPACITY_SD_CARD)
  {
    BlockSize = 512;
    ReadAddr /= 512;
  }

  /*!< Set Block Size for Card */
  sdioCmdInitStructure.SDIO_Argument = (uint32_t) BlockSize;
  sdioCmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_BLOCKLEN;
  sdioCmdInitStructure.SDIO_Response = SDIO_Response_Short;
  sdioCmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  sdioCmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&sdioCmdInitStructure);

  errorstatus = CmdResp1Error(SD_CMD_SET_BLOCKLEN);

  if (SD_OK != errorstatus)
  {
    return(errorstatus);
  }
    
  sdioDataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT;
  sdioDataInitStructure.SDIO_DataLength = NumberOfBlocks * BlockSize;
  sdioDataInitStructure.SDIO_DataBlockSize = (uint32_t) 9 << 4;
  sdioDataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToSDIO;
  sdioDataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
  sdioDataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
  SDIO_DataConfig(&sdioDataInitStructure);

  /*!< Send CMD18 READ_MULT_BLOCK with argument data address */
  sdioCmdInitStructure.SDIO_Argument = (uint32_t)ReadAddr;
  sdioCmdInitStructure.SDIO_CmdIndex = SD_CMD_READ_MULT_BLOCK;
  sdioCmdInitStructure.SDIO_Response = SDIO_Response_Short;
  sdioCmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  sdioCmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&sdioCmdInitStructure);

  errorstatus = CmdResp1Error(SD_CMD_READ_MULT_BLOCK);

  if (errorstatus != SD_OK)
  {
    return(errorstatus);
  }

  return(errorstatus);
}

/**
  * @brief  Allows to write one block starting from a specified address in a card.
  *         The Data transfer can be managed by DMA mode or Polling mode.
  * @note   This operation should be followed by two functions to check if the 
  *         DMA Controller and SD Card status.
  *          - SD_ReadWaitOperation(): this function insure that the DMA
  *            controller has finished all data transfer.
  *          - SD_GetStatus(): to check that the SD Card has finished the 
  *            data transfer and it is ready for data.      
  * @param  writebuff: pointer to the buffer that contain the data to be transferred.
  * @param  WriteAddr: Address from where data are to be read.   
  * @param  BlockSize: the SD card Data block size. The Block size should be 512.
  * @retval SD_Error: SD Card Error code.
  */
SD_Error SD_WriteBlock(const uint8_t *writebuff, uint64_t WriteAddr, uint16_t BlockSize)
{
  SD_Error errorstatus = SD_OK;

#if defined (SD_POLLING_MODE)
  uint32_t bytestransferred = 0, count = 0, restwords = 0;
  uint32_t *tempbuff = (uint32_t *)writebuff;
#endif

  TransferError = SD_OK;
  TransferEnd = 0;
  StopCondition = 0;

  SDIO->DCTRL = 0x0;

#if defined (SD_DMA_MODE)
  SDIO_ITConfig(SDIO_IT_DCRCFAIL | SDIO_IT_DTIMEOUT | SDIO_IT_DATAEND | SDIO_IT_RXOVERR | SDIO_IT_STBITERR, ENABLE);
  SD_LowLevel_DMA_TxConfig((uint32_t *)writebuff, BlockSize);
  SDIO_DMACmd(ENABLE);
#endif

  if (cardType == SDIO_HIGH_CAPACITY_SD_CARD)
  {
    BlockSize = 512;
    WriteAddr /= 512;
  }

  /* Set Block Size for Card */ 
  sdioCmdInitStructure.SDIO_Argument = (uint32_t) BlockSize;
  sdioCmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_BLOCKLEN;
  sdioCmdInitStructure.SDIO_Response = SDIO_Response_Short;
  sdioCmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  sdioCmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&sdioCmdInitStructure);

  errorstatus = CmdResp1Error(SD_CMD_SET_BLOCKLEN);

  if (SD_OK != errorstatus)
  {
    return(errorstatus);
  }

  /*!< Send CMD24 WRITE_SINGLE_BLOCK */
  sdioCmdInitStructure.SDIO_Argument = (uint32_t)WriteAddr;
  sdioCmdInitStructure.SDIO_CmdIndex = SD_CMD_WRITE_SINGLE_BLOCK;
  sdioCmdInitStructure.SDIO_Response = SDIO_Response_Short;
  sdioCmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  sdioCmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&sdioCmdInitStructure);

  errorstatus = CmdResp1Error(SD_CMD_WRITE_SINGLE_BLOCK);

  if (errorstatus != SD_OK)
  {
    return(errorstatus);
  }

  sdioDataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT;
  sdioDataInitStructure.SDIO_DataLength = BlockSize;
  sdioDataInitStructure.SDIO_DataBlockSize = (uint32_t) 9 << 4;
  sdioDataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToCard;
  sdioDataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
  sdioDataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
  SDIO_DataConfig(&sdioDataInitStructure);

  /*!< In case of single data block transfer no need of stop command at all */
#if defined (SD_POLLING_MODE) 
  while (!(SDIO->STA & (SDIO_FLAG_DBCKEND | SDIO_FLAG_TXUNDERR | SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_STBITERR)))
  {
    if (SDIO_GetFlagStatus(SDIO_FLAG_TXFIFOHE) != RESET)
    {
      if ((512 - bytestransferred) < 32)
      {
        restwords = ((512 - bytestransferred) % 4 == 0) ? ((512 - bytestransferred) / 4) : (( 512 -  bytestransferred) / 4 + 1);
        for (count = 0; count < restwords; count++, tempbuff++, bytestransferred += 4)
        {
          SDIO_WriteData(*tempbuff);
        }
      }
      else
      {
        for (count = 0; count < 8; count++)
        {
          SDIO_WriteData(*(tempbuff + count));
        }
        tempbuff += 8;
        bytestransferred += 32;
      }
    }
  }
  if (SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT) != RESET)
  {
    SDIO_ClearFlag(SDIO_FLAG_DTIMEOUT);
    errorstatus = SD_DATA_TIMEOUT;
    return(errorstatus);
  }
  else if (SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL) != RESET)
  {
    SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL);
    errorstatus = SD_DATA_CRC_FAIL;
    return(errorstatus);
  }
  else if (SDIO_GetFlagStatus(SDIO_FLAG_TXUNDERR) != RESET)
  {
    SDIO_ClearFlag(SDIO_FLAG_TXUNDERR);
    errorstatus = SD_TX_UNDERRUN;
    return(errorstatus);
  }
  else if (SDIO_GetFlagStatus(SDIO_FLAG_STBITERR) != RESET)
  {
    SDIO_ClearFlag(SDIO_FLAG_STBITERR);
    errorstatus = SD_START_BIT_ERR;
    return(errorstatus);
  }
  
#endif
  
  return(errorstatus);
}

/**
  * @brief  Allows to write blocks starting from a specified address in a card.
  *         The Data transfer can be managed by DMA mode only. 
  * @note   This operation should be followed by two functions to check if the 
  *         DMA Controller and SD Card status.
  *          - SD_ReadWaitOperation(): this function insure that the DMA
  *            controller has finished all data transfer.
  *          - SD_GetStatus(): to check that the SD Card has finished the 
  *            data transfer and it is ready for data.     
  * @param  WriteAddr: Address from where data are to be read.
  * @param  writebuff: pointer to the buffer that contain the data to be transferred.
  * @param  BlockSize: the SD card Data block size. The Block size should be 512.
  * @param  NumberOfBlocks: number of blocks to be written.
  * @retval SD_Error: SD Card Error code.
  */
SD_Error SD_WriteMultiBlocks(const uint8_t *writebuff, uint64_t WriteAddr, uint16_t BlockSize, uint32_t NumberOfBlocks)
{
  SD_Error errorstatus = SD_OK;

  TransferError = SD_OK;
  TransferEnd = 0;
  StopCondition = 1;
  SDIO->DCTRL = 0x0;

  SDIO_ITConfig(SDIO_IT_DCRCFAIL | SDIO_IT_DTIMEOUT | SDIO_IT_DATAEND | SDIO_IT_TXUNDERR | SDIO_IT_STBITERR, ENABLE);
  SD_LowLevel_DMA_TxConfig((uint32_t *)writebuff, (NumberOfBlocks * BlockSize));
  SDIO_DMACmd(ENABLE);

  if (cardType == SDIO_HIGH_CAPACITY_SD_CARD)
  {
    BlockSize = 512;
    WriteAddr /= 512;
  }

  /* Set Block Size for Card */ 
  sdioCmdInitStructure.SDIO_Argument = (uint32_t) BlockSize;
  sdioCmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_BLOCKLEN;
  sdioCmdInitStructure.SDIO_Response = SDIO_Response_Short;
  sdioCmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  sdioCmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&sdioCmdInitStructure);

  errorstatus = CmdResp1Error(SD_CMD_SET_BLOCKLEN);

  if (SD_OK != errorstatus)
  {
    return(errorstatus);
  }
  
  /*!< To improve performance */
  sdioCmdInitStructure.SDIO_Argument = (uint32_t) (RCA << 16);
  sdioCmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_CMD;
  sdioCmdInitStructure.SDIO_Response = SDIO_Response_Short;
  sdioCmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  sdioCmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&sdioCmdInitStructure);


  errorstatus = CmdResp1Error(SD_CMD_APP_CMD);

  if (errorstatus != SD_OK)
  {
    return(errorstatus);
  }
  /*!< To improve performance */
  sdioCmdInitStructure.SDIO_Argument = (uint32_t)NumberOfBlocks;
  sdioCmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_BLOCK_COUNT;
  sdioCmdInitStructure.SDIO_Response = SDIO_Response_Short;
  sdioCmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  sdioCmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&sdioCmdInitStructure);

  errorstatus = CmdResp1Error(SD_CMD_SET_BLOCK_COUNT);

  if (errorstatus != SD_OK)
  {
    return(errorstatus);
  }


  /*!< Send CMD25 WRITE_MULT_BLOCK with argument data address */
  sdioCmdInitStructure.SDIO_Argument = (uint32_t)WriteAddr;
  sdioCmdInitStructure.SDIO_CmdIndex = SD_CMD_WRITE_MULT_BLOCK;
  sdioCmdInitStructure.SDIO_Response = SDIO_Response_Short;
  sdioCmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  sdioCmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&sdioCmdInitStructure);

  errorstatus = CmdResp1Error(SD_CMD_WRITE_MULT_BLOCK);

  if (SD_OK != errorstatus)
  {
    return(errorstatus);
  }

  sdioDataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT;
  sdioDataInitStructure.SDIO_DataLength = NumberOfBlocks * BlockSize;
  sdioDataInitStructure.SDIO_DataBlockSize = (uint32_t) 9 << 4;
  sdioDataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToCard;
  sdioDataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
  sdioDataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
  SDIO_DataConfig(&sdioDataInitStructure);

  return(errorstatus);
}

/**
  * @brief  Aborts an ongoing data transfer.
  * @param  None
  * @retval SD_Error: SD Card Error code.
  */
SD_Error SD_StopTransfer(void)
{
  SD_Error errorstatus = SD_OK;

  /*!< Send CMD12 STOP_TRANSMISSION  */
  sdioCmdInitStructure.SDIO_Argument = 0x0;
  sdioCmdInitStructure.SDIO_CmdIndex = SD_CMD_STOP_TRANSMISSION;
  sdioCmdInitStructure.SDIO_Response = SDIO_Response_Short;
  sdioCmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  sdioCmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&sdioCmdInitStructure);

  errorstatus = CmdResp1Error(SD_CMD_STOP_TRANSMISSION);

  return(errorstatus);
}



/**
  * @brief  Enables wide bus opeartion for the requeseted card if supported by 
  *         card.
  * @param  WideMode: Specifies the SD card wide bus mode. 
  *   This parameter can be one of the following values:
  *     @arg SDIO_BusWide_8b: 8-bit data transfer (Only for MMC)
  *     @arg SDIO_BusWide_4b: 4-bit data transfer
  *     @arg SDIO_BusWide_1b: 1-bit data transfer
  * @retval SD_Error: SD Card Error code.
  */
SD_Error SD_GetCardStatus(SD_CardStatus *cardstatus)
{
  SD_Error errorstatus = SD_OK;
  uint8_t tmp = 0;

  errorstatus = SD_SendSDStatus((uint32_t *)SDSTATUS_Tab);

  if (errorstatus  != SD_OK)
  {
    return(errorstatus);
  }

  /*!< Byte 0 */
  tmp = (uint8_t)((SDSTATUS_Tab[0] & 0xC0) >> 6);
  cardstatus->DAT_BUS_WIDTH = tmp;

  /*!< Byte 0 */
  tmp = (uint8_t)((SDSTATUS_Tab[0] & 0x20) >> 5);
  cardstatus->SECURED_MODE = tmp;

  /*!< Byte 2 */
  tmp = (uint8_t)((SDSTATUS_Tab[2] & 0xFF));
  cardstatus->SD_CARD_TYPE = tmp << 8;

  /*!< Byte 3 */
  tmp = (uint8_t)((SDSTATUS_Tab[3] & 0xFF));
  cardstatus->SD_CARD_TYPE |= tmp;

  /*!< Byte 4 */
  tmp = (uint8_t)(SDSTATUS_Tab[4] & 0xFF);
  cardstatus->SIZE_OF_PROTECTED_AREA = tmp << 24;

  /*!< Byte 5 */
  tmp = (uint8_t)(SDSTATUS_Tab[5] & 0xFF);
  cardstatus->SIZE_OF_PROTECTED_AREA |= tmp << 16;

  /*!< Byte 6 */
  tmp = (uint8_t)(SDSTATUS_Tab[6] & 0xFF);
  cardstatus->SIZE_OF_PROTECTED_AREA |= tmp << 8;

  /*!< Byte 7 */
  tmp = (uint8_t)(SDSTATUS_Tab[7] & 0xFF);
  cardstatus->SIZE_OF_PROTECTED_AREA |= tmp;

  /*!< Byte 8 */
  tmp = (uint8_t)((SDSTATUS_Tab[8] & 0xFF));
  cardstatus->SPEED_CLASS = tmp;

  /*!< Byte 9 */
  tmp = (uint8_t)((SDSTATUS_Tab[9] & 0xFF));
  cardstatus->PERFORMANCE_MOVE = tmp;

  /*!< Byte 10 */
  tmp = (uint8_t)((SDSTATUS_Tab[10] & 0xF0) >> 4);
  cardstatus->AU_SIZE = tmp;

  /*!< Byte 11 */
  tmp = (uint8_t)(SDSTATUS_Tab[11] & 0xFF);
  cardstatus->ERASE_SIZE = tmp << 8;

  /*!< Byte 12 */
  tmp = (uint8_t)(SDSTATUS_Tab[12] & 0xFF);
  cardstatus->ERASE_SIZE |= tmp;

  /*!< Byte 13 */
  tmp = (uint8_t)((SDSTATUS_Tab[13] & 0xFC) >> 2);
  cardstatus->ERASE_TIMEOUT = tmp;

  /*!< Byte 13 */
  tmp = (uint8_t)((SDSTATUS_Tab[13] & 0x3));
  cardstatus->ERASE_OFFSET = tmp;
 
  return(errorstatus);
}

/**
  * @brief  Returns the current SD card's status.
  * @param  psdstatus: pointer to the buffer that will contain the SD card status 
  *         (SD Status register).
  * @retval SD_Error: SD Card Error code.
  */
SD_Error SD_SendSDStatus(uint32_t *psdstatus)
{
  SD_Error errorstatus = SD_OK;
  uint32_t count = 0;

  if (SDIO_GetResponse(SDIO_RESP1) & SD_CARD_LOCKED)
  {
    errorstatus = SD_LOCK_UNLOCK_FAILED;
    return(errorstatus);
  }

  /*!< Set block size for card if it is not equal to current block size for card. */
  sdioCmdInitStructure.SDIO_Argument = 64;
  sdioCmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_BLOCKLEN;
  sdioCmdInitStructure.SDIO_Response = SDIO_Response_Short;
  sdioCmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  sdioCmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&sdioCmdInitStructure);

  errorstatus = CmdResp1Error(SD_CMD_SET_BLOCKLEN);

  if (errorstatus != SD_OK)
  {
    return(errorstatus);
  }

  /*!< CMD55 */
  sdioCmdInitStructure.SDIO_Argument = (uint32_t) RCA << 16;
  sdioCmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_CMD;
  sdioCmdInitStructure.SDIO_Response = SDIO_Response_Short;
  sdioCmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  sdioCmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&sdioCmdInitStructure);
  errorstatus = CmdResp1Error(SD_CMD_APP_CMD);

  if (errorstatus != SD_OK)
  {
    return(errorstatus);
  }

  sdioDataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT;
  sdioDataInitStructure.SDIO_DataLength = 64;
  sdioDataInitStructure.SDIO_DataBlockSize = SDIO_DataBlockSize_64b;
  sdioDataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToSDIO;
  sdioDataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
  sdioDataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
  SDIO_DataConfig(&sdioDataInitStructure);

  /*!< Send ACMD13 SD_APP_STAUS  with argument as card's RCA.*/
  sdioCmdInitStructure.SDIO_Argument = 0;
  sdioCmdInitStructure.SDIO_CmdIndex = SD_CMD_SD_APP_STAUS;
  sdioCmdInitStructure.SDIO_Response = SDIO_Response_Short;
  sdioCmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  sdioCmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&sdioCmdInitStructure);
  errorstatus = CmdResp1Error(SD_CMD_SD_APP_STAUS);

  if (errorstatus != SD_OK)
  {
    return(errorstatus);
  }

  while (!(SDIO->STA &(SDIO_FLAG_RXOVERR | SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_DBCKEND | SDIO_FLAG_STBITERR)))
  {
    if (SDIO_GetFlagStatus(SDIO_FLAG_RXFIFOHF) != RESET)
    {
      for (count = 0; count < 8; count++)
      {
        *(psdstatus + count) = SDIO_ReadData();
      }
      psdstatus += 8;
    }
  }

  if (SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT) != RESET)
  {
    SDIO_ClearFlag(SDIO_FLAG_DTIMEOUT);
    errorstatus = SD_DATA_TIMEOUT;
    return(errorstatus);
  }
  else if (SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL) != RESET)
  {
    SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL);
    errorstatus = SD_DATA_CRC_FAIL;
    return(errorstatus);
  }
  else if (SDIO_GetFlagStatus(SDIO_FLAG_RXOVERR) != RESET)
  {
    SDIO_ClearFlag(SDIO_FLAG_RXOVERR);
    errorstatus = SD_RX_OVERRUN;
    return(errorstatus);
  }
  else if (SDIO_GetFlagStatus(SDIO_FLAG_STBITERR) != RESET)
  {
    SDIO_ClearFlag(SDIO_FLAG_STBITERR);
    errorstatus = SD_START_BIT_ERR;
    return(errorstatus);
  }

  count = SD_DATATIMEOUT;
  while ((SDIO_GetFlagStatus(SDIO_FLAG_RXDAVL) != RESET) && (count > 0))
  {
    *psdstatus = SDIO_ReadData();
    psdstatus++;
    count--;
  }
  /*!< Clear all the static status flags*/
  SDIO_ClearFlag(SDIO_STATIC_FLAGS);

  return(errorstatus);
}

/**
  * @brief  Gets the cuurent sd card data transfer status.
  * @param  None
  * @retval SDTransferState: Data Transfer state.
  *   This value can be: 
  *        - SD_TRANSFER_OK: No data transfer is acting
  *        - SD_TRANSFER_BUSY: Data transfer is acting
  */
SDTransferState SD_GetStatus(void)
{
  SDCardState cardstate =  SD_CARD_TRANSFER;

  cardstate = SD_GetState();
  
  if (cardstate == SD_CARD_TRANSFER)
  {
    return(SD_TRANSFER_OK);
  }
  else if(cardstate == SD_CARD_ERROR)
  {
    return (SD_TRANSFER_ERROR);
  }
  else
  {
    return(SD_TRANSFER_BUSY);
  }
}

/**
  * @brief  Returns the current card's state.
  * @param  None
  * @retval SDCardState: SD Card Error or SD Card Current State.
  */
SDCardState SD_GetState(void)
{
  uint32_t resp1 = 0;
  
  if(SD_Detect()== SD_PRESENT)
  {
    if (SD_SendStatus(&resp1) != SD_OK)
    {
      return SD_CARD_ERROR;
    }
    else
    {
      return (SDCardState)((resp1 >> 9) & 0x0F);
    }
  }
  else
  {
    return SD_CARD_ERROR;
  }
}

/**
 * @brief  Detect if SD card is correctly plugged in the memory slot.
 * @param  None
 * @retval Return if SD is detected or not
 */
uint8_t SD_Detect(void)
{
  __IO uint8_t status = SD_PRESENT;

  /*!< Check GPIO to detect SD */
//  if (GPIO_ReadInputDataBit(SD_DETECT_GPIO_PORT, SD_DETECT_PIN) != Bit_RESET)
//  {
//    status = SD_NOT_PRESENT;
//  }
  return SD_PRESENT;
}


/**
  * @brief  Returns the current card's status.
  * @param  pcardstatus: pointer to the buffer that will contain the SD card 
  *         status (Card Status register).
  * @retval SD_Error: SD Card Error code.
  */
SD_Error SD_SendStatus(uint32_t *pcardstatus)
{
  SD_Error errorstatus = SD_OK;

  if (pcardstatus == NULL)
  {
    errorstatus = SD_INVALID_PARAMETER;
    return(errorstatus);
  }

  sdioCmdInitStructure.SDIO_Argument = (uint32_t) RCA << 16;
  sdioCmdInitStructure.SDIO_CmdIndex = SD_CMD_SEND_STATUS;
  sdioCmdInitStructure.SDIO_Response = SDIO_Response_Short;
  sdioCmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  sdioCmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&sdioCmdInitStructure);

  errorstatus = CmdResp1Error(SD_CMD_SEND_STATUS);

  if (errorstatus != SD_OK)
  {
    return(errorstatus);
  }

  *pcardstatus = SDIO_GetResponse(SDIO_RESP1);

  return(errorstatus);
}

/**
  * @brief  This function waits until the SDIO DMA data transfer is finished. 
  * @param  None.
  * @retval None.
  */
void SD_ProcessDMAIRQ(void)
{
  OS_ERR err;
  if(DMA_GetITStatus(SD_SDIO_DMA_STREAM, DMA_IT_TCIF3) != RESET)
  {
    DMAEndOfTransfer = 0x01;
    DMA_ClearITPendingBit(SD_SDIO_DMA_STREAM, DMA_IT_TCIF3);
  }
  if(DMA_GetITStatus(SD_SDIO_DMA_STREAM, DMA_IT_FEIF3) != RESET)
  {
    DMAEndOfTransfer = 0x01;
    DMA_ClearITPendingBit(SD_SDIO_DMA_STREAM, DMA_IT_FEIF3);
  }
  OSSemPost(&appData.tfKardDmaTc, OS_OPT_POST_1, &err);
}

/**
  * @brief  Allows to process all the interrupts that are high.
  * @param  None
  * @retval SD_Error: SD Card Error code.
  */
SD_Error SD_ProcessIRQSrc(void)
{ 
  if (SDIO_GetITStatus(SDIO_IT_DATAEND) != RESET)
  {
    TransferError = SD_OK;
    SDIO_ClearITPendingBit(SDIO_IT_DATAEND);
    TransferEnd = 1;
  }  
  else if (SDIO_GetITStatus(SDIO_IT_DCRCFAIL) != RESET)
  {
    SDIO_ClearITPendingBit(SDIO_IT_DCRCFAIL);
    TransferError = SD_DATA_CRC_FAIL;
  }
  else if (SDIO_GetITStatus(SDIO_IT_DTIMEOUT) != RESET)
  {
    SDIO_ClearITPendingBit(SDIO_IT_DTIMEOUT);
    TransferError = SD_DATA_TIMEOUT;
  }
  else if (SDIO_GetITStatus(SDIO_IT_RXOVERR) != RESET)
  {
    SDIO_ClearITPendingBit(SDIO_IT_RXOVERR);
    TransferError = SD_RX_OVERRUN;
  }
  else if (SDIO_GetITStatus(SDIO_IT_TXUNDERR) != RESET)
  {
    SDIO_ClearITPendingBit(SDIO_IT_TXUNDERR);
    TransferError = SD_TX_UNDERRUN;
  }
  else if (SDIO_GetITStatus(SDIO_IT_STBITERR) != RESET)
  {
    SDIO_ClearITPendingBit(SDIO_IT_STBITERR);
    TransferError = SD_START_BIT_ERR;
  }

  SDIO_ITConfig(SDIO_IT_DCRCFAIL | SDIO_IT_DTIMEOUT | SDIO_IT_DATAEND |
                SDIO_IT_TXFIFOHE | SDIO_IT_RXFIFOHF | SDIO_IT_TXUNDERR |
                SDIO_IT_RXOVERR | SDIO_IT_STBITERR, DISABLE);
  return(TransferError);
}

/**
  * @brief  This function waits until the SDIO DMA data transfer is finished. 
  *         This function should be called after SDIO_WriteBlock() and
  *         SDIO_WriteMultiBlocks() function to insure that all data sent by the 
  *         card are already transferred by the DMA controller.        
  * @param  None.
  * @retval SD_Error: SD Card Error code.
  */
SD_Error SD_WaitWriteOperation(void)
{
  SD_Error errorstatus = SD_OK;
  uint32_t timeout;
  CPU_TS ts;
  OS_ERR err;
  timeout = SD_DATATIMEOUT;
  
  OSSemPend(&appData.tfKardDmaTc, 0, OS_OPT_PEND_BLOCKING, &ts, &err);
  while ((DMAEndOfTransfer == 0x00) && (TransferEnd == 0) && (TransferError == SD_OK) && (timeout > 0))
  {
    timeout--;
  }
  
  DMAEndOfTransfer = 0x00;

  timeout = SD_DATATIMEOUT;
  
  while(((SDIO->STA & SDIO_FLAG_TXACT)) && (timeout > 0))
  {
    timeout--;  
  }

  if (StopCondition == 1)
  {
    errorstatus = SD_StopTransfer();
    StopCondition = 0;
  }
  
  if ((timeout == 0) && (errorstatus == SD_OK))
  {
    errorstatus = SD_DATA_TIMEOUT;
  }
  
  /*!< Clear all the static flags */
  SDIO_ClearFlag(SDIO_STATIC_FLAGS);
  
  if (TransferError != SD_OK)
  {
    return(TransferError);
  }
  else
  {
    return(errorstatus);
  }
}

/**
  * @brief  This function waits until the SDIO DMA data transfer is finished. 
  *         This function should be called after SDIO_ReadMultiBlocks() function
  *         to insure that all data sent by the card are already transferred by 
  *         the DMA controller.
  * @param  None.
  * @retval SD_Error: SD Card Error code.
  */
SD_Error SD_WaitReadOperation(void)
{
  SD_Error errorstatus = SD_OK;
  uint32_t timeout;
  CPU_TS ts;
  OS_ERR err;

  timeout = SD_DATATIMEOUT;
  
  OSSemPend(&appData.tfKardDmaTc, 0, OS_OPT_PEND_BLOCKING, &ts, &err);
  while ((DMAEndOfTransfer == 0x00) && (TransferEnd == 0) && (TransferError == SD_OK) && (timeout > 0))
  {
    timeout--;
  }
  
  DMAEndOfTransfer = 0x00;

  timeout = SD_DATATIMEOUT;
  
  while(((SDIO->STA & SDIO_FLAG_RXACT)) && (timeout > 0))
  {
    timeout--;  
  }

  if (StopCondition == 1)
  {
    errorstatus = SD_StopTransfer();
    StopCondition = 0;
  }
  
  if ((timeout == 0) && (errorstatus == SD_OK))
  {
    errorstatus = SD_DATA_TIMEOUT;
  }
  
  /*!< Clear all the static flags */
  SDIO_ClearFlag(SDIO_STATIC_FLAGS);

  if (TransferError != SD_OK)
  {
    return(TransferError);
  }
  else
  {
    return(errorstatus);  
  }
}

//===========================================  End Of File  ===========================================//
