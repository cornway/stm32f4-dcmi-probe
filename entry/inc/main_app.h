#ifndef MAIN_APP_H
#define MAIN_APP_H

#include "app_macro.h"
#include "app_def.h"
#include "device_conf.h"
#include "vm.h"
#include "vmapi.h"
#include "gui.h"
#include "ff.h"
#include "wave_sample.h"
#include "adv7180.h"

extern FontArray fontArray;

extern ADV7180_VIDEO_DECODER adv7180;

extern INT_T cam_app (WORD_T size, void *argv);
extern INT_T expl_app (WORD_T size, void *argv);
extern INT_T info_app (WORD_T size, void *argv);
extern INT_T file_app (WORD_T size, void *argv);
extern INT_T exec_app (WORD_T size, void *argv);
extern INT_T touch_app (WORD_T size, void *argv);
extern INT_T settings_app (WORD_T size, void *argv);
extern INT_T snake_app (WORD_T size, void *argv);
extern INT_T analog_app (WORD_T size, void *argv);

extern void usbdStart (void);
extern void usbdStop (void);
extern void updateAudio (WaveSample **s, char *path);
extern int parse_clock (char *token, DAY_TIME *clock);
extern void clockTick (void (*hook) (void));
extern char *getClockText ();
extern WaveSample *clickWave;
extern WaveSample *confirmWave;
extern DAY_TIME clock;

static inline void update_descktop ();
static inline void repaint ();
static inline void refreshGraphic ();
static void update_sprites ();
static void clear_sprites ();
static void updateStartupSound ();
static void lUsbDisconnect (abstract::Event e);
static void lSystemRestart (abstract::Event e);


static int parse_conf ();

int snapshot_1bpp (char *name);
int snapshot_4bpp (char *name);
int collect_token (FIL *file, char *name, char *buf, int size, int tsize);
int collect_color (FIL *file, char *name, color_t *color);
int update_ipv4 (char *token, uint8_t *ip);
int update_mac (char *token, uint8_t *mac);

INT_T main_app (WORD_T size, void *argv);

#endif  /*MAIN_APP_H*/
