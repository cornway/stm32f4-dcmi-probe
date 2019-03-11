
#ifndef __TFT
#define __TFT


/*============================================================================*/
/*                               ***ILI9488***                                */
/*                        TFT LCD Single Chip Driver                          */
/*                        Command List Specifications                         */
/*============================================================================*/

/*Note: Following text and descriptions are partially copied from the original*/
/*product datasheet and for detail information  please refer to the manufacrer*/
/*Web-Site.                                                                   */
/*===========================Stansart Command list============================*/

enum LcdCmdEnumDef {
    LcdCmdNop       = 0x00,
    LcdCmdSWRESET   = 0x01,
    LcdCmdRDDIDIF   = 0x04,
    LcdCmdRDNUMED   = 0x05,
    LcdCmdRDDST     = 0x09,
    LcdCmdRDDPM     = 0x0A,
    LcdCmdRDDMADCTL = 0x0B,
    LcdCmdRDDCOLMOD = 0x0C,
    LcdCmdRDDIM     = 0x0D,
    LcdCmdRDDSM     = 0x0E,
    LcdCmdRDDSDR    = 0x0F,
    LcdCmdSLPOUT    = 0x11,
    LcdCmdPTLON     = 0x12,
    LcdCmdNORON     = 0x13,
    LcdCmdINVOFF    = 0x20,
    LcdCmdINVON     = 0x21,
    LcdCmdALLPOFF   = 0x22,
    LcdCmdALLPON    = 0x23,
    LcdCmdDISOFF    = 0x28,
    LcdCmdDISON     = 0x29,
    LcdCmdCASET     = 0x2A,
    LcdCmdPASET     = 0x2B,
    LcdCmdRAMWR     = 0x2C,
    LcdCmdRAMRD     = 0x2E,
    LcdCmdPLTAR     = 0x30,
    LcdCmdVSCRDEF   = 0x33,
    LcdCmdTEOFF     = 0x34,
    LcdCmdTEON      = 0x35,
    LcdCmdMADCTL    = 0x36,
    LcdCmdVSCRSADD  = 0x37,
    LcdCmdIDMOFF    = 0x38,
    LcdCmdIDMON     = 0x39,
    LcdCmdCOLMOD    = 0x3A,
    LcdCmdRAMWRC    = 0x3C,
    LcdCmdRAMRDRC   = 0x3E,
    LcdCmdTESLWR    = 0x44,
    LcdCmdWRDISBV   = 0x51,
    LcdCmdRDDISBV   = 0x52,
    LcdCmdWRCTRLD   = 0x53,
    LcdCmdRDCTRLD   = 0x54,
    LcdCmdWRCABC    = 0x55,
    LcdCmdRDCABC    = 0x56,
    LcdCmdWRCABCMB  = 0x5E,
    LcdCmdRDCABCMB  = 0x5F,
    LcdCmdRDABCSDR  = 0x68,
    LcdCmdRDID1     = 0xDA,
    LcdCmdRDID2     = 0xDB,
    LcdCmdRDID3     = 0xDC,
};

