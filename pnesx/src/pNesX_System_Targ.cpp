/*===================================================================*/
/*                                                                   */
/*  pNesX_System_Nucleo.cpp : The function which depends on a system */
/*                         (for Nucleo F446RE)                       */
/*                                                                   */
/*  2016/1/20  Racoon                                                */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Include files                                                    */
/*-------------------------------------------------------------------*/



//#define PS_GAMEPAD

#include "ff.h"
#include "pNesX.h"
#include "pNesX_System.h"
#include <stdint.h>
#include <string.h>
#include "device_conf.h"

extern int pNesX_Filer();
extern void ApuMute(bool mute);


extern WORD *pDrawData;

//#define SHOW_FPS

#if defined(SHOW_FPS)
Timer timer;
int fps = 0;
int dmawait;
#endif

/*-------------------------------------------------------------------*/
/*  ROM image file information                                       */
/*-------------------------------------------------------------------*/

extern char szRomFolder[];
extern char szRomFilename[];
extern char szRomPath[];
    
char szRomName[ 256 ];
char szSaveName[ 256 ];
int nSRAM_SaveFlag;

// Palette data
const WORD NesPalette[ 64 ] =
{
    0x73ae, 0x20d1, 0x0015, 0x4013, 0x880e, 0xa802, 0xa000, 0x7840,
	0x4160, 0x0220, 0x0280, 0x01e2, 0x19eb, 0x0000, 0x0000, 0x0000,
	0xbdf7, 0x039d, 0x21dd, 0x801e, 0xb817, 0xe00b, 0xd940, 0xca61,
	0x8b80, 0x04a0, 0x0540, 0x0487, 0x0411, 0x0000, 0x0000, 0x0000,
	0xffff, 0x3dff, 0x5cbf, 0xa45f, 0xf3df, 0xfbb6, 0xfbac, 0xfcc7,
	0xf5e7, 0x8682, 0x4ee9, 0x5fd3, 0x075b, 0x0000, 0x0000, 0x0000,
	0xffff, 0xaf3f, 0xc6bf, 0xd65f, 0xfe3f, 0xfe3b, 0xfdf6, 0xfed5,
	0xff34, 0xe7f4, 0xaf97, 0xb7f9, 0x9ffe, 0x0000, 0x0000, 0x0000,
};

/*-------------------------------------------------------------------*/
/*  Function prototypes                                              */
/*-------------------------------------------------------------------*/

int LoadSRAM();
int SaveSRAM();

/*===================================================================*/
/*                                                                   */
/*           LoadSRAM() : Load a SRAM                                */
/*                                                                   */
/*===================================================================*/
int LoadSRAM()
{
/*
 *  Load a SRAM
 *
 *  Return values
 *     0 : Normally
 *    -1 : SRAM data couldn't be read
 */

  return 0;
}

/*===================================================================*/
/*                                                                   */
/*           SaveSRAM() : Save a SRAM                                */
/*                                                                   */
/*===================================================================*/
int SaveSRAM()
{
/*
 *  Save a SRAM
 *
 *  Return values
 *     0 : Normally
 *    -1 : SRAM data couldn't be written
 */


  // Successful
  return 0;
}

/*===================================================================*/
/*                                                                   */
/*                  pNesX_Menu() : Menu screen                       */
/*                                                                   */
/*===================================================================*/
int pNesX_Menu()
{
/*
 *  Menu screen
 *
 *  Return values
 *     0 : Normally
 *    -1 : Exit pNesX
 */

#if defined(SHOW_FPS)
    timer.stop();
#endif

    ApuMute(true);

    switch (pNesX_Filer())
    {
        case 0:  // Selected a file
            if ( pNesX_Load( szRomPath ) == 0 )
            {
                // Set a ROM image name
                strcpy( szRomName, szRomFilename );
        
                // Load SRAM
                LoadSRAM();
            }
            break;
            
        case 1:  // Return to Emu
            break;
            
        case 2:  // Reset
            if ( szRomName[ 0 ] != 0 )
            {
              // Reset pNesX
              pNesX_Reset();
            }                
        
            break;
            
        case -1:  // Error
            //printf("Filer Error\r\n");
            while(1);
    }

    //tft_clear(TFT_BLACK);
    //tft_set_window(32, 8, NES_DISP_WIDTH + 32 - 1, NES_DISP_HEIGHT + 8 - 1);        
    
    ApuMute(false);

#if defined(SHOW_FPS)
    timer.start();  
#endif

  return 0;
}

