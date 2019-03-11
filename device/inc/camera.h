#ifndef __CAMERA_H
#define __CAMERA_H

#include "stdint.h"



/************************************************************/
/************************************************************/
/*
since we use adv7180 that can provide next raw data stream ->
     Cb-Y-Cr-Y-Cb-Y-Cr-Y-Cb-Y-Cr-Y...
     |pix |
          |pix |
               |pix |
and we have 4 + 1 pclock cycles for 2 pixels, 6 + 1 for 3 and etc., and to calculate needed clock cycles per line we need to use next formula ->
(pixels_per_line / 2) * 4 + 1 = pixels_per_line * 2 + 1
*/

#define CAMERA_GET_CLOCKS_PER_LINE(width)               ( ((width) * 2) - (width == 0 ? 0 : 1) )
#define CAMERA_GET_LINES_PER_FRAME(height)              ((height) + 1)
#define CAMERA_GET_CLOCKS_PER_FRAME(width, height)      (CAMERA_GET_CLOCKS_PER_LINE(width) * CAMERA_GET_LINES_PER_FRAME(height))  

#define CAMERA_BYTES_PER_LINE(width)                    CAMERA_GET_CLOCKS_PER_LINE(width)                  
#define CAMERA_HWORDS_PER_LINE(width)                   (CAMERA_GET_CLOCKS_PER_LINE(width) / 2)
#define CAMERA_WORDS_PER_LINE(width)                    (CAMERA_GET_CLOCKS_PER_LINE(width) / 4)

#define CAMERA_LINES_PER_FRAME(height)                  CAMERA_GET_LINES_PER_FRAME(height)

#define CAMERA_BYTES_PER_FRAME(width, height)           CAMERA_GET_CLOCKS_PER_FRAME(width, height)
#define CAMERA_HWORDS_PER_FRAME(width, height)          (CAMERA_GET_CLOCKS_PER_FRAME(width, height) / 2)
#define CAMERA_WORDS_PER_FRAME(width, height)           (CAMERA_GET_CLOCKS_PER_FRAME(width, height) / 4)


#define CAMERA_MAX_FRAME_SIZE_BYTES                     (CAMERA_GET_CLOCKS_PER_LINE(CAMERA_MAX_LINE_WIDTH) * CAMERA_GET_LINES_PER_FRAME(CAMERA_MAX_FRAME_HEIGHT))
#define CAMERA_MAX_FRAME_SIZE_HWORDS                    (CAMERA_MAX_FRAME_SIZE_BYTES / 2)
#define CAMERA_MAX_FRAME_SIZE_WORDS                     (CAMERA_MAX_FRAME_SIZE_BYTES / 4)

/*
following implementation is based on the next progressive video standart -> ITU-R BT.656-4
embedded synchro codes
0xff 0x00 0x00 xy where xy is :
msb -> lsb
1 F V H P3 P2 P1 P0

F :
0 - during field 1
1 - during field 2

V : 
0 - elsewhere
1 - during field blanking

H : 
0 - in SAV (start active video)
1 - in EAV (end active video)



*/
#ifndef CAMERA_SYNCHRO_PROTECT

#define CAMERA_FSC_CODE (0xABU)/*frame start delimiter code*/
#define CAMERA_LSC_CODE (0x80U)/*line start delimiter code*/
#define CAMERA_LEC_CODE (0x9DU)/*line end delimiter code*/
#define CAMERA_FEC_CODE (0xB6U)/*frame end delimiter code*/

#define CAMERA_MASK_CODE (0x00U) /**/

#endif /*! CAMERA_SYNCHRO_PROTECT*/

typedef struct {
    uint16_t lineStart, lineWidth;
    uint16_t frameStart, frameHeight;
    uint32_t memory;
} CameraFrame;

int DCMIinit ();
void DCMIDeInit ();
void DCMIrestart ();
    
int DCMIsnapshotStart (CameraFrame frame);
int DCMIcontinuousStart (CameraFrame frame);

    
void DCMI_DMA_IrqHandler ();
void DCMIIrqHandler ();
bool DCMIisBusy ();



#endif /*__CAMERA_H*/



