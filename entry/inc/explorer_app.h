#ifndef EXPLORER_APP_H
#define EXPLORER_APP_H

#include "app_macro.h"
#include "app_def.h"
#include "device_conf.h"
#include "vm.h"
#include "vmapi.h"
#include "gui.h"
#include "adv7180.h"
#include "wave_sample.h"

extern FontArray fontArray;

INT_T expl_app (WORD_T size, void *argv);

static void update_sprites ();
static void clear_sprites ();
static void repaint ();
static void get_fs_info ();
static void rename ();
static int scan_dir_filter (int option, char *arg);
static int scan_dir (const char *path, char *ext);
static void getFileInfo (char *path);
static int checkFileExt (char *name, char *ext);
static void lOpenFile (abstract::Event e);
static void lDecline (abstract::Event e);




INT_T file_app (WORD_T size, void *argv);

extern ADV7180_VIDEO_DECODER adv7180;
extern void updateAudio (WaveSample **s, char *path);
extern WaveSample *clickWave;

static int getFileExtension (char *fileName, char *fileExtension);



#endif  /*EXPLORER_APP_H*/