/*===================================================================*/
/*                                                                   */
/*               pNesX_ReadRom() : Read ROM image file               */
/*                                                                   */
/*===================================================================*/

uint8_t pad_states[4][4];
//BYTE vrom[0x4000];

int pNesX_ReadRom( const char *pszFileName )
{
/*
 *  Read ROM image file
 *
 *  Parameters
 *    const char *pszFileName          (Read)
 *
 *  Return values
 *     0 : Normally
 *    -1 : Error
 */
    
    /*
        init pad states array
        since we have touch sensor against usb keypad,
        which was presented in current version of pnesx
    */
    
    pad_states[0][0] = 0;
    pad_states[1][0] = 0;
    pad_states[2][0] = 0;
    pad_states[3][0] = 0;
    pad_states[0][1] = 0;
    pad_states[0][2] = 0;
    pad_states[0][3] = 0;
    
    pad_states[1][1] = 1;
    pad_states[2][1] = 5;
    pad_states[3][1] = 2;
    pad_states[1][2] = 7;
    pad_states[2][2] = 0;
    pad_states[3][2] = 8;
    pad_states[1][3] = 3;
    pad_states[2][3] = 6;
    pad_states[3][3] = 4;
    
    FIL *file;
    FRESULT res;
    uint32_t f_bytes_read;
    
    file = new FIL;
    if (file == nullptr) {
        return -1;
    }
    
    /* Open ROM file */
    res = f_open(file, (TCHAR *)pszFileName, FA_READ);
    if ( res != FR_OK )
    {
        //printf("open error\r\n");
        delete file;
        return -1;
    }
    
    /* Read ROM Header */
    res = f_read(file, &NesHeader, sizeof NesHeader, &f_bytes_read);
    if ( memcmp( NesHeader.byID, "NES\x1a", 4 ) != 0 )
    {
        /* not .nes file */
        f_close(file);
        delete file;
        return -1;
    }
    
    LineData[0] = new WORD[NES_DISP_WIDTH * 2];
    if (LineData[0] == nullptr) {
        f_close(file);
        delete file;
        return -1;
    }
    LineData[1] = LineData[0] + NES_DISP_WIDTH;
    
    
    RAM = new BYTE[RAM_SIZE];
    if (RAM == nullptr) {
        f_close(file);
        delete file;
        return -1;
    }

    SRAM = new BYTE[SRAM_SIZE];
    if (SRAM == nullptr) {
        delete RAM;
        f_close(file);
        delete file;
        return -1;
    }
    
    /* Clear SRAM */
    memset( SRAM, 0, SRAM_SIZE );

    /* If trainer presents Read Triner at 0x7000-0x71ff */
    if ( NesHeader.byInfo1 & 4 )
    {
        //printf("Read Trainer\r\n");
        res = f_read(file, &SRAM[ 0x1000 ], 512, &f_bytes_read);
    }

    //printf("RomSize: %d\r\n", NesHeader.byRomSize);
    /* Allocate Memory for ROM Image */
    //ROM = (BYTE *)new char( NesHeader.byRomSize * 0x4000 );
    //printf("ROM addr:%x\r\n", ROM);

    /* Read ROM Image */
    
    ROM = new BYTE[0x4000 * NesHeader.byRomSize];
    if (ROM == nullptr) {
        delete SRAM;
        delete RAM;
        f_close(file);
        delete file;
        return -1;
    }
    
    res = f_read(file, ROM, 0x4000 * NesHeader.byRomSize, &f_bytes_read);
    if ( NesHeader.byVRomSize > 0 )
    {
        /* Allocate Memory for VROM Image */
        /*
        VROM = new BYTE[ NesHeader.byVRomSize * 0x2000 ];
        if (VROM == nullptr) {
            delete SRAM;
            delete RAM;
            delete ROM;
            f_close(file);
            delete file;
            return -1;
        }  
        */  
        if (NesHeader.byVRomSize > 2) {
            delete SRAM;
            delete RAM;
            delete ROM;
            f_close(file);
            delete file;
            return -1;
        } 
        VROM = (BYTE *)(NES_VRAM_START + NES_DISP_HEIGHT * NES_DISP_WIDTH * sizeof(WORD));        
        //printf("VROM addr:%x\r\n", VROM);

        /* Read VROM Image */
        res = f_read(file, VROM, 0x2000 * NesHeader.byVRomSize, &f_bytes_read);
    }

    /* File close */
    f_close(file);
    delete file;
    
    /* Successful */
    return 0;
}

