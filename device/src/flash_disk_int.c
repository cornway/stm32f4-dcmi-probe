#include "flash_diskio.h"



enum ATML_FLASH_CMD {
    READ_ARRAY          = 0x0b,
    READ_ARR_SLOW       = 0x03,
    ERASE_4K            = 0x20,
    ERASE_32K           = 0x52,
    ERASE_64K           = 0xd8,
    ERASE_ALL           = 0x60,
    PROG_PAGE           = 0x02,
    SEQ_PROG            = 0xad,
    WREN                = 0x06,
    WRDIS               = 0x04,
    PROTECT_SECTOR      = 0x36,
    UNPROTECT_SECTOR    = 0x39,
    GLO_PROTECT_TOG     = 0x36,
    READ_PROTECT        = 0x3c,
    READ_STAT           = 0x05,
    WRITE_STAT          = 0x01,
    READ_ID             = 0x9f,
    POWER_DOWN          = 0xb9,
    RESUME              = 0xab,
};

enum FLASH_STAT_BP {
    BUSY = (1 << 0),
    WEL = (1 << 1),
    SWPL = (1 << 2),
    SWPH = (1 << 3),
    WPP = (1 << 4),
    EPE = (1 << 5),
    SPM = (1 << 6),
    SPRL = (1 << 7),
};

typedef union {
    uint32_t word;
    uint8_t byte[4];
} fl_adr_t;


extern int8_t STORAGE_Inquirydata[80];

typedef struct {
    uint8_t factory;
    uint8_t idl, idh;
    int8_t usr;
} at_info_t;

#define AT_ASSERT   (0U)
#define AT_DEASSERT (1U)
#define AT_NOP      (0U)
#define AT_ENABLE   (1U)
#define AT_DISABLE  (0U)

#define TX_ABORT(v) do { at_ll_sel(AT_DEASSERT); return v; } while (0)
#define TX_START()  do { at_ll_sel(AT_ASSERT); } while(0)

#include <string.h>

AT_STATIC AT_BYTE pinfo (AT_BYTE pos, const char *info)
{
    AT_BYTE strl = (AT_BYTE)strlen(info);
    memcpy(STORAGE_Inquirydata + pos, info, strl);
    STORAGE_Inquirydata[pos + strl] = ' ';
    return pos + strl + 1;
}

AT_STATIC AT_INLINE void at_cmd (AT_BYTE cmd)
{
	at_ll_sel(AT_ASSERT);
	at_ll_rw(cmd);
	at_ll_sel(AT_DEASSERT);
}

AT_STATIC AT_INLINE void at_wren (AT_BYTE x)
{
    if (x)
        at_cmd(WREN);
    else 
        at_cmd(WRDIS);
}

AT_STATIC AT_INLINE at_info_t at_read_info (void)
{
    at_info_t info;
    at_ll_sel(AT_ASSERT);
    at_ll_rw(READ_ID);
    info.factory =  at_ll_rw(AT_NOP);
    info.idl = at_ll_rw(AT_NOP);
    info.idh = at_ll_rw(AT_NOP);
    info.usr = at_ll_rw(AT_NOP);
    at_ll_sel(AT_DEASSERT);
    return info;
}

AT_STATIC AT_INLINE AT_BYTE at_read_status (void)
{
    AT_BYTE status;
    at_ll_sel(AT_ASSERT);
    at_ll_rw(READ_STAT);
    status = at_ll_rw(AT_NOP);
    at_ll_sel(AT_DEASSERT);
    return status;
}

AT_STATIC AT_INLINE void at_wait (void)
{
    at_read_status();
    while (at_read_status() & BUSY) {}
}

AT_STATIC AT_INLINE void at_write_status (AT_BYTE status)
{
    at_ll_sel(AT_ASSERT);
    at_ll_rw(WRITE_STAT);
    at_ll_rw(status);
    at_ll_sel(AT_DEASSERT);
}


static AT_WORD at_blknum, at_blk_size;

