#include "pnesx_app.h"
#include "pnesx.h"
#include "time.h"

TouchSensor pnesxAppTouchSensor;

bool soundOn = false;
bool fullScreen = false;

TouchPointTypeDef touch_point;
FG_CopyHandle fgCpy;

static GuiEngine<color_t, range_t, COLOR_WHITE> *engine;

static GContentPane<color_t, range_t, COLOR_WHITE> *pane;

static GDialog<color_t, range_t, COLOR_WHITE> *dialog;
static GAlert<color_t, range_t, COLOR_WHITE> *alert;

static GLabel<color_t, range_t, COLOR_WHITE>    *label_exit,
                                                *label_restart,
                                                *label_info,
                                                *label_snapshot,
                                                *label_nes,
                                                *label_fullscreen;



void nes_frame_update ()
{
    repaint();
}

static void mute (bool _mute);

INT_T pnesx_app (WORD_T size, void *argv)
{
    time::delay_ms(50);
    pnesxAppTouchSensor.clearEvent();
    dmaInit();
    
    vm::lock(MEMORY_ALLOC_LOCK_ID);
    
    engine = new GuiEngine<color_t, range_t, COLOR_WHITE>(fontArray.font_mono, (color_t *)FRAME_MEMORY_BASE, 0, 0, TFT_WIDTH, TFT_HEIGHT);
    pane = engine->newContentPane(0, 0, TFT_WIDTH, TFT_HEIGHT);
    pane->setVisible(true);
    
    dialog = pane->newDialog("ATTENTION !", 64, 200, 150);
    dialog->setBackground(ALERT_BACK_COLOR);
    dialog->setForeground(ALERT_TEXT_COLOR);
    dialog->setKeyColor(ALERT_KEY_COLOR);
    
    alert = pane->newAlert("WARNING !", 64, 200, 150);
    alert->setBackground(ALERT_BACK_COLOR);
    alert->setForeground(ALERT_TEXT_COLOR);
    alert->setKeyColor(ALERT_KEY_COLOR);
    
    label_nes = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 0>
    ("nes", "", 0, 0, 280, 260);
    pane->addLabel(label_nes);
    label_nes->setOrigins(pane->getOrigins());
    label_nes->setOpaque(0);
    label_nes->setSilent(true);
    label_nes->setVisible(true);
    pane->setForeground(RGB24_TO_RGB16(255, 255, 255));
    label_nes->setMaskColor(0);
    
    tImage<color_t> nesFrame;
    nesFrame.Image = (color_t *)NES_VRAM_START;
    nesFrame.W = NES_DISP_WIDTH;
    nesFrame.H = NES_DISP_HEIGHT;
    label_nes->setIcon(&nesFrame);
    
    label_exit = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 5>
    ("exit", "EXIT", 0, 0, 60, 20);
    pane->addLabel(label_exit);
    label_exit->setBackground(RGB24_TO_RGB16(32, 32, 128));
    label_exit->setForeground(RGB24_TO_RGB16(0,128,128));
    
    label_restart = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 8>
    ("exit", "Restart", 60, 0, 80, 20);
    pane->addLabel(label_restart);
    label_restart->setColorProperties(label_exit);
    
    label_fullscreen = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 8>
    ("fullcreen", "FullScr", 140, 0, 100, 20);
    pane->addLabel(label_fullscreen);
    label_fullscreen->setColorProperties(label_restart);
    
    label_info = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 5>
    ("exit", "INFO", 240, 0, 120, 20);
    pane->addLabel(label_info);
    label_info->setColorProperties(label_restart);
    
    label_snapshot = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 5>
    ("snapshot", "Save BMP", 360, 0, 120, 20);
    pane->addLabel(label_snapshot);
    label_snapshot->setColorProperties(label_restart);
    
    pnesxAppTouchSensor.addListener([](abstract::Event e) -> void {
            if (pane->fireSensor(e.getSource(), e.getCause()) < 0) {
                touch_point = *(TouchPointTypeDef *)e.getSource();
            }                
            
    });
    
    label_fullscreen->addListener([](abstract::Event e) -> void {
            if (e.getCause() == SENSOR_CLICK) {
               if (fullScreen == true) {
                    pane->fill();
                    pane->wakeUp();
                    mute(false);
                    fullScreen = false;
               } else {
                   if (applicationControl.graphicControl.acceleration == true) {
                        fullScreen = true;
                        mute(true);
                   } else {
                        pane->openAlert("TURN ON HARDWARE\nACCELERATION\nBEFORE USE\nFULL SCREEN\nMODE !", nullptr);
                   }
               }
           }
    });
     
    label_exit->addListener([](abstract::Event e) -> void {
        if (e.getCause() == SENSOR_RELEASE) {
            pane->openDialog("EXIT ?", [](abstract::Event e) -> void {
                pNesX_Fin();
                dmaDeInit();
                vm::unlock(FRAME_RENDER_LOCK_ID);
                applicationControl.audioControl.soundOn = soundOn;
                appOnActionHook();
                APP_CLEANUP(engine, pnesxAppTouchSensor);
                vm::exit(0);
            }, nullptr);
        }
    });
    
    label_restart->addListener([](abstract::Event e) -> void {
        if (e.getCause() == SENSOR_RELEASE) {
            pane->openDialog("RESTART ?", [](abstract::Event e) -> void {
                appOnActionHook();
                pNesX_Reset();
            }, nullptr);
        }
    });
    
    label_snapshot->addListener([](abstract::Event e) -> void {
        if (e.getCause() == SENSOR_RELEASE) {
            SNAPSHOT(ret, "Nes.bmp");
            if (ret >= 0) {
                appOnActionHook();
                pane->openAlert("Snapshot Saved\nSuccessfully !", nullptr);
            }
        }
    });
    
    static char *rom_name;
    rom_name = (char *)argv;
    label_info->addListener([](abstract::Event e) -> void {
        if (e.getCause() == SENSOR_RELEASE) {
            pane->openAlert("ROM NAME :\n", nullptr);
            alert->apendText(rom_name);
        }
    });
    
    vm::unlock(MEMORY_ALLOC_LOCK_ID);
    
    int ret = 0;
    
    vm::lock(FILE_SYSTEM_LOCK_ID);
    vm::lock(MEMORY_ALLOC_LOCK_ID);
    vm::lock(SENSOR_LOCK_ID);
    
    ret = pNesX_Load((char *)argv);
    
    if (ret >= 0) {
        if (parseScale(&fgCpy) < 0) {
            fgCpy.scaleX = 0x101;
            fgCpy.scaleY = 0x40b;
        }
    }
    
    vm::unlock(SENSOR_LOCK_ID);
    vm::unlock(FILE_SYSTEM_LOCK_ID);
    vm::unlock(MEMORY_ALLOC_LOCK_ID);
    
    if (ret < 0) {
        dmaDeInit();
        APP_CLEANUP(engine, pnesxAppTouchSensor);
        return ret;
    }
    
    soundOn = applicationControl.audioControl.soundOn;
    applicationControl.audioControl.soundOn = false;
    
	ApuInit();
    pNesX_Init();
    
    fgCpy.dest.x = 0;
    fgCpy.dest.y = 20;
    fgCpy.dest.w = TFT_WIDTH;
    fgCpy.dest.h = TFT_HEIGHT - 20;
    fgCpy.dest.leg = TFT_HEIGHT;
    fgCpy.src.x = 610;
    fgCpy.src.y = 0;
    fgCpy.src.w = NES_DISP_HEIGHT;
    fgCpy.src.h = NES_DISP_WIDTH;
    fgCpy.src.leg = NES_DISP_WIDTH;
    fgCpy.mirrorX = 0;
    fgCpy.mirrorY = 0;
    fgCpy.swapN = 1;
    fgCpy.preprocess = 0;
    fgCpy.interlace = 0;
    fgCpy.mask = 0x0;
    
    vm::lock(FRAME_RENDER_LOCK_ID);
    
    clearNesVram();
    pane->fill();
    pane->repaint(*engine->getViewPort());
    
    while(1) {
        if (dialog->isOpened() == false) {
            pNesX_Cycle();
        } else {
            repaint();
        }
        pnesxAppTouchSensor.invokeEvent();
    }
}

