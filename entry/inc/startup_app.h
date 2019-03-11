#ifndef STARTUP_APP_H
#define STARTUP_APP_H

#include "app_def.h"
#include "device_conf.h"
#include "vm.h"
#include "vmapi.h"
#include "fgpu.h"
#include "gui.h"
#include "wave_sample.h"
#include "tsc2046.h"
#include "TFTili9486.h"

extern TFT_ILI9486 tft;
extern FontArray fontArray;
extern APPLICATION_CONTROL applicationControl;
extern tsc2046Drv tsc2046;


extern INT_T main_app (WORD_T size, void *argv);
extern INT_T render_app (WORD_T size, void *argv);
extern int update_ipv4 (char *token, uint8_t *ip);
extern int update_mac (char *token, uint8_t *mac);

extern TouchSensor mainAppTouchSensor;
extern TouchSensor camAppTouchSensor;
extern TouchSensor explAppTouchSensor;
extern TouchSensor infoAppTouchSensor;
extern TouchSensor execAppTouchSensor;
extern TouchSensor analogAppTouchSensor;
extern TouchSensor settingsAppTouchSensor;
extern TouchSensor pnesxAppTouchSensor;
extern TouchSensor snakeAppTouchSensor;

INT_T tcp_app (WORD_T size, void *argv);

static void control_init ();
static void init_adc_bat ();
static uint16_t measureBattery ();
void updateAudio (WaveSample **s, char *path);
static TouchPointTypeDef sp;
static INT_T SENSOR_THREAD (WORD_T size, void *argv);
WaveSample *clickWave;
WaveSample *confirmWave;



#endif  /*STARTUP_APP_H*/