DSTATUS at_init (void)
{
    DSTATUS status = (DSTATUS)at_ll_init();
    at_info_t info;
    at_ll_wp(1);
    at_ll_hold(1);
    at_wren(AT_ENABLE);
    at_write_status(0);
    
    info = at_read_info();
    if (info.usr != 0) {
        TX_ABORT(STA_NODISK);
    }
    
    at_blknum = 0;
    at_blk_size = 0;
    
    switch (info.factory) {
        case 0x1f : 
                    pinfo(8, "Adesto");
            break;
        default :
                    pinfo(8, "unknown");
            break;
    }
    switch (info.idl & 0xe0) {
        case 0x40 : 
                    pinfo(16, "AT25/26DFxxx");
                    at_blk_size = AT_BLOCK_LEN;
            break;
        default :
                    pinfo(16, "unknown");
            break;
    }
    switch (info.idl & 0x1f) {
        case 0x04 : 
                    pinfo(32, "4M");
                    at_blknum = 0x7ffff / AT_BLOCK_LEN;
            break;
        default :
                    pinfo(32, "unknown");
            break;
    }
    uint8_t reg = at_read_status();
    if (
            (reg & SPM) ||
            (reg & EPE) ||
            (reg & BUSY)
        ) {
            status = STA_NOINIT;
        }
    TX_ABORT(status);
}

DRESULT at_read_blk (AT_WORD blk, BYTE *dest, AT_WORD blknum)
{
    AT_BYTE reg;
    DRESULT status = RES_OK;
    
    reg = at_read_status();
    if (
            (reg & SPM) ||
            (reg & EPE) ||
            (reg & BUSY)
        ) {
            TX_ABORT(RES_NOTRDY);
        }
    TX_START();
    #if (AT_READ_FAST == 1)
        at_ll_rw(READ_ARRAY);
    #else
        at_ll_rw(READ_ARR_SLOW);
    #endif
    fl_adr_t adr;    
    adr.word = blk * AT_BLOCK_LEN;
        
    at_ll_rw(adr.byte[2]); 
    at_ll_rw(adr.byte[1]); 
    at_ll_rw(adr.byte[0]); 
        
    #if (AT_READ_FAST == 1)
        at_ll_rw(AT_NOP);
    #endif
     
    AT_WORD length = blknum * AT_BLOCK_LEN;
    for (AT_WORD i = 0; i < length; i++) {
        reg = at_ll_rw(AT_NOP);
        *((AT_BYTE *)dest + i) = reg;
    }
    TX_ABORT(status);
}
DRESULT at_erase_blk (AT_WORD size, AT_WORD blk)
{
    DRESULT status = RES_OK;
    AT_BYTE reg;
    reg = at_read_status();
    if (reg & BUSY) {
        TX_ABORT(RES_NOTRDY);
    }
		
    AT_BYTE cmd;
    AT_WORD address;
    switch (size) {
        case ERASE_P4K :
                        cmd = ERASE_4K;
						address = blk * AT_BLOCK_LEN;	
            break;
        case ERASE_P32K :
						cmd = ERASE_32K;
						address = blk * AT_SECT32_LEN;
            break;
        case ERASE_P64K :
						cmd = ERASE_64K;
						address = blk * AT_SECT64_LEN;
            break;
        case ERASE_PALL :
						cmd = ERASE_ALL;
						address = 0;
            break;
				default : 	address = 0;
						break;
    };
		
	fl_adr_t adr;
	adr.word = address;
		
    at_wren(AT_ENABLE);
    TX_START();
    at_ll_rw(cmd); 
    at_ll_rw(adr.byte[2]); 
    at_ll_rw(adr.byte[1]); 
    at_ll_rw(adr.byte[0]);
    at_wait();
    reg = at_read_status();
    if (reg & EPE) {
        status = RES_ERROR;
    }
    TX_ABORT(status);
}

#define AT_SECT4_BLK (1U)

