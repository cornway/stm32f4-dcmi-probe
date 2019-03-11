#include "exec_interface.h"
#include <string.h>
extern ADV7180_VIDEO_DECODER adv7180;
int writeVideoDecoderRegFromStr (char *str)
{
    return adv7180.parseReg(str);
}

extern ACCESS_CONTROL globalAccessControl;
int account_logon (char *str)
{
    if (strcmp(str, globalAccessControl.password) == 0) {
        globalAccessControl.privilege = PRIVILEGE_ADMIN;
        return 0;
    }
    return -1;
}

void account_exit ()
{
    globalAccessControl.privilege = PRIVILEGE_USER;
}
