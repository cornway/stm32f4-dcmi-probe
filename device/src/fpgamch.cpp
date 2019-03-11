#include "gpio.h"
#include "fpga.h"

static gpio::gpio_dsc FpgaMax10_conf_select_pin     = {GPIOG, GPIO_PIN_2};
static gpio::gpio_dsc FpgaMax10_conf_done_pin       = {GPIOG, GPIO_PIN_6};
static gpio::gpio_dsc FpgaMax10_crc_err_pin         = {GPIOG, GPIO_PIN_4};
static gpio::gpio_dsc FpgaMax10_nConf_pin           = {GPIOG, GPIO_PIN_3};
static gpio::gpio_dsc FpgaMax10_nStatus_pin         = {GPIOG, GPIO_PIN_5};        
gpio::gpio_dsc FpgaMax10_gpu_wait_pin               = {GPIOH, GPIO_PIN_7};     



void Fpga::hal_init ()
{
  __GPIOG_CLK_ENABLE();
  __GPIOH_CLK_ENABLE();
    
  gpio::init(FpgaMax10_conf_select_pin, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_HIGH);
  gpio::init(FpgaMax10_nConf_pin, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_HIGH);
  gpio::init(FpgaMax10_gpu_wait_pin, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_HIGH);
    
  gpio::init(FpgaMax10_nStatus_pin, GPIO_MODE_INPUT, GPIO_PULLUP, GPIO_SPEED_HIGH);
  gpio::init(FpgaMax10_conf_done_pin, GPIO_MODE_INPUT, GPIO_PULLUP, GPIO_SPEED_HIGH); 

  gpio::init(FpgaMax10_crc_err_pin, GPIO_MODE_IT_RISING, GPIO_NOPULL, GPIO_SPEED_HIGH);
    
  //HAL_NVIC_SetPriority(EXTI4_IRQn, 2, 0);
  //HAL_NVIC_EnableIRQ(EXTI4_IRQn);
}

void Fpga::setConfSelectPin (int image)
{
    gpio::pin_set(FpgaMax10_conf_select_pin, image);
}

void Fpga::setNConfPin (bool state)
{
    gpio::pin_set(FpgaMax10_nConf_pin, state);
}


bool Fpga::getNStatusPin ()
{
    return gpio::pin_read(FpgaMax10_nStatus_pin);
}

bool Fpga::getCrcErrPin ()
{
    return false;
}

bool Fpga::getConfDonePin ()
{
    return gpio::pin_read(FpgaMax10_conf_done_pin);
}