static void repaint ()
{
    if (fullScreen == true) {
        gpu_wait();
        gpu_copy_2d(fgCpy);
        gpu_wait();
    } else {
        pane->wakeUp();
        pane->repaint(*engine->getViewPort());
    }
    FG_Rect fgRect = {0, 0, TFT_WIDTH, TFT_HEIGHT};
    gpu_wait();
    gpu_update(fgRect);
    gpu_wait();
}

static void clearNesVram ()
{
    color_t *dest = (color_t *)NES_VRAM_START;
    for (int i = 256 * 300; i >= 0; i--) {
        dest[i] == 0;
    }
}

#include "pp.h"

static int parseScale (FG_CopyHandle *handle)
{
    static uint16_t scale_x = 0, scale_y = 0;
    FRESULT res = FR_OK;
    FIL *file = new FIL;
    if (file == nullptr) {
        return -1;
    }
    res = f_open(file, "boot.inf", FA_READ);
    if (res != FR_OK) {
        delete file;
        return -1;
    }
    char buf[36];
    int ret = 0;
    if (collect_token(file, "NES_SCALE", buf, 36, 24) == 0) { 
        ret = parse_struct(buf, 24, [&] (char *token, int size) -> int {
            if (*token =='\0') {
                return -1;
            }
            int ret = 0;
            switch (size) {
                case 0 :    ret = parse_hex16(token, &scale_x);
                    break;
                case 1 :    ret = parse_hex16(token, &scale_y);
                    break;
                default : return -1;
            };
            if (ret < 0) {
                return -1;
            }
            return 0;
        });
    }
    if (ret >= 0) {
        handle->scaleX = scale_x;
        handle->scaleY = scale_y;
    }
    
    f_close(file);
    delete file;
    
    return ret;
}

