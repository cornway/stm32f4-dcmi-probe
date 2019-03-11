#ifndef DEVICE_GUI_CONF
#define DEVICE_GUI_CONF

#include <stdint.h>

typedef uint16_t color_t;
typedef int16_t range_t;

#define COLOR_WHITE (0xffffU)
#define COLOR_RED   (0xf800U)
#define COLOR_BLUE  (0x001fU)
#define COLOR_GREEN (0x07e0U)

#define TFT_WIDTH (480U)
#define TFT_HEIGHT (320U)

#define SCREEN_PRESENT  (1U)

#define SCR_WIDTH   TFT_WIDTH
#define SCR_HEIGHT  TFT_HEIGHT

#define GRID_CELLS_WIDTH (10)
#define GRID_CELLS_HEIGHT (8)
#define GRID_GRAIN		  (3)



#endif /*DEVICE_GUI_CONF*/

/**/

