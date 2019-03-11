#ifndef EXEC_APP_H
#define EXEC_APP_H

#include "app_macro.h"
#include "app_def.h"
#include "device_conf.h"
#include "vm.h"
#include "vmapi.h"
#include "gui.h"

extern FontArray fontArray;

INT_T exec_app (WORD_T size, void *argv);

static int check_password (char *password);


#endif  /*EXEC_APP_H*/