/*===================================================================*/
/*                                                                   */
/*           pNesX_ReleaseRom() : Release a memory for ROM           */
/*                                                                   */
/*===================================================================*/
void pNesX_ReleaseRom()
{
/*
 *  Release a memory for ROM
 *
 */
    
  if ( LineData[0] )
  {
    delete[] LineData[0] ;
    LineData[0] = NULL;
  }
    
  if ( RAM )
  {
    delete[] RAM ;
    RAM = NULL;
  }
  
  if ( ROM )
  {
    delete[] ROM ;
    ROM = NULL;
  }
  
  if ( SRAM )
  {
    delete[] SRAM ;
    SRAM = NULL;
  }
  /*
  if ( VROM )
  {
    delete( VROM );
    VROM = NULL;
  }
  */
}

/*===================================================================*/
/*                                                                   */
/*      pNesX_LoadFrame() :                                          */
/*           Transfer the contents of work frame on the screen       */
/*                                                                   */
/*===================================================================*/
#include "fgpu.h"
#include "touch.h"
void pNesX_LoadFrame()
{
/*
 *  Transfer the contents of work frame on the screen
 *
 */
  
#if defined(SHOW_FPS)
    fps++;
    
    if (timer.read_ms() >= 1000)
    {
        timer.stop();
        printf("%d %d %d\r\n", fps, timer.read_ms(), dmawait);
        fps = 0;
        timer.reset();
        timer.start();
    }
    dmawait = 0;
#endif

    nes_frame_update();  
}


#include "main.h"

static uint32_t line_count = 0;
static uint32_t offset_count = NES_DISP_HEIGHT * NES_DISP_WIDTH * sizeof(WORD);
extern DMA_HandleTypeDef dma222comHandle;
void pNesX_TransmitLinedata()
{
  /*
  uint16_t *dest = (uint16_t *)NES_VRAM_START;
  uint16_t *src = (uint16_t *)pDrawData;
  for (uint32_t i = 0, d = offset_count; i < 256; i++, d++) {
      dest[d] = src[i];
  }
  */
  HAL_DMA_Start_IT(&dma222comHandle, (uint32_t)pDrawData, NES_VRAM_START + offset_count, NES_DISP_WIDTH);
  if (++line_count >= NES_DISP_HEIGHT) {
      line_count = 0;
      offset_count = NES_DISP_HEIGHT * NES_DISP_WIDTH * sizeof(WORD);
  } else {
      offset_count -= NES_DISP_WIDTH * sizeof(WORD);
  }
  /*
  while (SpiHandle.State != HAL_SPI_STATE_READY)
  {
#if defined(SHOW_FPS)
    dmawait++;
#endif
  }
  HAL_SPI_Transmit_DMA(&SpiHandle, (uint8_t *)pDrawData, 256 * 2);
  */
}

#if !defined(PS_GAMEPAD)
const BYTE UsbPadTable[] = {0x10, 0x90, 0x80, 0xa0, 0x20, 0x60, 0x40, 0x50}; 
#endif

