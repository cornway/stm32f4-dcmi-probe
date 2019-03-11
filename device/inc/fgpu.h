#ifndef FGPU_H
#define FGPU_H

#ifdef __cplusplus
    extern "C" {
#endif

#include <stdint.h>
#include "device_conf.h"
        


typedef range_t r_t;
typedef color_t c_t;
typedef uint32_t p_t;
typedef uint16_t arg_t;
  
typedef struct {
    r_t x, y, w, h, leg;
} FG_Rect;
        
typedef struct {
    c_t mask;
    FG_Rect dest;
    FG_Rect src; 
    r_t scaleX, scaleY;
    uint8_t mirrorX, mirrorY, swapN;
    uint8_t preprocess;
    uint8_t interlace;
} FG_CopyHandle;

void gpu_reset (void);
void gpu_load_dest (FG_Rect rect, p_t p);
void gpu_load_src (FG_Rect rect, p_t p);
void gpu_update (FG_Rect rect);
void gpu_pix_2d (FG_Rect rect, c_t color);
void gpu_frect_2d (FG_Rect rect, c_t color);
void gpu_copy_2d (FG_CopyHandle handle);       
arg_t gpu_busy (void);
void gpu_wait (void);
void gpuWaitIrq (void);

#ifdef __cplusplus
    }
#endif
        
#endif /*FGPU_H*/