static void mute (bool _mute)
{
    label_exit->setSilent(_mute);
    label_restart->setSilent(_mute);
    label_info->setSilent(_mute);
    label_snapshot->setSilent(_mute);
    if (_mute == true) {
        label_exit->setGlow(150);
        label_restart->setGlow(150);
        label_info->setGlow(150);
        label_snapshot->setGlow(150);
        label_exit->wakeUp();
        label_restart->wakeUp();
        label_info->wakeUp();
        label_snapshot->wakeUp();
    } else {
        label_exit->setGlow(0);
        label_restart->setGlow(0);
        label_info->setGlow(0);
        label_snapshot->setGlow(0);
    }
}


#include "stm32f4xx_hal.h"

DMA_HandleTypeDef dma222comHandle;

static void dmaInit ()
{
  __DMA2_CLK_ENABLE();
  dma222comHandle.Init.Channel             = DMA_CHANNEL_2;
  dma222comHandle.Init.Direction           = DMA_MEMORY_TO_MEMORY;
  dma222comHandle.Init.PeriphInc           = DMA_PINC_ENABLE;
  dma222comHandle.Init.MemInc              = DMA_MINC_ENABLE;
  dma222comHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
  dma222comHandle.Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;
  dma222comHandle.Init.Mode                = DMA_NORMAL;
  dma222comHandle.Init.Priority            = DMA_PRIORITY_HIGH;
  dma222comHandle.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;         
  dma222comHandle.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
  dma222comHandle.Init.MemBurst            = DMA_MBURST_SINGLE;
  dma222comHandle.Init.PeriphBurst         = DMA_PBURST_SINGLE; 

  dma222comHandle.Instance = DMA2_Stream2;  
  
  if (HAL_DMA_Init(&dma222comHandle) != HAL_OK) {
      for (;;) {}
  }
}

static void dmaDeInit ()
{
    HAL_DMA_DeInit(&dma222comHandle);
}
