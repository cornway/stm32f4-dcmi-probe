#include "fgpu.h"
#include "ccpu.h"

void gpu_reset ()
{
    
}

void gpu_load_dest (FG_Rect rect, p_t p)
{
    CCPU_WRITE_REG(CCPU_WRITE_GPU_DEST_X, rect.x);
    CCPU_WRITE_REG(CCPU_WRITE_GPU_DEST_Y, rect.y);
	CCPU_WRITE_REG(CCPU_WRITE_GPU_DEST_W, rect.w);
    CCPU_WRITE_REG(CCPU_WRITE_GPU_DEST_H, rect.h);			
    CCPU_WRITE_REG(CCPU_WRITE_GPU_DEST_LEG, rect.leg);
    CCPU_WRITE_REG(CCPU_WRITE_GPU_DPL, p & 0xff); 
    CCPU_WRITE_REG(CCPU_WRITE_GPU_DPH, (p >> 8) & 0xff); 
    
}
void gpu_load_src (FG_Rect rect, p_t p)
{
    CCPU_WRITE_REG(CCPU_WRITE_GPU_SRC_X, rect.x); 
    CCPU_WRITE_REG(CCPU_WRITE_GPU_SRC_Y, rect.y);
    CCPU_WRITE_REG(CCPU_WRITE_GPU_SRC_W, rect.w); 
    CCPU_WRITE_REG(CCPU_WRITE_GPU_SRC_H, rect.h); 
    CCPU_WRITE_REG(CCPU_WRITE_GPU_SRC_LEG, rect.leg);
    CCPU_WRITE_REG(CCPU_WRITE_GPU_SPL, p & 0xff); 
    CCPU_WRITE_REG(CCPU_WRITE_GPU_SPH, (p >> 8) & 0xff); 
}

void gpu_update (FG_Rect rect)
{
    rect.leg = SCR_HEIGHT;
    gpu_load_src(rect, 0);
    CCPU_WRITE_REG(CCPU_WRITE_RENDER_CTL_REGISTER, CCPU_RND_UPDATE_BM);
}
void gpu_pix_2d (FG_Rect rect, c_t color)
{
    CCPU_WRITE_REG(CCPU_WRITE_GPU_COLOR, color);
    CCPU_WRITE_REG(CCPU_WRITE_GPU_DEST_X, rect.x);
    CCPU_WRITE_REG(CCPU_WRITE_GPU_DEST_Y, rect.y);
    CCPU_WRITE_REG(CCPU_WRITE_GPU_DEST_LEG, rect.leg);
	CCPU_WRITE_REG(CCPU_WRITE_PFILL_CTL, 0);
}
void gpu_frect_2d (FG_Rect rect, c_t color)
{
    gpu_load_dest(rect, 0);
    CCPU_WRITE_REG(CCPU_WRITE_GPU_COLOR, color);
	CCPU_WRITE_REG(CCPU_WRITE_FILL_CTL, CCPU_FILL_UPDATE_BM);
}

void gpu_copy_2d (FG_CopyHandle handle)
{ 
    gpu_load_dest(handle.dest, 0);
    gpu_load_src(handle.src, 0); 
    CCPU_WRITE_REG(CCPU_WRITE_GPU_SCALEX, handle.scaleX); 
    CCPU_WRITE_REG(CCPU_WRITE_GPU_SCALEY, handle.scaleY); 
    CCPU_WRITE_REG(CCPU_WRITE_GPU_COLOR, handle.mask); 
    arg_t setup =   CCPU_COPY_UPDATE_BM                                |
                    (handle.mirrorX ? CCPU_COPY_X_MIRROR_BM : 0)       |
                    (handle.mirrorY ? CCPU_COPY_Y_MIRROR_BM : 0)       |
                    (handle.preprocess ? CCPU_COPY_SRC_BM : 0)         |
                    (handle.swapN ? CCPU_COPY_SWAP_BM : 0)             |
                    (handle.interlace ? CCPU_COPY_INTERLACE_Y_BM : 0);
    CCPU_WRITE_REG(CCPU_WRITE_COPY_CTL, setup);
}

arg_t gpu_busy ()
{
    return CCPU_READ_REG(CCPU_READ_BUSY);
}




