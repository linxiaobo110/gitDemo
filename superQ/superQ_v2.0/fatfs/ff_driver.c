#include "diskio.h"		/* FatFs lower layer API */

DSTATUS RAM_disk_status(BYTE pdrv)
{
  return STA_NOINIT;
}

DSTATUS MMC_disk_status(BYTE pdrv)
{
  return STA_NOINIT;
}

DSTATUS USB_disk_status(BYTE pdrv)
{
  return STA_NOINIT;
}

DSTATUS RAM_disk_initialize(BYTE pdrv)
{
  return STA_NOINIT;
}

DSTATUS MMC_disk_initialize(BYTE pdrv)
{
  return STA_NOINIT;
}

DSTATUS USB_disk_initialize(BYTE pdrv)
{
  return STA_NOINIT;
}

/* [IN] Physical drive number */
/* [OUT] Pointer to the read data buffer */
/* [IN] Start sector number */
/* [IN] Number of sectros to read */
DRESULT RAM_disk_read(BYTE pdrv, BYTE* buff, DWORD sector, UINT count)
{
  return RES_NOTRDY;
}

/* [IN] Physical drive number */
/* [OUT] Pointer to the read data buffer */
/* [IN] Start sector number */
/* [IN] Number of sectros to read */
DRESULT MMC_disk_read(BYTE pdrv, BYTE* buff, DWORD sector, UINT count)
{
  return RES_NOTRDY;
}

/* [IN] Physical drive number */
/* [OUT] Pointer to the read data buffer */
/* [IN] Start sector number */
/* [IN] Number of sectros to read */
DRESULT USB_disk_read(BYTE pdrv, BYTE* buff, DWORD sector, UINT count)
{
  return RES_NOTRDY;
}

/* [IN] Physical drive number */
/* [IN] Pointer to the data to be written */
/* [IN] Sector number to write from */
/* [IN] Number of sectros to read */
DRESULT RAM_disk_write (BYTE drv, const BYTE* buff, DWORD sector, UINT count)
{
  return RES_NOTRDY;
}

/* [IN] Physical drive number */
/* [IN] Pointer to the data to be written */
/* [IN] Sector number to write from */
/* [IN] Number of sectros to read */
DRESULT MMC_disk_write (BYTE drv, const BYTE* buff, DWORD sector, UINT count)
{
  return RES_NOTRDY;
}

/* [IN] Physical drive number */
/* [IN] Pointer to the data to be written */
/* [IN] Sector number to write from */
/* [IN] Number of sectros to read */
DRESULT USB_disk_write (BYTE drv, const BYTE* buff, DWORD sector, UINT count)
{
  return RES_NOTRDY;
}

/* [IN] Drive number */
/* [IN] Control command code */
/* [I/O] Parameter and data buffer */
DRESULT RAM_disk_ioctl (BYTE pdrv, BYTE cmd, void* buff)
{
  return RES_NOTRDY;
}

/* [IN] Drive number */
/* [IN] Control command code */
/* [I/O] Parameter and data buffer */
DRESULT MMC_disk_ioctl (BYTE pdrv, BYTE cmd, void* buff)
{
  return RES_NOTRDY;
}

/* [IN] Drive number */
/* [IN] Control command code */
/* [I/O] Parameter and data buffer */
DRESULT USB_disk_ioctl (BYTE pdrv, BYTE cmd, void* buff)
{
  return RES_NOTRDY;
}

DWORD get_fattime (void)
{
  return 0;
}
