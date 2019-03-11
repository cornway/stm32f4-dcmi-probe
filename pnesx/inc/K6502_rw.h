/*===================================================================*/
/*                                                                   */
/*  K6502_RW.h : 6502 Reading/Writing Operation for NES              */
/*               This file is included in K6502.cpp                  */
/*                                                                   */
/*  1999/11/03  Racoon  New preparation                              */
/*                                                                   */
/*===================================================================*/

#ifndef K6502_RW_H_INCLUDED
#define K6502_RW_H_INCLUDED

/*-------------------------------------------------------------------*/
/*  Include files                                                    */
/*-------------------------------------------------------------------*/

#include "pNesX.h"
#include "pNesX_System.h"

/*===================================================================*/
/*                                                                   */
/*            K6502_ReadZp() : Reading from the zero page            */
/*                                                                   */
/*===================================================================*/
static inline BYTE K6502_ReadZp( BYTE byAddr )
{
/*
 *  Reading from the zero page
 *
 *  Parameters
 *    BYTE byAddr              (Read)
 *      An address inside the zero page
 *
 *  Return values
 *    Read Data
 */

  return RAM[ byAddr ];
}

/*===================================================================*/
/*                                                                   */
/*               K6502_Read() : Reading operation                    */
/*                                                                   */
/*===================================================================*/
static inline BYTE K6502_Read( WORD wAddr )
{
/*
 *  Reading operation
 *
 *  Parameters
 *    WORD wAddr              (Read)
 *      Address to read
 *
 *  Return values
 *    Read data
 *
 *  Remarks
 *    0x0000 - 0x1fff  RAM ( 0x800 - 0x1fff is mirror of 0x0 - 0x7ff )
 *    0x2000 - 0x3fff  PPU
 *    0x4000 - 0x5fff  Sound
 *    0x6000 - 0x7fff  SRAM ( Battery Backed )
 *    0x8000 - 0xffff  ROM
 *
 */
  BYTE byRet;

  switch ( wAddr & 0xe000 )
  {
    case 0x0000:  /* RAM */
      return RAM[ wAddr & 0x7ff ];

    case 0x2000:  /* PPU */
      if ( wAddr <= 0x2006 )  /* PPU Status */
      {
        // Set return value
        byRet = PPU_R2;

        // Reset a V-Blank flag
        PPU_R2 &= ~R2_IN_VBLANK;

        // Reset address latch
        PPU_Latch_Flag = 0;

        // Make a Nametable 0 in V-Blank
        if ( PPU_Scanline >= SCAN_VBLANK_START && !( PPU_R0 & R0_NMI_VB ) )
        {
          PPU_R0 &= ~R0_NAME_ADDR;
          PPU_NameTableBank = NAME_TABLE0;
        }
        return byRet;
      }
      else
      if ( wAddr == 0x2007 )  /* PPU Memory */
      {
        // Set return value;
        byRet = PPU_R7;

        // Read PPU Memory
        PPU_R7 = PPUBANK[ PPU_Addr >> 10 ][ PPU_Addr & 0x3ff ];

        // Increment PPU Address
        PPU_Addr += PPU_Increment;
        PPU_Addr &= 0x3fff;

        return byRet;
      }
      break;

    case 0x4000:  /* Sound */
      if ( wAddr < 0x4016 )
      {
        // Return APU Register
        //return APU_Reg[ wAddr & 0x1f ];
        return ApuRead( wAddr & 0x1f );
      }
      else
      if ( wAddr == 0x4016 )
      {
        // Set Joypad1 data
        byRet = (BYTE)( ( PAD1_Latch >> PAD1_Bit ) & 1 ) | 0x40;
        PAD1_Bit = ( PAD1_Bit == 23 ) ? 0 : ( PAD1_Bit + 1 );
        return byRet;
      }
      else
      if ( wAddr == 0x4017 )
      {
        // Set Joypad2 data
        byRet = (BYTE)( ( PAD2_Latch >> PAD2_Bit ) & 1 ) | 0x40;
        PAD2_Bit = ( PAD2_Bit == 23 ) ? 0 : ( PAD2_Bit + 1 );
        return byRet;
      }
      break;

    case 0x6000:  /* SRAM */
      return SRAM[ wAddr & 0x1fff ];

    case 0x8000:  /* ROM BANK 0 */
      return ROMBANK0[ wAddr & 0x1fff ];

    case 0xa000:  /* ROM BANK 1 */
      return ROMBANK1[ wAddr & 0x1fff ];

    case 0xc000:  /* ROM BANK 2 */
      return ROMBANK2[ wAddr & 0x1fff ];

    case 0xe000:  /* ROM BANK 3 */
      return ROMBANK3[ wAddr & 0x1fff ];
  }

  return 0;
}

