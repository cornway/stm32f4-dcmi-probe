#include "tsc2046.h"
#include "main.h"
#include "gpio.h"



gpio::gpio_dsc tsc_busy_pin_dsc = {GPIOD, GPIO_PIN_3};
gpio::gpio_dsc tsc_pen_pin_dsc = {GPIOD, GPIO_PIN_2};
gpio::gpio_dsc tsc_sel_pin_dsc = {GPIOG, GPIO_PIN_15};

void tsc2046Drv::ll_init (bool it_ena)
{
    __GPIOD_CLK_ENABLE();
    __GPIOG_CLK_ENABLE();
    gpio::init(tsc_busy_pin_dsc, GPIO_MODE_INPUT, GPIO_PULLUP, GPIO_SPEED_HIGH);
    gpio::init(tsc_sel_pin_dsc, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_SPEED_HIGH);
    gpio::init(tsc_pen_pin_dsc, GPIO_MODE_IT_FALLING, GPIO_PULLUP, GPIO_SPEED_HIGH);
    
    if (it_ena) {
        HAL_NVIC_SetPriority(EXTI2_IRQn, 2, 0);
        HAL_NVIC_EnableIRQ(EXTI2_IRQn);
    }
    
}

void  tsc2046Drv::ll_sel (bool sel)
{
    gpio::pin_set(tsc_sel_pin_dsc, sel);
}

bool tsc2046Drv::ll_busy ()
{
    return gpio::pin_read(tsc_busy_pin_dsc);
}

uint8_t tsc2046Drv::ll_rw (uint8_t data)
{
    while ((SPI1->SR & SPI_FLAG_TXE) == 0){}
			*(__IO uint8_t *)&SPI1->DR = data;
	while ((SPI1->SR & SPI_FLAG_RXNE) == 0){}

    return *(__IO uint8_t *)&SPI1->DR;
}



#include "fgpu.h"
#include "gui.h"

extern FontArray fontArray;

static GuiEngine<color_t, range_t, COLOR_WHITE> *tsc2046_engine;
static GContentPane<color_t, range_t, COLOR_WHITE> *tsc2046_pane;

FG_Rect fgRect = {0, 0, TFT_WIDTH, TFT_HEIGHT};

int tsc2046_frame_buffer_open ()
{
    tsc2046_engine = new GuiEngine<color_t, range_t, COLOR_WHITE>(fontArray.font_mono, (color_t *)FRAME_MEMORY_BASE, 0, 0, TFT_WIDTH, TFT_HEIGHT);
    if (tsc2046_engine == nullptr) {
        return -1;
    }
    tsc2046_pane = tsc2046_engine->newContentPane(0, 0, TFT_WIDTH, TFT_HEIGHT);
    if (tsc2046_pane == nullptr) {
        delete tsc2046_engine;
        return -1;
    }
    return 0;
}

void tsc2046_frame_buffer_close ()
{
    delete tsc2046_engine;
}

void tsc2046_frame_buffer_safe_clear ()
{
    tsc2046_pane->fill(COLOR_WHITE);
    gpu_update(fgRect);
    gpu_wait();
}

void tsc2046_frame_buffer_safe_circle_fill (int x, int y, int r)
{
    Box<range_t> box = {(range_t)x, (range_t)y, (range_t)r, (range_t)r};
    tsc2046_pane->getGraphic()->circleFilled(box, (float)r, COLOR_GREEN);
    gpu_update(fgRect);
    gpu_wait();
}

void tsc2046_frame_buffer_safe_draw_string (char * str, int x, int y)
{
    Box<range_t> box = {(range_t)x, (range_t)y, TFT_WIDTH, 30};
    tsc2046_pane->getGraphic()->drawString(box, str, 0xf800);
    gpu_update(fgRect);
    gpu_wait();
}

