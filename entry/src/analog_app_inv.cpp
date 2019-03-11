
#include "analog_app.h"
#include "gui.h"

uint8_t AdModes[] = 
{
    ((   1 << AD_WATCH_GRID      ) |
    (   1 << AD_WATCH_BORDER    ) | 
    (   1 << AD_WATCH_COORDS    ) | 
    (   1 << AD_WATCH_VSTRIPS   ) |
    (   1 << AD_GRID_GRAIN_MODE )),
    
    0x00,
    
    ((  1 << AD_WATCH_GRID      ) |
    (   1 << AD_WATCH_BORDER    ) | 
    (   1 << AD_WATCH_COORDS    ) | 
    (   1 << AD_WATCH_VSTRIPS   )),
    
    ((   1 << AD_WATCH_GRID      ) |
    (   1 << AD_WATCH_BORDER    ) | 
    (   1 << AD_WATCH_COORDS    ) | 
    (   1 << AD_WATCH_VSTRIPS   ) |
    (   0 << AD_GRID_GRAIN_MODE )),
    
    ((  1 << AD_WATCH_GRID      ) |
    (   1 << AD_WATCH_BORDER    ) | 
    (   0 << AD_WATCH_COORDS    ) | 
    (   1 << AD_WATCH_VSTRIPS   ) |
    (   0 << AD_GRID_GRAIN_MODE )),
    
    ((  1 << AD_WATCH_GRID      ) |
    (   0 << AD_WATCH_BORDER    ) | 
    (   0 << AD_WATCH_COORDS    ) | 
    (   0 << AD_WATCH_VSTRIPS   ) |
    (   0 << AD_GRID_GRAIN_MODE )),
    
    ((  1 << AD_WATCH_GRID      ) |
    (   0 << AD_WATCH_BORDER    ) | 
    (   0 << AD_WATCH_COORDS    ) | 
    (   0 << AD_WATCH_VSTRIPS   ) |
    (   1 << AD_GRID_GRAIN_MODE )),
};



const char *sTextArray[] = 
{
    "5V/Div",
    "2V/Div",
    "1V/Div",
    "500mV/Div",
    "200mV/Div",
    "100mV/Div",
    "50mV/Div",
};



const char *tmTextArray[] = 
{
    "200mS/Div",
    "100mS/Div",
    "50mS/Div",
    "20mS/Div",
    "10mS/Div",
    "5mS/Div",
    "2mS/Div",
    "1mS/Div",
    "500uS/Div",
    "200uS/Div",
    "100uS/Div",
    "50uS/Div",
    "20uS/Div",
    "10uS/Div",
    "5uS/Div",
    "2uS/Div",
    "1uS/Div",
    "500nS/Div",
    "200nS/Div",
};


const char *helpText =  "\
AB - A & B run independently;\n\
A+B - it will show sum graph;\n\
A-B - absolute difference;\n\
A*B - multiply of both channels;\n\
A/B - division A by B (zero div suppressible)\n\
A(sin(T)) - A will be a function of pre-defined\n\
function sin(T), whith period = 2 * Div(H scan)\n\
A(B) - A will be a function of B (Figure of Lissajous);\n\
FFT(X) - Fast Fourier Transfer; X - A or B\n\
Touch screen to exit\
";

const char *mTextArray[] = 
{
    "AB",
    "A+B",
    "A-B",
    "A*B",
    "A/B",
    "A(T)",
    "A(B)",
    "FFT A",
    "FFT B",
};

const char *tIconPaths[] = 
{
    "trise.bmp",
    "tfall.bmp",
    "tlow.bmp",
    "thigh.bmp",
    "tpeak.bmp",
    "tpit.bmp",
    "tany.bmp"
};

TSprite tSpritesArray[T_ARRAY_END];

ADC_CTL adc_ctl = 
{
    T_RISE,
    S_1V_DIV,
    S_1V_DIV,
    TM_1MS_DIV,
    M_AB,
    AD_WATCH_ALL,
    0,
    0,
    4,
    2,
        {
            RGB24_TO_RGB16(128, 128, 128),
            RGB24_TO_RGB16(32, 128, 32),
            RGB24_TO_RGB16(128, 128, 0),
            RGB24_TO_RGB16(0, 128, 255),
            RGB24_TO_RGB16(64, 255, 255),
            RGB24_TO_RGB16(255, 64, 255),
            RGB24_TO_RGB16(0, 0, 0),
        }
};

const char *tColorArray[] = 
{
    "GRID COLOR",
    "COORDS COLOR",
    "CHA COLOR",
    "CHB COLOR",
    "HSTRIP COLOR",
    "VSTRIP COLOR",
    "BACK COLOR",
};

int8_t *current_scale_p;

Box<range_t> strip_h1, strip_h2, strip_v1, strip_v2;
