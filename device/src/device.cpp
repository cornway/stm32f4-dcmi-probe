#include <stdint.h>
#include "main.h"
#include "gpio.h"
#include "device.h"
#include "device_conf.h"

#include "VM.h"
#include "vmapi.h"
#include "vmem.h"

extern "C" void *vmalloc (UINT_T size)
{
    //return new uint8_t[size];
    return malloc(size);
}
extern "C" void vmfree (void *p)
{
    //delete (uint8_t *)p;
    free(p);
}


#include "init.h"
#include "fpga.h"
#include "ccpu.h"
#include "tsc2046.h"
#include "adv7180.h"
#include "adv7180_conf.h"

ADV7180_VIDEO_DECODER adv7180;
tsc2046Drv tsc2046;
Fpga fpga;

Device::Device ()
{
    
}


#include "flash_diskio.h"
#include "ff_gen_drv.h"

extern Diskio_drvTypeDef  FLASHDISK_Driver;

FATFS FLASHDISKFatFs; 
char FLASHDISKPath[4]; 

void Device::init ()
{
    init::hal();
    init::clock();
    init::clock_enable();
    init::nvic ();
    init::tim_hal_milis();
    init::fsmc();
    init::clock_out();
    init::gpio();
    init::tim_tsc_update();
    init::audio();
    init::usb();
    init::i2c1();
    adv7180.init(ADV_I2C_ADDRESS);
    tsc2046.init(false);
    
    
    fpga.init(0);
    uint16_t psram_lat =    (0 << CCPU_PSRAM_ADLAT_GP) 	| 
                            (1 << CCPU_PSRAM_DLAT_GP) 	| 
							(0 << CCPU_PSRAM_HOLD_GP);
    
    CCPU_WRITE_REG(CCPU_WRITE_PSRAM_LAT, psram_lat);
    CCPU_WRITE_REG(CCPU_WRITE_RENDER_CTL_REGISTER, 0);
    CCPU_WRITE_REG(CCPU_WRITE_FILL_CTL, 0);
    CCPU_WRITE_REG(CCPU_WRITE_COPY_CTL, 0);
	CCPU_WRITE_REG(CCPU_WRITE_COLOR_MODE, 0);	 
    
    FLASHDISK_Driver.disk_initialize(1);
    FATFS_LinkDriver(&FLASHDISK_Driver, FLASHDISKPath);
    f_mount(&FLASHDISKFatFs, (TCHAR const*)FLASHDISKPath, 0);
    
    vm::init();
    HAL_InitTick (TICK_INT_PRIORITY);
}


void Device::run ()
{ 
        vm::start();
        for (;;) {
        }
}


uint32_t sys_time_milis = 0; /*for lwip tcp\ip stack*/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	
	switch ((uint32_t)htim->Instance) {
		case (uint32_t)TIM3: 
			         tsc2046.tim_it_handle();
		break;
		case (uint32_t)TIM2:
                     sys_time_milis++;
			         HAL_IncTick();
                     
		break;
		
	}
  
}


void *operator new (uint32_t size)
{
    return malloc(size);
}

void operator delete (void *p)
{
    free(p);
}


extern gpio::gpio_dsc tsc_pen_pin_dsc;
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == tsc_pen_pin_dsc.pin) {
        tsc2046.IT_Handle();
    }
}

#include "gui_defs.h"
void bmp_mach_conv (void **dest, uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    uint16_t *p = (uint16_t *)*dest;
    *p = RGB24_TO_RGB16(r, g, b);
    *dest = (void *)++p;
}

#include "fgpu.h"

extern gpio::gpio_dsc FpgaMax10_gpu_wait_pin;
void gpu_wait (void)
{
    while (gpio::pin_read(FpgaMax10_gpu_wait_pin) == true) {}
}


#include "wave_sample.h"

extern DAC_HandleTypeDef    dac1Handle;

void WaveSample::ll_play (WAVE_DSC dsc, void *buffer)
{
  uint32_t allign = 0;
  switch (dsc.bitRate) {
      case 8 : allign = DAC_ALIGN_8B_R;
          break;
      case 16 : allign = DAC_ALIGN_12B_L;
          break;
      default : return;
          //break;
  }
  if(HAL_DAC_Start_DMA(&dac1Handle, DAC_CHANNEL_1, (uint32_t*)buffer, dsc.dataSize, allign) != HAL_OK)
  {
    for (;;);
  }
}

#include "time.h"
void WaveSample::ll_wait ()
{
    time::delay_ms(200);
}


#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_msc.h"
#include "usbd_storage.h"

void *usbd_malloc (uint32_t size)
{
    return malloc(size);
}

void usbd_free (void *p)
{
    free(p);
}


extern USBD_HandleTypeDef USBD_Device;
void usbdStart (void)
{
    USBD_Start(&USBD_Device);
}

void usbdStop (void)
{
    USBD_Stop(&USBD_Device);
}






