/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */

/* Definitions of physical drive number for each drive */
#define DEV_RAM		0	/* Example: Map Ramdisk to physical drive 0 */
#define DEV_MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
#define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/
/* Physical drive nmuber to identify the drive */
DSTATUS disk_status (BYTE pdrv)
{
//	DSTATUS stat;
//	SDTransferState result;

	switch (pdrv) 
  {
    case DEV_RAM :
      
      //		result = RAM_disk_status();

      // translate the reslut code here

      break;

    case DEV_MMC :
    //	result = MMC_disk_status();
//        result = SD_GetStatus();
//      // translate the reslut code here
//      if(result != SD_TRANSFER_OK)
//      {
//        stat = STA_PROTECT;
//      }else
//      {
//        stat = 0;
//      }
      //stat = 0;
      return 0;

    case DEV_USB :
      //	result = USB_disk_status();

      // translate the reslut code here

		break;
	}
	return STA_NODISK;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;
	SD_Error result;

	switch (pdrv) {
	case DEV_RAM :
	//	result = RAM_disk_initialize();

		// translate the reslut code here

		break;

	case DEV_MMC :
		//result = MMC_disk_initialize();
    //result = Bsp_sdioInit();
    result = SD_OK;
		// translate the reslut code here
    if(result == SD_OK)
    {
      stat = 0;
    }else
    {
      stat = STA_NOINIT;
    }
		return stat;

	case DEV_USB :
		//result = USB_disk_initialize();

		// translate the reslut code here

		break;
	}
	return STA_NODISK;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	DRESULT res;
	SD_Error result;

	switch (pdrv) {
	case DEV_RAM :
		// translate the arguments here

	//	result = RAM_disk_read(buff, sector, count);

		// translate the reslut code here

		break;

	case DEV_MMC :
		// translate the arguments here
     if(count > 1)
     {
       result = SD_ReadMultiBlocks(buff, sector*BLOCK_SIZE, BLOCK_SIZE, count);
       result = SD_WaitReadOperation();
       while(SD_GetStatus() != SD_TRANSFER_OK);
     }else
     {
       result = SD_ReadBlock(buff, sector*BLOCK_SIZE, BLOCK_SIZE);
       result = SD_WaitReadOperation();
       while(SD_GetStatus() != SD_TRANSFER_OK);
     }
     if(result != SD_OK)
     {
       res = RES_ERROR;
     }else
     {
       res = RES_OK;
     }
		return res;

	case DEV_USB :
		// translate the arguments here

	//	result = USB_disk_read(buff, sector, count);

		// translate the reslut code here

		break;
	}

	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	DRESULT res;
	int result;

	switch (pdrv) {
	case DEV_RAM :
		// translate the arguments here

	//	result = RAM_disk_write(buff, sector, count);

		// translate the reslut code here

		//return res;
  break;

	case DEV_MMC :
		 if(count > 1)
     {
       result = SD_WriteMultiBlocks(buff, sector*BLOCK_SIZE, BLOCK_SIZE, count);
       result = SD_WaitWriteOperation();
       while(SD_GetStatus() != SD_TRANSFER_OK);
     }else
     {
       result = SD_WriteBlock(buff, sector*BLOCK_SIZE, BLOCK_SIZE);
       result = SD_WaitWriteOperation();
       while(SD_GetStatus() != SD_TRANSFER_OK);
     }
     if(result != SD_OK)
     {
       res = RES_ERROR;
     }else
     {
       res = RES_OK;
     }

		return res;

	case DEV_USB :
		// translate the arguments here

	//	result = USB_disk_write(buff, sector, count);

		// translate the reslut code here
    break;
		//return res;
	}

	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;
	//int result;
  u32 *temp; 

	switch (pdrv) {
	case DEV_RAM :

		// Process of the command for the RAM drive

		//return res;
    break;
	case DEV_MMC :
    switch(cmd)
    {
      case CTRL_SYNC:
        res = RES_OK;
        break;
      case GET_SECTOR_COUNT:
        temp = buff;
        *temp = (u32)(sdCardInfo.CardCapacity/sdCardInfo.CardBlockSize);
      break;
      case GET_BLOCK_SIZE:
        temp = buff;
        *temp = sdCardInfo.CardBlockSize;
      break;
    }
		// Process of the command for the MMC/SD card
    res = RES_OK;
		return res;

	case DEV_USB :

		// Process of the command the USB drive
    break;
		//return res;
	}

	return RES_PARERR;
}


