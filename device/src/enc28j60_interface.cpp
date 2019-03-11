#include "gpio.h"
#include "enc28j60_interface.h"
#include "enchw.h"
#include "lwip/sys.h"
#include "cc.h"

static void __delay_ns(int us);

gpio::gpio_dsc enc28j60_spi_gpio[] = 
{
    {GPIOB, GPIO_PIN_13},
    {GPIOB, GPIO_PIN_14},
    {GPIOB, GPIO_PIN_15},
};

gpio::gpio_dsc enc28j60_reset_gpio = {GPIOH, GPIO_PIN_3};
gpio::gpio_dsc enc28j60_cs_gpio = {GPIOH, GPIO_PIN_4};

SPI_HandleTypeDef         SpiHandle_Enc28j60;

void enc28j60_ll_init ()
{
    __GPIOB_CLK_ENABLE();
    __GPIOH_CLK_ENABLE();
    __SPI2_CLK_ENABLE();
    for (auto dsc : enc28j60_spi_gpio) {
        gpio::init_af(dsc, GPIO_NOPULL, GPIO_SPEED_HIGH, GPIO_AF5_SPI2, GPIO_MODE_AF_PP);
    }
    gpio::init(enc28j60_reset_gpio, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_HIGH);
    gpio::init(enc28j60_cs_gpio, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_HIGH);
    
    SpiHandle_Enc28j60.Instance               = SPI2;
    SpiHandle_Enc28j60.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
    SpiHandle_Enc28j60.Init.Direction         = SPI_DIRECTION_2LINES;
    SpiHandle_Enc28j60.Init.CLKPhase          = SPI_PHASE_1EDGE;
    SpiHandle_Enc28j60.Init.CLKPolarity       = SPI_POLARITY_LOW;
    SpiHandle_Enc28j60.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
    SpiHandle_Enc28j60.Init.CRCPolynomial     = 7;
    SpiHandle_Enc28j60.Init.DataSize          = SPI_DATASIZE_8BIT;
    SpiHandle_Enc28j60.Init.FirstBit          = SPI_FIRSTBIT_MSB;
    SpiHandle_Enc28j60.Init.NSS               = SPI_NSS_SOFT;
    SpiHandle_Enc28j60.Init.TIMode            = SPI_TIMODE_DISABLE;
    SpiHandle_Enc28j60.Init.Mode              = SPI_MODE_MASTER;  

    //HAL_SPI_DeInit(&SpiHandle_Enc28j60);
    
    if(HAL_SPI_Init(&SpiHandle_Enc28j60) != HAL_OK)
    {
        for (;;) {}
    }
    __HAL_SPI_ENABLE(&SpiHandle_Enc28j60);
}


extern uint32_t sys_time_milis;
u32_t sys_now (void)
{
    return (u32_t)sys_time_milis;
}


void enchw_select(enchw_device_t *dev)
{
    __delay_ns(2);
    gpio::pin_reset(enc28j60_cs_gpio);
}

void enchw_unselect(enchw_device_t *dev)
{
    __delay_ns(2);
    gpio::pin_set(enc28j60_cs_gpio);	
}


extern SPI_HandleTypeDef         SpiHandle_Enc28j60;
uint8_t enchw_exchangebyte(enchw_device_t *dev, uint8_t byte)
{
    while ((SPI2->SR & SPI_FLAG_TXE) == 0);
    *(__IO uint8_t *)&SpiHandle_Enc28j60.Instance->DR = byte;
    while ((SPI2->SR & SPI_FLAG_RXNE) == 0);
    byte =  *(__IO uint8_t *)&SpiHandle_Enc28j60.Instance->DR;
    
    return byte;
}

static void __delay_ns(int us)
{
    for (volatile int i = 0; i < us; i++);
}

