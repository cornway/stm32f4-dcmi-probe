/*===================================================================*/
/*                                                                   */
/*  pspad.cpp : PS Pad function                                      */
/*                                                                   */
/*  2016/1/20  Racoon                                                */
/*                                                                   */
/*===================================================================*/

#include "mbed.h"
#include "pspad.h"

// SPI interface
//SPI pad_spi(PC_12, PC_11, PC_10);   // MOSI(should pullup), MISO, SCK
//DigitalOut pad1_cs(PD_2);

// PS pad initialize
void pspad_init()
{
    /*
    pad_spi.format(8, 3);
    pad_spi.frequency(250000);
    */
}

// Read PS Pad state
// data1  SE -- -- ST U  R D L
// data2  L2 R2 L1 R1 TR O X SQ
void pspad_read(unsigned short *pad1, unsigned short *pad2)
{
    /*
    pad1_cs = 0;
    wait_us(500);

    pad_spi.write(0x80);
    pad_spi.write(0x42);
    pad_spi.write(0);
    pad_spi.write(0);
    int d1 = pad_spi.write(0);
    int d2 = pad_spi.write(0);

    pad1_cs = 1;
    
    *pad1 = (char)~d1 << 8 | (char)~d2;
    
    *pad2 = 0;
    */
}

