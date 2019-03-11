#include "time.h"
#include "gpio.h"
#include "TFTili9486.h"
#include "ccpu.h"
#include "device.h"

void TFT_ILI9486::setReset (bool reset)
{
    CCPU_WRITE_REG(CCPU_WRITE_TFT_RESET_REGISTER, reset == true ? CCPU_TFT_RESET_BM : 0);
}

uint16_t TFT_ILI9486::spiRxTx_9bit (uint16_t data)
{
    while ((CCPU_READ_REG(CCPU_READ_SPI_STATUS_REGISTER) & CCPU_SPI_BUSY_BM) == CCPU_SPI_BUSY_BM) {}
    CCPU_WRITE_REG(CCPU_WRITE_SPI_DATA_REGISTER, data);
    time::delay_us(1);    
    while ((CCPU_READ_REG(CCPU_READ_SPI_STATUS_REGISTER) & CCPU_SPI_BUSY_BM) == CCPU_SPI_BUSY_BM) {}
    return CCPU_READ_REG(CCPU_READ_SPI_REGISTER);
}

void TFT_ILI9486::delay_ms (uint32_t  ms)
{
	time::delay_ms(ms);
}

gpio::gpio_dsc tft_bcklt_gpio = {GPIOA, GPIO_PIN_9};
TIM_HandleTypeDef             TimHandle;
TIM_OC_InitTypeDef 			  sConfig;

void TFT_ILI9486::hal_init ()
{
    uint16_t spi_control_word = (0 << CCPU_SPI_CPOL_BP)                |
                                (0 << CCPU_SPI_CPHA_BP)                |
                                (0 << CCPU_SPI_DIR_BP)                 |
                                (4 << CCPU_SPI_PRESC_GP)               |
                                (9 << CCPU_SPI_COUNT_GP)               |
                                (0 << CCPU_SPI_RESET_BP);
    CCPU_WRITE_REG(CCPU_WRITE_SPI_CONTROL_REGISTER, spi_control_word);
    
    __GPIOA_CLK_ENABLE();
    gpio::init_af(tft_bcklt_gpio, GPIO_NOPULL, GPIO_SPEED_HIGH, GPIO_AF1_TIM1, GPIO_MODE_AF_PP);
    //gpio::init(tft_bcklt_gpio, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_SPEED_LOW);
    __TIM1_CLK_ENABLE();
    TimHandle.Instance = TIM1;
  
    TimHandle.Init.Prescaler     = ((SystemCoreClock /2) / 10000000) - 1;
    TimHandle.Init.Period        = (1000 - 1);
    TimHandle.Init.ClockDivision = 0;
    TimHandle.Init.CounterMode   = TIM_COUNTERMODE_UP;
    if(HAL_TIM_PWM_Init(&TimHandle) != HAL_OK)
    {
        /* Initialization Error */
        for(;;);
    }
  
    sConfig.OCMode     = TIM_OCMODE_PWM1;
    sConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfig.OCFastMode = TIM_OCFAST_DISABLE;

    /* Set the pulse value for channel 1 */
    sConfig.Pulse = 500;  
    if(HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_2) != HAL_OK)
    {
    /* Configuration Error */
        for(;;);
    }
  
    if(HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_2) != HAL_OK)
    {
    /* Starting Error */
        for(;;);
    }
}

void TFT_ILI9486::hal_backlight (int value)
{
    /*
    if (value) {
        gpio::pin_set(tft_bcklt_gpio);
    } else {
        gpio::pin_reset(tft_bcklt_gpio);
    }
    */
    
    if (this->backlight == value) {
        return;
    }
    this->backlight = value;
    
    if(HAL_TIM_PWM_Stop(&TimHandle, TIM_CHANNEL_2) != HAL_OK)
    {
        for(;;);
    }	
	
    sConfig.Pulse = this->backlight * 10;  
    if(HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_2) != HAL_OK)
    {
        for(;;);
    }
  
    if(HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_2) != HAL_OK)
    {
        for(;;);
    }
}


