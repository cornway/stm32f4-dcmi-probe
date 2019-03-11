#ifndef EXEC_APP_H
#define EXEC_APP_H

#include "app_def.h"
#include "device_conf.h"
#include "vm.h"
#include "vmapi.h"
#include "tsc2046.h"

extern tsc2046Drv tsc2046;

INT_T touch_app (WORD_T size, void *argv);


#endif  /*EXEC_APP_H*/