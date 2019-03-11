#include "touch_app.h"

INT_T touch_app (WORD_T size, void *argv)
{
    int cal_res = 0;
    vm::lock(MEMORY_ALLOC_LOCK_ID);
    vm::lock(FRAME_RENDER_LOCK_ID);
    vm::lock(SENSOR_LOCK_ID);
    
    cal_res = tsc2046.calibration((char *)argv);
    
    vm::unlock(SENSOR_LOCK_ID);
    vm::unlock(MEMORY_ALLOC_LOCK_ID);
    vm::unlock(FRAME_RENDER_LOCK_ID);
    
    return cal_res;
}


