#ifndef DEVICE_CONF
#define DEVICE_CONF


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "device_gui_conf.h"
   

#define BATTERY_CORRECTION_CONSTANT (2.0F)

#define BATTERY_MAX_VOLTAGE			(3.300F)
#define BATTERY_FULL				(3.300F)
#define BATTERY_MEDIUM				(3.100F)
#define BATTERY_LOW					(3.000F)


#define BRIGHTNESS_MAX_LEVEL		(100U)
#define BRIGHTNESS_MEDIUM_LEVEL		(75U)
#define BRIGHTNESS_LOW_LEVEL		(50U)
#define BRIGHTNESS_OFF_LEVEL		(20U)


#define FRAME_MEMORY_BASE           (0x60000000UL)
#define FRAME_MEMORY_SIZE           (0x0004B000UL)
    
#define SPRITE_MEMORY_BASE          (0x6004c000UL)
#define SPRITE_MEMORY_SIZE          (0x00025800UL)

#define VIDEO_FRAME_BASE            (0x6004b000UL)
#define VIDEO_FRAME_SIZE            (0x00025800UL)

#define FLASH_DEVICE_SIZE   0x80000

#endif /*DEVICE_CONF*/

#define NES_ARM_LIB


#ifndef CONFIG_H
#define CONFIG_H


#define NES_VRAM_START      (0x6004c000U)
#define SPRITE_RAM_START    (0x60038400U)

#define WAVE_PCM_ONLY       (1U)
#define WAVE_MONO           (1U)
#define WAVE_SAMPLE_RATE    (11025U)
#define WAVE_BIT_RATE       (8U)

#define PLAY_STARTUP_SOUND  (0U)



#define ENC28J60_USE_PBUF

extern void nes_frame_update(void);

#ifdef __cplusplus
}
#endif


#define STM32

#endif


/**/