/*===================================================================*/
/*                                                                   */
/*             pNesX_PadState() : Get a joypad state                 */
/*                                                                   */
/*===================================================================*/
#include "tsc2046.h"
extern TouchPointTypeDef touch_point;
void pNesX_PadState( DWORD *pdwPad1, DWORD *pdwPad2, DWORD *pdwSystem )
{
    uint16_t f, h;
    uint16_t x = touch_point.x, y = touch_point.y;
    if ((x < 480) && (x > 320)) {
        f = 3;
    } else if ((x < 320) && (x > 160)) {
        f = 2;
    } else if ((x < 160) && (x > 0)) {
        f = 1;
    } else {
        f = 0;
    }

    
    if ((y < 120) && (y > 0)) {
            h = 3;
    } else if ((y < 220) && (y > 120)) {
            h = 2;
    } else if ((y < 320) && (y > 220)) {
            h = 1;
    } else {
        h = 0;
    }
    
    // R L D U St Se B A 
 
    // SE -- -- ST U R D L       L2 R2 L1 R1 TR O X SQ
    
    unsigned short pad1;
    pad1 = pad_states[f][h] ? 1 << (pad_states[f][h] - 1) : 0;
    *pdwPad1 = pad1;
                ((pad1 & 0x0400) >> 3) | /*r*/
               ((pad1 & 0x0100) >> 2) | /*l*/
               ((pad1 & 0x0200) >> 4) | /*d*/
               ((pad1 & 0x0800) >> 7) | /*st*/
               ((pad1 & 0x1000) >> 9) | /*u*/
               ((pad1 & 0x8000) >> 13) | /*se*/
               ((pad1 & 1) << 1) | /*b*/
               ((pad1 & 2) >> 1); /*a*/

    
    *pdwSystem = ((pad1 & 0x80) >> 7) |
                 (((pad1 & 0x50) == 0x50) << 1);
    
    touch_point.x = 0;
    touch_point.y = 0;
/*
    R L D U St Se B A 
    *pdwSystem = ((pad1 & 0x80) >> 7) |
                 (((pad1 & 0x50) == 0x50) << 1);
    
#endif
    */
}

/*===================================================================*/
/*                                                                   */
/*             pNesX_MemoryCopy() : memcpy                           */
/*                                                                   */
/*===================================================================*/
void *pNesX_MemoryCopy( void *dest, const void *src, int count )
{
/*
 *  memcpy
 *
 *  Parameters
 *    void *dest                       (Write)
 *      Points to the starting address of the copied block to destination
 *
 *    const void *src                  (Read)
 *      Points to the starting address of the block of memory to copy
 *
 *    int count                        (Read)
 *      Specifies the size, in bytes, of the block of memory to copy
 *
 *  Return values
 *    Pointer of destination
 */

  memcpy(dest, src, count );
  return dest;
}

/*===================================================================*/
/*                                                                   */
/*             pNesX_MemorySet() :                                   */
/*                                                                   */
/*===================================================================*/
void *pNesX_MemorySet( void *dest, int c, int count )
{
/*
 *  memset
 *
 *  Parameters
 *    void *dest                       (Write)
 *      Points to the starting address of the block of memory to fill
 *
 *    int c                            (Read)
 *      Specifies the byte value with which to fill the memory block
 *
 *    int count                        (Read)
 *      Specifies the size, in bytes, of the block of memory to fill
 *
 *  Return values
 *    Pointer of destination
 */

  memset(dest, c, count); 
  return dest;
}

/*===================================================================*/
/*                                                                   */
/*                DebugPrint() : Print debug message                 */
/*                                                                   */
/*===================================================================*/
void pNesX_DebugPrint( char *pszMsg )
{
}

/*===================================================================*/
/*                                                                   */
/*                pNesX Initialise                                   */
/*                                                                   */
/*===================================================================*/
/*
int main()
{


    // Apu Initialize
    ApuInit();

    pNesX_Main();    
}
*/