#define AT_ERASE(size, blk, blknum) \
do { \
     status = at_erase_blk(ERASE_P##size##K, blk); \
     blk += AT_SECT##size##_BLK; \
     blknum -= AT_SECT##size##_BLK; \
} while (0)

#define AT_ERASE_LOOP(size, blk, blknum, cond) \
do { \
     for (uint32_t i = 0; i < cond; i++) { \
         status = at_erase_blk(ERASE_P##size##K, blk); \
         blk += AT_SECT##size##_BLK; \
         blknum -= AT_SECT##size##_BLK; \
     } \
} while (0)

DRESULT at_prog_blk (AT_WORD blk, const AT_BYTE *src, AT_WORD blknum)
{
    DRESULT status = RES_OK;
    AT_BYTE reg;
    
	
    reg = at_read_status();
    if (reg & BUSY) {
        TX_ABORT(RES_NOTRDY);
    }
#if (AT_ERASE_FAST == 1)	
    uint32_t block;
    uint32_t blk__ = blk;
    uint32_t blknum__ = blknum;
    
    if (blknum < AT_SECT32_BLK) {
        AT_ERASE_LOOP(4, blk__, blknum__, block);
    } else if (blknum < AT_SECT64_BLK) {
        block = blk__ & (AT_SECT32_BLK - 1);
        
        if (block == 0) {
            AT_ERASE(32, blk__, blknum__);
            AT_ERASE_LOOP(4, blk__, blknum__, block);
        } else {
            AT_ERASE_LOOP(4, blk__, blknum__, block);
            AT_ERASE(32, blk__, blknum__);
        }
    } else {
        block = (blk__ & (AT_SECT32_BLK - 1));
        AT_ERASE_LOOP(4, blk__, blknum__, block);
        block = (blk__ & (AT_SECT64_BLK - 1)) / AT_SECT32_BLK;
        AT_ERASE_LOOP(32, blk__, blknum__, block);
        block = blknum__ / AT_SECT64_BLK;
        AT_ERASE_LOOP(64, blk__, blknum__, block);
        block = blknum__ / AT_SECT32_BLK;
        AT_ERASE_LOOP(32, blk__, blknum__, block);
        block = blknum__;
        AT_ERASE_LOOP(4, blk__, blknum__, block);
    }
    
    block = blk % AT_SECT32_BLK;
    if (block) {
        AT_ERASE_LOOP(4, blk__, blknum__, block);
    }
    block = blk % AT_SECT64_BLK;
    if (block) {
        AT_ERASE(32, blk__, blknum__);
    }
    block = blknum / AT_SECT64_BLK;
    AT_ERASE_LOOP(64, blk__, blknum__, block);
    if (blknum >= AT_SECT32_BLK) {
        AT_ERASE(32, blk__, blknum__);
    }
    block = blknum;
    AT_ERASE_LOOP(4, blk__, blknum__, block);
    
#else
    for (AT_WORD i = 0; i < blknum; i++) {
        status = at_erase_blk(ERASE_P4K, blk + i);
    }
#endif
	if (status != RES_OK) {
        TX_ABORT(status);
    }	
    fl_adr_t adr;

    AT_WORD prog_pages = blknum * (AT_BLOCK_LEN / AT_PAGE_LEN);
    AT_WORD src_adr = (AT_WORD)src;
    AT_WORD dest_adr = blk * AT_BLOCK_LEN;
    for (AT_WORD page = 0; page < prog_pages; page++) {
        adr.word = dest_adr;
        
        at_wren(AT_ENABLE);
        
        TX_START();
        at_ll_rw(PROG_PAGE); 
        at_ll_rw(adr.byte[2]); 
        at_ll_rw(adr.byte[1]); 
        at_ll_rw(adr.byte[0]);
        for (AT_WORD i = 0; i < AT_PAGE_LEN; i++) {
            reg = *((uint8_t *)src_adr + i);
            at_ll_rw(reg);
        }
        at_ll_sel(AT_DEASSERT);
        at_wait(); 
        dest_adr += AT_PAGE_LEN;
        src_adr += AT_PAGE_LEN;
    }
    
    reg = at_read_status();
    if (reg & EPE) {
        status = RES_ERROR;
    }
    TX_ABORT(status);
}
DRESULT at_sleep (void)
{
    at_cmd(POWER_DOWN);
    return RES_OK;
}
DRESULT at_resume (void)
{
	at_cmd(RESUME);
    return RES_OK;
}
DSTATUS at_get_stat (void)
{
    DSTATUS status = 0;
    AT_BYTE reg;
    reg = at_read_status();
    if (
            (reg & SPM) ||
            (reg & EPE) ||
            (reg & BUSY)
        ) {
            TX_ABORT(STA_PROTECT);
        }
    TX_ABORT(status);
}

