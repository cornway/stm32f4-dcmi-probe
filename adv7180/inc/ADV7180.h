#ifndef ADV7180_VIDEO_CODEC
#define ADV7180_VIDEO_CODEC

#include <stdint.h>
#include "errno.h"

/*
brief
*/

enum ADV_ERRORS {
  ADV7180_OK,
  ADV7180_NOT_FOUND,
};

#define ADV_PWR_DOWN_ASSERT 	(false)
#define ADV_PWR_DOWN_DEASSERT 	(true)
#define ADV_RESET_ASSERT 		(false)
#define ADV_RESET_DEASSERT 		(true)  

#define ADV_EXTENDED_INFO   (0U)


namespace adv_rw_map {
	enum adv_rw_registers_map {
		inputControl 		= 0x00,
		videoSelection    	= 0x01,
		outputControl     	= 0x03,
		extOutputControl  	= 0x04,
		autoDetectEnable    = 0x07,
		contrast            = 0x08,
		brightness          = 0x0a,
		hue                 = 0x0b,
		defaultValueY       = 0x0c,
		defaultValueC       = 0x0d,
		adiControl          = 0x0e,
		powerManagement     = 0x0f,
		analogClampControl  = 0x14,
		digitalClampCtl_1   = 0x15,
		shapingFilterCtl_1  = 0x17,
		shapingFilterCtl_2  = 0x18,
		combFilterCtl       = 0x19,
		adiCtl_2            = 0x1d,
		pixelDelayCtl       = 0x27,
		miscGainCtl         = 0x2B,
		agcModeCtl          = 0x2c,
		chromaGainCtl_1_WO  = 0x2d,
		chromaGainCtl_2_WO  = 0x2e,
		lumaGainCtl_1_WO    = 0x2f,
		lumaGainCtl_2_WO    = 0x30,
		vs_fieldCtl_1       = 0x31,
		vs_fieldCtl_2       = 0x32,
		vs_fieldCtl_3       = 0x33,
		hs_positionCtl_1    = 0x34,
		hs_positionCtl_2    = 0x35,
		hs_positionCtl_3    = 0x36,
		polarity            = 0x37,
		ntscCombCtl         = 0x38,
		palCombCtl          = 0x39,
		adcCtl              = 0x3a,
		manualWindowCtl     = 0x3d,
		resampleCtl         = 0x41,
		gemstarCtl_1        = 0x48,
		gemstarCtl_2        = 0x49,
		gemstarCtl_3        = 0x4a,
		gemstarCtl_4        = 0x4b,
		gemstarCtl_5        = 0x4c,
		ctiDnrCtl_1         = 0x4d,
		ctiDnrCtl_2         = 0x4e,
		ctiDnrCtl_3         = 0x50,
		lockCount           = 0x51,
		cvbsTrim            = 0x52,
		vs_fieldPinctl      = 0x58,
		GPO                 = 0x59,
		freeRunLineLen      = 0x8f,
		crcEnable           = 0xb2,
		adcSwitch_1         = 0xc3,
		adcSwitch_2         = 0xc4,
		letterBoxCtl_1      = 0xdc,
		letterBoxCtl_2      = 0xdd,
		sdOffsetCb          = 0xe1,
		sdOffsetCr          = 0xe2,
		sdSaturationCb      = 0xe3,
		sdSaturationCr      = 0xe4,
		ntscVBitBegin       = 0xe5,
		ntscVBitEnd         = 0xe6,
		ntscVBitToggle      = 0xe7,
		palVBitBegin        = 0xe8,
		palVBitEnd          = 0xe9,
		palFBitToggle       = 0xea,
		vBlankCtl_1         = 0xeb,
		vBlankCtl_2         = 0xec,
		afeCtl_1            = 0xf3,
		driveStrenght       = 0xf4,
		IFCompCtl           = 0xf8,
		vsModeCtl           = 0xf9,
		peakingCtl          = 0xfb,
		coringTreshhold     = 0xfc,
	};
	
	
};

namespace adv_r_map {
	enum adv_r_registers_map {
		status1             = 0x10,
		ident               = 0x11,
		status2             = 0x12,
		status3             = 0x13,
		chromaGain_1        = 0x2d,
		chromaGain_2        = 0x2d,
		lumaGain_1          = 0x2f,
		lumaGain_2          = 0x30,
		ccap_1              = 0x99,
		ccap_2              = 0x9a,
		letterBox_1         = 0x9b,
		letterBox_2         = 0x9c,
		letterBox_3         = 0x9d,
		stNoiseReadBack_1   = 0xde,
		stNoiseReadBack_2   = 0xdf,
	};
	
}



class ADV7180_VIDEO_DECODER {
    private :
        char *revision_info;
        int32_t revision_id;
        uint8_t ownAddress;
        bool ready;
        virtual uint8_t i2cMemRead (uint8_t reg);
        virtual void  i2cMemWrite (uint8_t reg, uint8_t data);
        virtual void setPwrDownPin (bool value);
        virtual void setResetPin (bool value);
        virtual void delayMs (uint32_t ms);
        virtual void hal_init ();
    
        void writeReg (uint8_t reg, uint8_t data);
        uint8_t readReg (uint8_t reg);
    public :
        ADV7180_VIDEO_DECODER ();
        ADV_ERRORS init (uint8_t address);
        
        void powerDown ();
        void powerUp ();
        bool isReady ();
    
        char *getRevisionInfo ();
        int32_t getRevisionId ();
        
        void setLumaGain (uint16_t gain);
        void setChromaGain (uint16_t gain);
        void setBrightness (uint16_t value);
        void setContrast (uint16_t value);
        bool inLock ();
        bool inFscLock ();
    
        uint16_t getChromaGain ();
        uint16_t getLumaGain ();
        uint16_t getContrast ();
        uint16_t getBrightnes ();
        char *getDecodedStandard ();
        template <typename P>
            void printInfo (P *printer)
        {
            printer->print("STD :");
            printer->print(this->getDecodedStandard());
            printer->print("\n");
            if (this->inFscLock()) {
                printer->print("Fsc locked\n");
            } else {
                printer->print("No Fsc Lock\n");
            }
            uint8_t status = this->readReg(adv_r_map::status1);
            if (status & (1 << 3)) {
                printer->print("Follow peak white\n");
            } 
            printer->print("Color kill : ");
            if (status & (1 << 7)) {
                printer->print("On\n");
            } else {
                printer->print("Off\n");
            }
            status = this->readReg(adv_r_map::status2);
            #if (ADV_EXTENDED_INFO == 1U)
                
            #endif
            if (status & (1 << 4)) {
                printer->print("Line non standard\n");
            } else {
                printer->print("Line standard\n");
            }
            if (status & (1 << 5)) {
                printer->print("fsc non standard\n");
            } else {
                printer->print("fsc standard\n");
            }
            status = this->readReg(adv_r_map::status3);
            if (status & (1 << 5)) {
                printer->print("Field correct\n");
            }
            if (status & (1 << 6)) {
                printer->print("Field seq found\n");
            }
            printer->print("Pal_Sw Lock : ");
            if (status & (1 << 7)) {
                printer->print("OK");
            } else {
                printer->print("Fail");
            }
        }
    
        int setConfFromfile (char *fileName);
        int parseReg (char *str);
};


#endif /*ADV7180_VIDEO_CODEC*/

/*End of file*/

