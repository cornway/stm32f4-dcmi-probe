#ifndef EXEC_INTERFACE_H
#define EXEC_INTERFACE_H

#include "VM.h"
#include "vmapi.h"
#include "fgpu.h"
#include "gui.h"
#include "app_def.h"
#include "camera.h"
#include "adv7180.h"

#include "device_conf.h"

int writeVideoDecoderRegFromStr (char *str);
int account_logon (char *str);
void account_exit ();

#endif  /*EXEC_INTERFACE_H*/


