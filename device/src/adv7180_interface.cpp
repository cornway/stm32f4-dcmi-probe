#include "gpio.h"
#include "time.h"
#include "adv7180.h"


gpio::gpio_dsc adv_reset_pin_dsc = {GPIOI, GPIO_PIN_6};
gpio::gpio_dsc adv_powerdown_pin_dsc = {GPIOI, GPIO_PIN_7};


extern I2C_HandleTypeDef I2cHandle;

void ADV7180_VIDEO_DECODER::hal_init ()
{
    __GPIOI_CLK_ENABLE();
    
    gpio::init(adv_reset_pin_dsc, GPIO_MODE_OUTPUT_PP, GPIO_PULLDOWN, GPIO_SPEED_LOW);
    gpio::init(adv_powerdown_pin_dsc, GPIO_MODE_OUTPUT_PP, GPIO_PULLDOWN, GPIO_SPEED_LOW);
}

void ADV7180_VIDEO_DECODER::i2cMemWrite (uint8_t reg, uint8_t data)
{
    uint8_t buf[1] = {data};
    while (HAL_I2C_Mem_Write(&I2cHandle, (uint16_t)this->ownAddress, reg, I2C_MEMADD_SIZE_8BIT, buf, 1, 25) != HAL_OK);
}

uint8_t ADV7180_VIDEO_DECODER::i2cMemRead (uint8_t reg)
{
    uint8_t buf[1] = {0};
    while (HAL_I2C_Mem_Read(&I2cHandle, (uint16_t)this->ownAddress, reg, I2C_MEMADD_SIZE_8BIT, buf, 1, 25) != HAL_OK);
    return buf[0];
}

void ADV7180_VIDEO_DECODER::setResetPin (bool value)
{
    gpio::pin_set(adv_reset_pin_dsc, value);
}

void ADV7180_VIDEO_DECODER::setPwrDownPin (bool value)
{
    gpio::pin_set(adv_powerdown_pin_dsc, value);
}

void ADV7180_VIDEO_DECODER::delayMs (uint32_t ms)
{
    time::delay_ms(ms);
}
