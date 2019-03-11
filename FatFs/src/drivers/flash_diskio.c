#include <string.h>
#include "ff_gen_drv.h"
#include "device_conf.h"
#include "flash_diskio.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Block Size in Bytes */
#define BLOCK_SIZE                512

/* Private variables ---------------------------------------------------------*/
/* Disk status */
static volatile DSTATUS Stat = STA_NOINIT;

/* Private function prototypes -----------------------------------------------*/
DSTATUS FLASHDISK_initialize (BYTE);
DSTATUS FLASHDISK_status (BYTE);
DRESULT FLASHDISK_read (BYTE, BYTE*, DWORD, UINT);
#if _USE_WRITE == 1
  DRESULT FLASHDISK_write (BYTE, const BYTE*, DWORD, UINT);
#endif /* _USE_WRITE == 1 */
#if _USE_IOCTL == 1
  DRESULT FLASHDISK_ioctl (BYTE, BYTE, void*);
#endif /* _USE_IOCTL == 1 */
  
const Diskio_drvTypeDef FLASHDISK_Driver =
{
  FLASHDISK_initialize,
  FLASHDISK_status,
  FLASHDISK_read, 
#if  _USE_WRITE == 1
  FLASHDISK_write,
#endif /* _USE_WRITE == 1 */  
#if  _USE_IOCTL == 1
  FLASHDISK_ioctl,
#endif /* _USE_IOCTL == 1 */
};

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initializes a Drive
  * @param  lun : not used 
  * @retval DSTATUS: Operation status
  */



DSTATUS FLASHDISK_initialize(BYTE lun)
{
  Stat = at_init();
  return Stat;
}

/**
  * @brief  Gets Disk Status
  * @param  lun : not used 
  * @retval DSTATUS: Operation status
  */
DSTATUS FLASHDISK_status(BYTE lun)
{
  return at_get_stat();
}

/**
  * @brief  Reads Sector(s)
  * @param  lun : not used 
  * @param  *buff: Data buffer to store read data
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to read (1..128)
  * @retval DRESULT: Operation result
  */
DRESULT FLASHDISK_read(BYTE lun, BYTE *buff, DWORD sector, UINT count)
{
  return at_read_blk(sector, buff, count);
}

/**
  * @brief  Writes Sector(s)
  * @param  lun : not used 
  * @param  *buff: Data to be written
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to write (1..128)
  * @retval DRESULT: Operation result
  */
#if _USE_WRITE == 1
DRESULT FLASHDISK_write(BYTE lun, const BYTE *buff, DWORD sector, UINT count)
{ 
  return at_prog_blk(sector, buff, count);
}
#endif /* _USE_WRITE == 1 */

/**
  * @brief  I/O control operation
  * @param  lun : not used 
  * @param  cmd: Control code
  * @param  *buff: Buffer to send/receive control data
  * @retval DRESULT: Operation result
  */
#if _USE_IOCTL == 1
DRESULT FLASHDISK_ioctl(BYTE lun, BYTE cmd, void *buff)
{
  DRESULT res = RES_ERROR;
  
  if (Stat & STA_NOINIT) return RES_NOTRDY;
  
  switch (cmd)
  {
  /* Make sure that no pending write process */
  case CTRL_SYNC :
    res = (DRESULT)at_get_stat();
    break;
  
  /* Get number of sectors on the disk (DWORD) */
  case GET_SECTOR_COUNT :
    *(DWORD*)buff = FLASH_DEVICE_SIZE / BLOCK_SIZE;
    res = RES_OK;
    break;
  
  /* Get R/W sector size (WORD) */
  case GET_SECTOR_SIZE :
    *(WORD*)buff = BLOCK_SIZE;
    res = RES_OK;
    break;
  
  /* Get erase block size in unit of sector (DWORD) */
  case GET_BLOCK_SIZE :
    *(DWORD*)buff = BLOCK_SIZE;
    break;
  
  default:
    res = RES_PARERR;
  }
  
  return res;
}
#endif /* _USE_IOCTL == 1 */
  
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/