/*===================================================================*/
/*                                                                   */
/*               K6502_Write() : Writing operation                    */
/*                                                                   */
/*===================================================================*/
static inline void K6502_Write( WORD wAddr, BYTE byData )
{
/*
 *  Writing operation
 *
 *  Parameters
 *    WORD wAddr              (Read)
 *      Address to write
 *
 *    BYTE byData             (Read)
 *      Data to write
 *
 *  Remarks
 *    0x0000 - 0x1fff  RAM ( 0x800 - 0x1fff is mirror of 0x0 - 0x7ff )
 *    0x2000 - 0x3fff  PPU
 *    0x4000 - 0x5fff  Sound
 *    0x6000 - 0x7fff  SRAM ( Battery Backed )
 *    0x8000 - 0xffff  ROM
 *
 */

  switch ( wAddr & 0xe000 )
  {
    case 0x0000:  /* RAM */
      RAM[ wAddr & 0x7ff ] = byData;
      break;

    case 0x2000:  /* PPU */
      switch ( wAddr & 0x7 )
      {
        case 0:    /* 0x2000 */
          PPU_R0 = byData;
          PPU_Increment = ( PPU_R0 & R0_INC_ADDR ) ? 32 : 1;
          PPU_NameTableBank = NAME_TABLE0 + ( PPU_R0 & R0_NAME_ADDR );
          PPU_BG_Base = ( PPU_R0 & R0_BG_ADDR ) ? ChrBuf + 256 * 64 : ChrBuf;
          PPU_SP_Base = ( PPU_R0 & R0_SP_ADDR ) ? ChrBuf + 256 * 64 : ChrBuf;
          PPU_SP_Height = ( PPU_R0 & R0_SP_SIZE ) ? 16 : 8;
          break;

        case 1:   /* 0x2001 */
          PPU_R1 = byData;
          break;

        case 2:   /* 0x2002 */
          PPU_R2 = byData;
          break;

        case 3:   /* 0x2003 */
          // Sprite RAM Address
          PPU_R3 = byData;
          break;

        case 4:   /* 0x2004 */
          // Write data to Sprite RAM
          SPRRAM[ PPU_R3++ ] = byData;
          break;

        case 5:   /* 0x2005 */
          // Set Scroll Register
          if ( PPU_Latch_Flag )
          {
            // V-Scroll Register
            PPU_Scr_V_Next = ( byData > 239 ) ? 0 : byData;
            PPU_Scr_V_Byte_Next = PPU_Scr_V_Next >> 3;
            PPU_Scr_V_Bit_Next = PPU_Scr_V_Next & 7;
          }
          else
          {
            // H-Scroll Register
            PPU_Scr_H_Next = byData;
            PPU_Scr_H_Byte_Next = PPU_Scr_H_Next >> 3;
            PPU_Scr_H_Bit_Next = PPU_Scr_H_Next & 7;
          }
          PPU_Latch_Flag ^= 1;
          break;

        case 6:   /* 0x2006 */
          // Set PPU Address
          if ( PPU_Latch_Flag )
          {
            // Low
            PPU_Addr |= byData;
          }
          else
          {
            // High
            PPU_Addr = ( byData & 0x3f ) << 8;
          }
          PPU_Latch_Flag ^= 1;
          break;

        case 7:   /* 0x2007 */
          // Write to PPU Memory
          if ( PPU_Addr < 0x2000 && NesHeader.byVRomSize == 0 )
          {
            // Pattern Data
            ChrBufUpdate |= ( 1 << ( PPU_Addr >> 10 ) );
            PPURAM[ PPU_Addr ] = byData;
          }
          else
          if ( PPU_Addr < 0x3f00 )  /* 0x2000 - 0x3eff */
          {
            // Name Table
            PPUBANK[ PPU_Addr >> 10 ][ PPU_Addr & 0x3ff ] = byData;
          }
          else
          if ( !( PPU_Addr & 0xf ) )  /* 0x3f00 or 0x3f10 */
          {
            // Palette mirror
            PPURAM[ 0x3f10 ] = PPURAM[ 0x3f14 ] = PPURAM[ 0x3f18 ] = PPURAM[ 0x3f1c ] = 
            PPURAM[ 0x3f00 ] = PPURAM[ 0x3f04 ] = PPURAM[ 0x3f08 ] = PPURAM[ 0x3f0c ] = byData;
            PalTable[ 0x00 ] = PalTable[ 0x04 ] = PalTable[ 0x08 ] = PalTable[ 0x0c ] =
            PalTable[ 0x10 ] = PalTable[ 0x14 ] = PalTable[ 0x18 ] = PalTable[ 0x1c ] = NesPalette[ byData ]; // | 0x8000;
          }
          else
          if ( PPU_Addr & 3 )
          {
            // Palette
            PPURAM[ PPU_Addr ] = byData;
            PalTable[ PPU_Addr & 0x1f ] = NesPalette[ byData ];
          }

          // Increment PPU Address
          PPU_Addr += PPU_Increment;
          PPU_Addr &= 0x3fff;
          break;
      }
      break;

    case 0x4000:  /* Sound */
      switch ( wAddr & 0x1f )
      {
        case 0x14:  /* 0x4014 */
          // Sprite DMA
          switch ( byData >> 5 )
          {
            case 0x0:  /* RAM */
              pNesX_MemoryCopy( SPRRAM, &RAM[ ( (WORD)byData << 8 ) & 0x7ff ], SPRRAM_SIZE );
              break;

            case 0x3:  /* SRAM */
              pNesX_MemoryCopy( SPRRAM, &SRAM[ ( (WORD)byData << 8 ) & 0x1fff ], SPRRAM_SIZE );
              break;

            case 0x4:  /* ROM BANK 0 */
              pNesX_MemoryCopy( SPRRAM, &ROMBANK0[ ( (WORD)byData << 8 ) & 0x1fff ], SPRRAM_SIZE );
              break;

            case 0x5:  /* ROM BANK 1 */
              pNesX_MemoryCopy( SPRRAM, &ROMBANK1[ ( (WORD)byData << 8 ) & 0x1fff ], SPRRAM_SIZE );
              break;

            case 0x6:  /* ROM BANK 2 */
              pNesX_MemoryCopy( SPRRAM, &ROMBANK2[ ( (WORD)byData << 8 ) & 0x1fff ], SPRRAM_SIZE );
              break;

            case 0x7:  /* ROM BANK 3 */
              pNesX_MemoryCopy( SPRRAM, &ROMBANK3[ ( (WORD)byData << 8 ) & 0x1fff ], SPRRAM_SIZE );
              break;
          }
          APU_Reg[ 0x14 ] = byData;          
          break;

        case 0x16:  /* 0x4016 */
          // Reset joypad
          if ( !( APU_Reg[ 0x16 ] & 1 ) && ( byData & 1 ) )
          {
            PAD1_Bit = 0;
            PAD2_Bit = 0;
          }
          APU_Reg[ 0x16 ] = byData;
          break;
          
        default:
          if ( wAddr <= 0x4017 )
          {
            // Write to APU Register
            ApuWrite( wAddr & 0x1f, byData );
          }
          break;
      }
      break;

    case 0x6000:  /* SRAM */
      SRAM[ wAddr & 0x1fff ] = byData;
      break;

    case 0x8000:  /* ROM BANK 0 */
    case 0xa000:  /* ROM BANK 1 */
    case 0xc000:  /* ROM BANK 2 */
    case 0xe000:  /* ROM BANK 3 */
      // Write to Mapper
      MapperWrite( wAddr, byData );
      break;
  }
}

// Reading/Writing operation (WORD version)
static inline WORD K6502_ReadW( WORD wAddr ){ return K6502_Read( wAddr ) | (WORD)K6502_Read( wAddr + 1 ) << 8; };
static inline void K6502_WriteW( WORD wAddr, WORD wData ){ K6502_Write( wAddr, wData & 0xff ); K6502_Write( wAddr + 1, wData >> 8 ); };
static inline WORD K6502_ReadZpW( BYTE byAddr ){ return K6502_ReadZp( byAddr ) | ( K6502_ReadZp( byAddr + 1 ) << 8 ); };

#endif /* !K6502_RW_H_INCLUDED */


