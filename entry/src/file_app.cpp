#include "explorer_app.h"
#include "pnesx_app.h"
#include <string.h>
#include "adv7180.h"


INT_T file_app (WORD_T size, void *argv) 
{
    static char *fileName;
    static char fileExt[4];
    fileName = (char *)argv;
    if (getFileExtension(fileName, fileExt) != 0) {
        return UNSUPPORTED_FILE_TYPE;
    }
    if (strcmp(fileExt, "ADV") == 0) {
        vm::lock(MEMORY_ALLOC_LOCK_ID);
        vm::lock(FILE_SYSTEM_LOCK_ID);
        INT_T ret = 0;
        if (adv7180.setConfFromfile(fileName) == 0) {
            ret = ADV_CONF_OK;
        } else {
            ret = ADV_CONF_FAILED;
        }
        vm::unlock(MEMORY_ALLOC_LOCK_ID);
        vm::unlock(FILE_SYSTEM_LOCK_ID);
        return ret;
    } else if (strcmp(fileExt, "NES") == 0) {
        __XCALL(ret, 8192, pnesx_app, 0, (void *)fileName);
        if ((int)ret < 0) {
            return FILE_OPEN_ERROR;
        }
        return FILE_OPEN_OK;
    } else if (strcmp(fileExt, "WAV") == 0) {
        updateAudio(&clickWave, fileName);
        return FILE_OPEN_OK;
    } else {
        return UNSUPPORTED_FILE_TYPE;
    }
    return 0;
}


#if (_USE_LFN == 0)
    #define FILE_NAME_LEN   (8)
    #define FILE_EXT_LEN    (3)
#else
    #error "please use sfn mode of fatfs to operate!"
#endif

static int getFileExtension (char *fileName, char *fileExtension)
{
    uint32_t i = 0;
    while ((fileName[i] != '.') && (i < FILE_NAME_LEN + FILE_EXT_LEN + 1)) {
        i++;
    }
    if (i < FILE_NAME_LEN + FILE_EXT_LEN + 1) {
        i++;
        fileExtension[0] = fileName[i];
        fileExtension[1] = fileName[i + 1];
        fileExtension[2] = fileName[i + 2];
        fileExtension[3] = '\0';
    } else {
        return -1;
    }
    return 0;
}