enum LcdExCmdEnumDef {
    LcdCmdIFMODE                = 0xB0,
    LcdCmdFRMCTR1               = 0xB1,
    LcdCmdFRMCTR2               = 0xB2,
    LcdCmdFRMCTR3               = 0xB3,
    LcdCmdINVTR                 = 0xB4,
    LcdCmdPRCTR                 = 0xB5,
    LcdCmdDISCTRL               = 0xB6,
    LcdCmdETMOD                 = 0xB7,
    LcdCmdCECTRL1               = 0xB9,
    LcdCmdCECTRL2               = 0xBA,
    LcdCmdHSLCTRL               = 0xBE,
    LcdCmdPWCTRL1               = 0xC0,
    LcdCmdPWCTRL2               = 0xC1,
    LcdCmdPWCTRL3               = 0xC2,
    LcdCmdPWCTRL4               = 0xC3,
    LcdCmdPWCTRL5               = 0xC4,
    LcdCmdVMCTRL                = 0xC5,
    LcdCmdCABCCTRL1             = 0xC6,
    LcdCmdCABCCTRL2             = 0xC8,
    LcdCmdCABCCTRL3             = 0xC9,
    LcdCmdCABCCTRL4             = 0xCA,
    LcdCmdCABCCTRL5             = 0xCB,
    LcdCmdCABCCTRL6             = 0xCC,
    LcdCmdCABCCTRL7             = 0xCD,
    LcdCmdCABCCTRL8             = 0xCE,
    LcdCmdCABCCTRL9             = 0xCF,
    LcdCmdNVMWR                 = 0xD0,
    LcdCmdNVMPKEY               = 0xD1,
    LcdCmdNVMSR                 = 0xD2,
    LcdCmdRDID4                 = 0xD3,
    LcdCmdAdjustControl1        = 0xD7,
    LcdCmdPGAMCTRL              = 0xE0,
    LcdCmdNGAMCTRL              = 0xE1,
    LcdCmdDGAMCTRL              = 0xE2,
    LcdCmdDGAMCTRL2             = 0xE3,
    LcdCmdSETIMAGE              = 0xE9,
    LcdCmdAdjustControl2        = 0xF2,
    LcdCmdAdjustControl3        = 0xF7,
    LcdCmdAdjustControl4        = 0xF8,
    LcdCmdAdjustControl5        = 0xF9,
    LcdCmdSPIRCS                = 0xFB,
    LcdCmdAdjustControl6        = 0xFC,
};

#if (1)
/**
  *@Nop(00h)  
  *Has No Parameters
  *This command is an empty command. It does not have any effect on the ILI9488. However, it can be used to terminate Frame
  *Memory Write or Read, as described in RAMWR (Memory Write) and RAMRD (Memory Read) Commands. 
*/

/**@Software @Reset(01h)
  *Has No Parameters
  *When the Software Reset command is written, it causes software reset. It resets commands and parameters to their S/W
  *Reset default values. (See default tables in each command description.)
  *After the Software Reset is applied, the display becomes blank immediately. 
  *
  *It is necessary to wait 5msec before sending a new command after software reset. The display module loads all factory
  *default values of the display supplier to the registers during this 5msec. If Software Reset is applied during the Sleep Out
  *mode, it will be necessary to wait 120msec before sending the Sleep Out command.
  *The Software Reset Command cannot be sent during the Sleep Out sequence.
*/

/**@Read @Display @Identification @Information(04h) 
  *This read byte can read 24 bits of display identification information.
  *The 1st parameter is a dummy data.
  *The 2nd parameter (ID1 [7:0]): LCD module? manufacturer ID
  *The 3rd parameter (ID2 [7:0]): LCD module/driver version ID
  *The 4th parameter (ID3 [7:0]): LCD module/driver ID 
  *
*/

/**@Read @Number @of @the @Errors @on @DSI (05h) 
  *The 1st parameter is a dummy data.
  *The 2nd parameter indicates the number of errors on the DSI. More detailed description of the bits is below.
  *P [6...0] bits indicate the number of the errors.
  *P [7] is set to 1 if there is overflow with P [6..0] bits.
  *P [7...0] bits are set to 0 (RDDSM(0Eh)? D0 is set to 0 at the same time) after the second parameter information is sent (=
  *The read function is completed). This function always returns P [7...0] = 00h if the parallel MCU interface is selected. 
  *
*/

/**@Read @Display @Status (09h)
  *
  *
*/

/**@Read @Display @Power @Mode (0Ah) 
  *
  *
  *
*/

/**@Read @Display @MADCTL (0Bh)  
  *
  *
  *
*/

/**@Read @Display @Pixel @Format (0Ch) 
  *
  *
  *
*/

/**@Read @Display @Image @Mode (0Dh) 
  *
  *
  *
*/

/**@Read @Display @Signal @Mode (0Eh) 
  *
  *
  *
*/

/**@Read @Display @SelfDiagnostic @Result (0Fh) 
  *
  *
  *
*/

/**@Sleep @OUT (11h) 
  *Has no parameters
  *This command turns off the sleep mode.
  *In this mode, the DC/DC converter is enabled, and Internal oscillator and panel scanning are activated. 
  *
  *
*/

