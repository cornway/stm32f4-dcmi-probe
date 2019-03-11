#ifndef FLASH_IO__H
#define FLASH_IO__H

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>
#include "diskio.h"
     
#define AT_BYTE    uint8_t
#define AT_WORD    uint32_t     
     
#define AT_READ_FAST        (1U)
#define AT_ERASE_FAST       (0U)
     
#define AT_INLINE   inline
#define AT_STATIC   static

#define AT_PAGE_LEN (0x100U)
	 
#define AT_BLOCK_LEN (0x1000U)
     
#define AT_SECT64_LEN   (0x10000U)
#define AT_SECT32_LEN   (0x8000U)

#define AT_SECT64_BLK   (AT_SECT64_LEN / AT_BLOCK_LEN)
#define AT_SECT32_BLK   (AT_SECT32_LEN / AT_BLOCK_LEN)

enum FLASH_DISK_STAT {
    FLASH_OK,
    FLASH_ABRT = -1,
};

enum FLASH_ERASE_SIZE {
  ERASE_P4K,
  ERASE_P32K,
  ERASE_P64K,
  ERASE_PALL,
};

DSTATUS at_init (void);
DSTATUS at_get_stat (void);

DRESULT at_read_blk (AT_WORD blk, AT_BYTE *dest, AT_WORD blknum);
DRESULT at_erase_blk (AT_WORD size, AT_WORD blknum);
DRESULT at_prog_blk (AT_WORD blk, const AT_BYTE *src, AT_WORD blknum);
DRESULT at_sleep (void);
DRESULT at_resume (void);


AT_BYTE at_ll_init (void);
void at_ll_sel (AT_BYTE x);
void at_ll_wp (AT_BYTE x);
void at_ll_hold (AT_BYTE x);
AT_BYTE at_ll_rw (AT_BYTE write_data);

#ifdef __cplusplus
    }
#endif

#endif /*FLASH_IO__H*/
    
