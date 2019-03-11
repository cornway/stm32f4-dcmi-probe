#ifndef PNESX_APP_H
#define PNESX_APP_H

#include "app_def.h"
#include "app_macro.h"
#include "vm.h"
#include "vmapi.h"
#include "device_conf.h"
#include "gui.h"
#include "fgpu.h"
#include "ff.h"

extern APPLICATION_CONTROL applicationControl;
extern FontArray fontArray;
extern int snapshot (char *name);
extern int collect_token (FIL *file, char *name, char *buf, int size, int tsize);

INT_T pnesx_app (WORD_T size, void *argv);

static void dmaInit ();
static void dmaDeInit ();
static void repaint ();
static void clearNesVram ();
static int parseScale (FG_CopyHandle *);

#endif  /*PNESX_APP_H*/