/**@Partial @Mode @ON (12h) 
  *Has no parameters
  *This command will turn on the Partial mode. The Partial mode window is described in the Partial Area command (30H).
  *To leave the Partial mode, the Normal Display Mode On command (13H) should be written. 
  *
  *
*/

/**@Normal @Display @Mode @ON (13h)
  *Has no parameters
  *This command will make the display return to the normal mode. Normal Display Mode On means Partial mode off and Scroll
  *mode off.
  *
  *
*/

/**@Display Inversion OFF (20h) 
  *Has no parameters
  *This command is used to recover from the Display Inversion mode. Output from the Frame Memory is enabled.
  *This command makes no change to the content of the frame memory.
  *This command does not change any other status. 
  *
*/

/**@Display @Inversion @ON (21h) 
  *Has no parameters
  *This command is used to enter the Display Inversion mode.
  *This command makes no change of the content of the frame memory. Every bit is inverted from the frame memory to the
  *display.
  *This command does not change any other status.
  *To exit Display Inversion mode, the Display inversion OFF command (20h) should be written. 
*/ 

/**@All @Pixels @   OFF (22h) 
  *Has no parameters
  *This command turns the display panel black in the Sleep Out mode, and the status of the Display On/Off register can be On or
  *Off. This command makes no change to the contents of the Frame Memory. This command does not change any other status.
  *
  *To exit this mode, All Pixels On, Normal Display Mode On or Partial Mode On commands can be used. The display panel
  *shows the content of the Frame Memory after applying Normal Display Mode On and Partial Mode On commands. 
*/

/**@All @Pixels @ON (23h) 
  *Has no parameters
  *This command turns the display panel white in the Sleep Out mode, and the status of the Display On/Off register can be On or
  *Off. This command makes no change to the contents of the Frame Memory. This command does not change any other status.
  *
  *To exit this mode, All Pixels Off, Normal Display Mode On or Partial Mode On commands can be used. The display shows the
  *content of the Frame Memory after applying Normal Display Mode On and Partial Mode On commands. 
*/

/**@Display @OFF (28h) 
  *Has no parameters
  *This command causes the ILI9488 to stop displaying the image data on the display device. The frame memory contents
  *remain unchanged. No status bits are changed. 
*/

/**@Display @ON (29h) 
  *Has no parameters
  *This command causes the ILI9488 to start displaying the image data on the display device. The frame memory contents
  *remain unchanged. No status bits are changed. 
*/

/**@Column @Address @Set (2Ah) 
  *1st Parameter SC [15:8] 
  *2st Parameter SC [7:0] 
  *3st Parameter EC [15:8] 
  *4st Parameter EC [7:0] 
  *This command is used to define the area of the frame memory that the MCU can access. This command makes no change on
  *the other driver status. The values of SC [15:0] and EC [15:0] are referred when RAMWR command is applied. Each value
  *represents one column line in the Frame Memory
*/

/**@Page @Address @Set (2Bh) 
  *1st Parameter SP [15:8] 
  *2st Parameter SP [7:0] 
  *3st Parameter EP [15:8] 
  *4st Parameter EP [7:0] 
  *This command is used to define the area of the frame memory that the MCU can access. This command makes no change on
  *the other driver status. The values of SP [15:0] and EP [15:0] are referred when RAMWR command is applied. Each value
  *represents one Page line in the Frame Memory. 

  *
*/

/**@Memory @Write (2Ch) 
  *Parameters: Pixel Data
  *This command transfers image data from the host processor to the ILI9488? frame memory starting at the pixel location
  *specified by Column Address Set (2Ah) and Page Address Set (2Bh) commands. 
  *
*/ 

/**@Memory @Read (2Eh) 
  *Parameters: Pixel Data
  *This command transfers image data from the ILI9488? frame memory to the host processor starting at the pixel location
  *specified by set_column_address and set_page_address commands. 
  *
*/

/**@Partial @Area (30h) 
  *1st Parameter SR [15:8] 
  *2st Parameter SR [7:0] 
  *3st Parameter ER [15:8] 
  *4st Parameter ER [7:0] 
  *This command defines the Partial Display mode? display area. There are two parameters associated with this command: the
  *first defines the Start Row (SR) and the second the End Row (ER), as illustrated in the following figure. SR and ER refer to the
  *Frame Memory
  *
*/

