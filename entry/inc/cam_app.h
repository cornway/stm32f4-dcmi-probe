#ifndef CAM_APP_H
#define CAM_APP_H

#include "app_macro.h"
#include "app_def.h"
#include "device_conf.h"
#include "vm.h"
#include "vmapi.h"
#include "gui.h"
#include "adv7180.h"
#include "ff.h"

typedef struct {
    bool fpsReduced;
    uint8_t fps;
    uint8_t channel;
    uint8_t mode;
} CAM_CFG;

extern FontArray fontArray;
extern APPLICATION_CONTROL applicationControl;
extern ADV7180_VIDEO_DECODER adv7180;
extern uint32_t sys_time_milis;
extern int collect_color (FIL *file, char *name, color_t *color);
extern int snapshot (char *name);
extern int snapshot_4bpp (char *name);

INT_T cam_app (WORD_T size, void *argv);

static CAM_CFG parseArgs (int size, char *arg);
static int camera_init ();
static void camera_deinit ();
static void parse_conf ();

#endif  /*CAM_APP_H*/
