#ifndef ANALOG_APP_H
#define ANALOG_APP_H


#include "app_def.h"
#include "vm.h"
#include "vmapi.h"
#include "device_conf.h"
#include "app_macro.h"
#include "adc_core.h"
#include "gui.h"

/*color_t gridColor;
    color_t coordsColor;
    color_t chaColor;
    color_t chbColor;
    color_t hstripColor;
    color_t vstripColor;*/

enum {
    SKIN_ARRAY_START = -1,
    SKIN_GRID_COLOR = 0,
    SKIN_COORDS_COLOR,
    SKIN_CHA_COLOR,
    SKIN_CHB_COLOR,
    SKIN_HSTRIP_COLOR,
    SKIN_VSTRIP_COLOR,
    SKIN_BACK_COLOR,
    SKIN_ARRAY_END,
};

typedef struct {
    color_t color[SKIN_ARRAY_END];
} ADC_PLOTTER_PALETTE;

typedef struct {
    int8_t trigger;
    int8_t scale_a;
    int8_t scale_b;
    int8_t timebase;
    int8_t mode;
    int8_t view;
    int8_t auto_zero;
    int8_t fft_grain;
    uint8_t chWidth;
    uint8_t stripWidth;
    ADC_PLOTTER_PALETTE skin;
} ADC_CTL;

class TSprite {
    private :
        tImage<color_t> icon;
    public :
       TSprite ()
       {
           this->icon.Image = nullptr;
           this->icon.W = 0;
           this->icon.H = 0;
       }
       ~TSprite ()
       {
           
       }
       
       void setIcon (tImage<color_t> *icon)
       {
           this->icon.Image = icon->Image;
           this->icon.W = icon->W;
           this->icon.H = icon->H;
       }
       
       tImage<color_t> *
       getImage ()
       {
           return &this->icon;
       }
};


enum {
    AD_WATCH_GRID,
    AD_WATCH_BORDER,
    AD_WATCH_COORDS,
    AD_WATCH_VSTRIPS,
    AD_GRID_GRAIN_MODE,
};

enum {
    AD_ARRAY_START = -1,
    AD_WATCH_ALL  = 0, 
    AD_WATCH_NONE,
    AD_WATCH_BUT_GRID, 
    AD_WATCH_BUT_TINY_GRID,    
    AD_WATCH_BUT_XY,   
    AD_TINY_GRID_ONLY,                       
    AD_GRID_ONLY,   
    AD_ARRAY_END,    
};




enum {
    S_ARRAY_START = -1,
    S_5V_DIV = 0,
    S_2V_DIV,
    S_1V_DIV,
    S_500MV_DIV,
    S_200MV_DIV,
    S_100MV_DIV,
    S_50MV_DIV,
    S_ARRAY_END,
}; /*scale modes enumeration*/

enum {
    TM_ARRAY_START = -1,
    TM_200MS_DIV = 0,
    TM_100MS_DIV,
    TM_50MS_DIV,
    TM_20MS_DIV,
    TM_10MS_DIV,
    TM_5MS_DIV,
    TM_2MS_DIV,
    TM_1MS_DIV,
    TM_500US_DIV,
    TM_200US_DIV,
    TM_100US_DIV,
    TM_50US_DIV,
    TM_20US_DIV,
    TM_10US_DIV,
    TM_5US_DIV,
    TM_2US_DIV,
    TM_1US_DIV,
    TM_500NS_DIV,
    TM_200NS_DIV,
    TM_ARRAY_END,
}; /*time base modes enumeration*/


enum {
    M_ARRAY_START =     -1,
    M_AB                = 0,
    M_ABSUM,
    M_ABDIF,
    M_AB_MUL,
    M_AB_DIV,
    M_AS_FUNC,
    M_AB_FUNC,
    M_FFT_A,
    M_FFT_B,
    M_ARRAY_END,
}; /*watch modes enumeration*/

enum {
    T_ARRAY_START       = -1,
    T_RISE              = 0,
    T_FALL,
    T_LOW,
    T_HIGH,
    T_PEAK,
    T_PIT,
    T_ANY,
    T_ARRAY_END,
}; /*trigger modes enumeration*/

extern FontArray fontArray;
extern uint8_t AdModes[];
extern const char *sTextArray[];
extern const char *tmTextArray[];
extern const char *helpText;
extern const char *mTextArray[];
extern const char *tColorArray[];

extern ADC_CTL adc_ctl;
extern int8_t *current_scale_p;

extern const char *tIconPaths[];
extern TSprite tSpritesArray[T_ARRAY_END];

extern Box<range_t> strip_h1, strip_h2, strip_v1, strip_v2;
extern int snapshot (char *name);

#define ADC_LOCK_ID     (0x800)

INT_T analog_app (WORD_T size, void *argv);

static int menu_init (GuiEngine<color_t, range_t, COLOR_WHITE> *engine);
static int menu_deinit (GuiEngine<color_t, range_t, COLOR_WHITE> *engine);
static int menu_repaint (GuiEngine<color_t, range_t, COLOR_WHITE> *engine);
static int menu_invoke_event (abstract::Event e);

static int random (int seed);

static int load_cfg (char *filename);
static int store_cfg (char *filename);


static void plotter_draw (NonPalette<color_t, range_t, COLOR_WHITE> *component);
static void plotter_listener (abstract::Event e);

static Point<range_t> curve (NonPalette<color_t, range_t, COLOR_WHITE> *c, adc_word_t *buffer, int size, color_t color);
static Point<range_t> spectre (NonPalette<color_t, range_t, COLOR_WHITE> *c, BaseComplex *buffer, int bsize, int size, color_t color);
static Box<range_t> curve_lisajou (NonPalette<color_t, range_t, COLOR_WHITE> *c, adc_word_t *buffer_a, adc_word_t *buffer_b, int size, color_t color);


static void update_settings (ADC_CTL *ctl);
static void lExit (abstract::Event e);
static void lStore (abstract::Event e);
static void lLoad (abstract::Event e);

static void update_sprites ();

static void trigger_wait (int8_t trigger, int8_t channel);
static float setScale (int8_t scale);
static float setTimeBase (int8_t timeBase);


static INT_T adc_app (WORD_T size, void *argv);


#endif  /*ANALOG_APP_H*/