/**@Vertical @Scrolling @Definition (33h)
  *Has 6 parameters, for detail information see the product datasheet
  *
*/

/**@Tearing @Effect @Line OFF (34h)
  *Has no parameters
  *This command turns off the ILI9488? Tearing Effect output signal on the TE signal line. 
*/

/**@Tearing @Effect @Line @ON (35h) 
  *1st Parameter D0 -> M
  *This command is used to turn ON the Tearing Effect output signal from the TE signal line. Changing the MADCTL bit D4 will
  *not affect this output. The Tearing Effect Line On has one parameter, which describes the mode of the Tearing Effect Output
  *Line. 
*/

/**@Memory @Access @Control (36h) 
  *This command defines read/write scanning direction of the frame memory.
  *This command makes no change on other driver status. 
  *1st Parameter: {
  *D7 MY Row Address Order    -> }
  *D6 MX Column Address Order -> } => These 3 bits control the direction from the MPU to memory write/read.
  *D5 MV Row/Column Exchange  -> }
  *D4 ML Vertical Refresh Order LCD vertical refresh direction control. 
  *D3 BGR RGB-BGR Order
  *D2 MH Horizontal Refresh ORDER LCD horizontal refreshing direction control. 
  *D1 X
  *D0 X
  *
  *
*/

/**@Vertical @Scrolling @Start @Address (37h)  
  *
  *
  *
*/

/**@Idle @Mode @OFF (38h) 
  *
  *
  *
*/

/**@Idle @Mode @ON (39h)  
  *
  *
  *
*/

/**@Interface @Pixel @Format (3Ah) 
  *
  *
  *
*/

/**@Memory @Write @Continue (3Ch)  
  *
  *
  *
*/

/**@Memory @Read @Continue (3Eh) 
  *
  *
  *
*/

/**@Write @Tear @Scan @Line (44h)  
  *
  *
  *
*/

/**@Read @Scan @Line (45h)  
  *
  *
  *
*/

/**@Write @Display @Brightness @Value (51h) 
  *
  *
  *
*/

/**@Read @Display @Brightness @Value (52h)  
  *
  *
  *
*/

/**@Write @CTRL @Display @Value (53h) 
  *
  *
  *
*/

/**@Read @CTRL @Display @Value (54h)  
  *
  *
  *
*/

/**@Write @Content @Adaptive @Brightness @Control @Value (55h)  
  *
  *
  *
*/

/**@Read @Content @Adaptive @Brightness @Control @Value (56h)  
  *
  *
  *
*/

/**@Write @CABC @Minimum @Brightness (5Eh)  
  *
  *
  *
*/

/**@Read @CABC @Minimum @Brightness (5Fh)  
  *
  *
  *
*/

/**@Read @Automatic @Brightness @Control @Selfdiagnostic @Result (68h)  
  *
  *
  *
*/

/**@Read @ID1 (DAh), @Read @ID2 (DBh), @Read @ID3 (DCh)    
  *
  *
  *
*/

/**@ 
  *
  *
  *
*/
#endif
#include <stdint.h>

class TFT_ILI9486 {
  
	private : 	
		/*interfacing functions*/
		uint16_t spiRxTx_9bit (uint16_t data);
        void setReset (bool reset);
		void delay_ms (uint32_t  ms);
        void hal_init ();
        void hal_backlight (int value);
		/*interfacing functions*/
    
        uint8_t backlight;
    
        void writeCmd (uint8_t __data);
		void writeData (uint8_t __data);
		void resetHard ();
        void DummyCycle (uint32_t __cnt);
	public:
        TFT_ILI9486 ();
		
        
		uint16_t init ();
		uint16_t window (uint16_t __X, uint16_t __Y, uint16_t __dX, uint16_t __dY);
        void Rotate90Clockwise ();
    
        const char* getDriverInfo ();
        void setBackLight (int value);
};

union __IntToByte {
	uint8_t Byte[2];
	uint16_t Int;
};


#endif


