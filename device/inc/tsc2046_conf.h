#ifndef TSC2046_CONF_H
#define TSC2046_CONF_H

#include "device_conf.h"

#define TSC_CD_TIMEOUT      (1U)
#define TSC_HOLD_TIMEOUT    (20U)
#define TSC_AC_SENSE        (10U)


// wait busy state(A/D conversion)
#define TSC2046_SPI_BUSY_WAIT_DURATION 3


/**
 * sampling configuration
 *  mode
 *   0: Average; result = sum(N samples)/N
 *   1: Weighted Averaging; skip first and tail sample, result = sum(samples[1 to N-1])/(N-2)
 *   2: Middle Value; result = median(samples)
 *   3: Averaging the Closest Data; result = sum(closest_two_data(samples))/2
 *   4: Touch Screen Panel Pressure; if (pressure > threshold) { result = sample; }
 */
#define TSC2046_SAMPLING_MODE 2

// maximum number of sampling, for average method
#define TSC2046_MAX_SAMPLE 5

// pressure threshold for mode.4
#define TSC2046_PRESSURE_THRESHOLD 70

/**
 * Calibration
 */

// calibration mode
// 0. http://www.ti.com/lit/an/slyt277/slyt277.pdf
// 1. single value N degree least squares method
// 2. Linear interpolation
#define TSC2046_CALIBRATION_MODE 0

// maximum number of sampling, while calibration
#define TSC2046_CAL_MAX_SAMPLE (25)

// parameter for mode.1
#define TSC2046_CAL_DEG 3     // degree of approximation curve
#define TSC2046_CAL_POINTS 8  // number of samples

// display resolution
#define TSC2046_WIDTH  TFT_WIDTH
#define TSC2046_HEIGHT TFT_HEIGHT

// LCD draw interface wrapper for calibration
extern int tsc2046_frame_buffer_open ();
#define TOUCH_CAL_LCD_FRAME_OPEN() tsc2046_frame_buffer_open()

extern void tsc2046_frame_buffer_close ();
#define TOUCH_CAL_LCD_FRAME_CLOSE() {\
    tsc2046_frame_buffer_open();\
}

extern void tsc2046_frame_buffer_safe_clear ();
#define TOUCH_CAL_LCD_CLEAR() {\
    tsc2046_frame_buffer_safe_clear();\
}

extern void tsc2046_frame_buffer_safe_circle_fill (int x, int y, int r);
#define TOUCH_CAL_LCD_DRAW_CIRCLE(x,y,r) {\
    tsc2046_frame_buffer_safe_circle_fill(x, y, r); \
}

extern void tsc2046_frame_buffer_safe_draw_string (char * str, int x, int y);
#define TOUCH_CAL_LCD_DRAW_STRING(str,x,y) {\
    tsc2046_frame_buffer_safe_draw_string(str, x, y); \
}

/**
 * definition of Control Byte parameters
 */

#define TSC2046_CB_START_BIT    (1<<7)

// input channel, for SER/DFR mode
#define TSC2046_CB_X_POSITION   (0x5<<4)
#define TSC2046_CB_Y_POSITION   (0x1<<4)
#define TSC2046_CB_Z1_POSITION  (0x3<<4)
#define TSC2046_CB_Z2_POSITION  (0x4<<4)

// input channel, for SER mode only
#define TSC2046_CB_TEMP0 (0x0<<4)
#define TSC2046_CB_TEMP1 (0x7<<4)
#define TSC2046_CB_VBAT  (0x2<<4)
#define TSC2046_CB_AUXIN (0x6<<4)

// ADC mode
#define TSC2046_CB_12BIT (0<<3)
#define TSC2046_CB_8BIT  (1<<3)
#define TSC2046_CB_SER   (1<<2)
#define TSC2046_CB_DFR   (0<<2)

// power down configuration                 // PENIRQ
#define TSC2046_CB_PD_AFTER_CONV     (0x0) // Enabled
#define TSC2046_CB_PD_REF_OFF_ADC_ON (0x1) // Disabled
#define TSC2046_CB_PD_REF_ON_ADC_OFF (0x2) // Enabled
#define TSC2046_CB_PD_ALWAYS_POWERED (0x3) // Disabled

// command presets
#define TSC2046_COMMAND_BASE ( \
    TSC2046_CB_START_BIT| \
    TSC2046_CB_12BIT| \
    TSC2046_CB_DFR| \
    TSC2046_CB_PD_AFTER_CONV \
)
#define TSC2046_COMMAND_READ_X  (TSC2046_COMMAND_BASE | TSC2046_CB_X_POSITION)
#define TSC2046_COMMAND_READ_Y  (TSC2046_COMMAND_BASE | TSC2046_CB_Y_POSITION)
#define TSC2046_COMMAND_READ_Z1 (TSC2046_COMMAND_BASE | TSC2046_CB_Z1_POSITION)
#define TSC2046_COMMAND_READ_Z2 (TSC2046_COMMAND_BASE | TSC2046_CB_Z2_POSITION)

/**
 * Debug
 */
//#define TSC2046_DEBUG
//extern UART_HandleTypeDef huart1;
//#define TSC2046_DEBUG_PRINT(str) {\
//  HAL_UART_Transmit(&huart1, str, strlen(str), 0xff);\
//}

/**
 * Exports
 */


#endif /*TSC2046_CONF_H*/

