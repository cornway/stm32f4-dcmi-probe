#include "cam_app.h"
#include "explorer_app.h"
#include "fgpu.h"
#include "ccpu.h"
#include "camera.h"
#include "adv7180_conf.h"
#include "device_conf.h"

TouchSensor camAppTouchSensor;
uint8_t backlightLock = 0;
CameraFrame cf;
static color_t textColor = RGB24_TO_RGB16(255, 255, 255);
static color_t screenColor = RGB24_TO_RGB16(30,144,255);
static color_t exitColor = RGB24_TO_RGB16(0, 0, 0);
static color_t hideColor = RGB24_TO_RGB16(0, 0, 0);
static color_t flipColor = RGB24_TO_RGB16(0, 0, 0);
static color_t fpsColor = RGB24_TO_RGB16(0, 0, 0);
static color_t openColor = RGB24_TO_RGB16(0, 0, 0);
static color_t resetColor = RGB24_TO_RGB16(0, 0, 0);
static color_t knobColor = RGB24_TO_RGB16(0, 0, 0);
static color_t infoColor = RGB24_TO_RGB16(0, 0, 0);

static GuiEngine<color_t, range_t, COLOR_WHITE> *engine;
static GContentPane<color_t, range_t, COLOR_WHITE> *pane,
                                                   *pane_nosignal;

static GDialog<color_t, range_t, COLOR_WHITE> *dialog;
static GAlert<color_t, range_t, COLOR_WHITE> *alert;


static GLabel<color_t, range_t, COLOR_WHITE>    *label_fps_info,
                                                *label_expand,
                                                *label_exit,
                                                *label_hide,
                                                *label_nosignal,
                                                *label_flip,
                                                *label_load,
                                                *label_reset;

static GProgressBar<color_t, range_t, COLOR_WHITE>  *pg_brightness,
                                                    *pg_contrast;

GTextField<color_t, range_t, COLOR_WHITE> *adv_info;


INT_T cam_app (WORD_T size, void *argv)
{
    if (camera_init() < 0) {
        return -1;
    }
    parse_conf();
    
    backlightLock = applicationControl.tftControl.backlightLock;
    applicationControl.tftControl.backlightLock = 1;
    
    camAppTouchSensor.clearEvent(); /*sprite ram init*/
    vm::lock(MEMORY_ALLOC_LOCK_ID);
    
    engine = new GuiEngine<color_t, range_t, COLOR_WHITE>(fontArray.font_mono, (color_t *)FRAME_MEMORY_BASE, 0, 0, TFT_WIDTH, TFT_HEIGHT);
    pane = engine->newContentPane(0, 0, TFT_WIDTH, TFT_HEIGHT);
    pane->mute(true);
    pane_nosignal = engine->newContentPane(0, 0, TFT_WIDTH, TFT_HEIGHT);
    pane_nosignal->mute(true);
    
    alert = pane->newAlert("WARNING !", 64, 200, 150);
    alert->setBackground(ALERT_BACK_COLOR);
    alert->setForeground(ALERT_TEXT_COLOR);
    alert->setKeyColor(ALERT_KEY_COLOR);
    
    dialog = pane->newDialog("ATTENTION !", 64, 200, 150);
    dialog->setBackground(ALERT_BACK_COLOR);
    dialog->setForeground(ALERT_TEXT_COLOR);
    dialog->setKeyColor(ALERT_KEY_COLOR);
    
    
    label_exit = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 6>
                ("exit", "EXIT", 0, 290, 120, 30);
    pane->addLabel(label_exit);
    label_exit->setBackground(exitColor);
    label_exit->setForeground(textColor);
    label_exit->setTextAllign(GTEXT_ALLIGN_CENTER);
    
    label_hide = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 5>
                ("hide", "HIDE", 120, 290, 120, 30);
    pane->addLabel(label_hide);
    label_hide->setBackground(hideColor);
    label_hide->setForeground(textColor);
    label_hide->setTextAllign(GTEXT_ALLIGN_CENTER);
    
    label_load = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 5>
                ("open", "OPEN", 0, 260, 120, 30);
    pane->addLabel(label_load);
    label_load->setBackground(openColor);
    label_load->setForeground(textColor);
    label_load->setTextAllign(GTEXT_ALLIGN_CENTER);
    
    label_reset = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 7>
                ("open", "RESET", 120, 260, 120, 30);
    pane->addLabel(label_reset);
    label_reset->setBackground(resetColor);
    label_reset->setForeground(textColor);
    label_reset->setTextAllign(GTEXT_ALLIGN_CENTER);
    
    label_flip = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 10>
                ("flip", "FLIP", 240, 290, 120, 30);
    pane->addLabel(label_flip);
    label_flip->setBackground(flipColor);
    label_flip->setForeground(textColor);
    label_flip->setTextAllign(GTEXT_ALLIGN_CENTER);
    
    label_fps_info = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 12>
                ("fps", "FPS", 360, 290, 120, 30);
    pane->addLabel(label_fps_info);
    label_fps_info->setBackground(fpsColor);
    label_fps_info->setForeground(textColor);
    label_fps_info->setTextAllign(GTEXT_ALLIGN_CENTER);
    label_fps_info->setSilent(true);
    
    auto label = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 12>
                ("brightness", "BRIGHTNESS", 0, 240, 200, 20);
    pane->addLabel(label);
    label->setBackground(infoColor);
    label->setForeground(textColor);
    label->setSilent(true);
    
    pg_brightness = pane->createComponent<GProgressBar<color_t, range_t, COLOR_WHITE>, 8>
                 ("pg_bri", "50%", 0, 220, 200, 20);
    pane->addProgressBar(pg_brightness);
    pg_brightness->setBackground(0x0);
    pg_brightness->setForeground(textColor);
    pg_brightness->setKnobColor(knobColor);
    pg_brightness->setTrackColor(COLOR_WHITE);
    pg_brightness->setMinValue(0);
    pg_brightness->setMaxValue(255);
    pg_brightness->setCurrentValue(127);
    pg_brightness->setSelectColor(0x001f);
    
    label = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 10>
                ("contrast", "CONTRAST", 0, 200, 200, 20);
    pane->addLabel(label);
    label->setBackground(infoColor);
    label->setForeground(textColor);
    label->setSilent(true);
    
    pg_contrast = pane->createComponent<GProgressBar<color_t, range_t, COLOR_WHITE>, 8>
                 ("pg_contr", "50%", 0, 180, 200, 20);
    pane->addProgressBar(pg_contrast);
    pg_contrast->setBackground(0x0);
    pg_contrast->setForeground(textColor);
    pg_contrast->setKnobColor(knobColor);
    pg_contrast->setTrackColor(COLOR_WHITE);
    pg_contrast->setMinValue(0);
    pg_contrast->setMaxValue(255);
    pg_contrast->setCurrentValue(127);
    pg_contrast->setSelectColor(0x001f);
    
    label_expand = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 16>
                ("expand", "MORE INFO ->", 0, 150, 200, 30);
    pane->addLabel(label_expand);
    label_expand->setBackground(infoColor);
    label_expand->setForeground(textColor);
    label_expand->setTextAllign(GTEXT_ALLIGN_CENTER);
    
    adv_info = pane->createComponent<GTextField<color_t, range_t, COLOR_WHITE>, 240>
    ("info", "", 240, 80, 200, 210);
    pane->addTextField(adv_info);
    adv_info->setBackground(infoColor);
    adv_info->setForeground(textColor);
    adv_info->setSilent(true);
    adv_info->setVisible(false);
    
    label_nosignal = pane_nosignal->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 28>
                ("nosignal", "NO SIGNAL");
    label_nosignal->setSize(pane_nosignal->x, pane_nosignal->y, pane_nosignal->w, pane_nosignal->h);
    pane_nosignal->addLabel(label_nosignal);
    label_nosignal->setTextAllign(GTEXT_ALLIGN_CENTER);
    label_nosignal->setBackground(screenColor);
    label_nosignal->setForeground(RGB24_TO_RGB16(255, 0, 7));
    
    uint32_t timeout = 0;
    cf.lineStart = 0;
    cf.frameStart = 50u;
    cf.lineWidth = 480;
    cf.frameHeight = 160;
    cf.memory = (uint32_t)VIDEO_FRAME_BASE;
    
    static uint16_t touchTimeout = 0;
    camAppTouchSensor.clearEvent(); 
    camAppTouchSensor.addListener([](abstract::Event e) -> void {
        if (pane->fireSensor(e.getSource(), e.getCause()) < 0) {
            if (e.getCause() == SENSOR_RELEASE) {
                pane->mute(false); 
            } else if (e.getCause() == SENSOR_HOLD) {
                uint32_t timeout = 0;
                while ((DCMIisBusy() == true) && (timeout++ < 1000000)) {}
                SNAPSHOT(ret, "camsnap.bmp");
                if (ret >= 0) {
                    pane->openAlert("Snapshot Saved\nSuccessfully !", nullptr);
                }
            } 
        }
        touchTimeout = 0;
    });
    
    label_exit->addListener([](abstract::Event e) -> void {
        if (e.getCause() == SENSOR_RELEASE) { 
            pane->openDialog("EXIT ?", [](abstract::Event e) -> void {
                appOnActionHook();
                applicationControl.tftControl.backlightLock = backlightLock;
                APP_CLEANUP(engine, camAppTouchSensor);
                camera_deinit();
                vm::exit(0);
            }, nullptr);
        }
    });
    
    label_expand->addListener([](abstract::Event e) -> void {
        if (e.getCause() == SENSOR_RELEASE) { 
            if (adv_info->isVisible() == true) {
                label_expand->setText("MORE INFO ->");
                adv_info->setVisible(false);
            } else {
                label_expand->setText("LESS INFO <-");
                adv_info->setVisible(true);
            }
        }
    });
    
    label_hide->addListener([](abstract::Event e) -> void {
        if (e.getCause() == SENSOR_RELEASE) {
            pane->mute(true);
        }
    });
    
    label_reset->addListener([](abstract::Event e) -> void {
        if (e.getCause() == SENSOR_RELEASE) {
            pane->openDialog("RESET \nADV and DCMI ?", [](abstract::Event e) -> void {
                camera_deinit();
                vm::lock(FILE_SYSTEM_LOCK_ID);
                vm::lock(MEMORY_ALLOC_LOCK_ID);
                vm::lock(SENSOR_LOCK_ID);
                
                adv7180.init(ADV_I2C_ADDRESS);
                
                vm::unlock(FILE_SYSTEM_LOCK_ID);
                vm::unlock(MEMORY_ALLOC_LOCK_ID);
                vm::unlock(SENSOR_LOCK_ID);
                camera_init();
            }, nullptr);
        }
    });
    
    
    label_load->addListener([](abstract::Event e) -> void {
        if (e.getCause() == SENSOR_RELEASE) {
            pane->openDialog("LOAD ADV\nCONF. SCRIPT ?", [](abstract::Event e) -> void {
                _XCALL(ret, expl_app, EXPLORER_APP_USE_FILTER, (void *)"ADV");
                if (ret != 0) {
                    pane->openAlert("LOAD\nFAILED !", nullptr);
                } else {
                    pane->openAlert("LOAD\nSUCCESSFUL !", nullptr);
                    appOnActionHook();
                }
            }, nullptr);
        }
    });
    
    pg_brightness->addListener([](abstract::Event e) -> void {
        pg_brightness->setText("");
        pg_brightness->printInt( (int)(pg_brightness->getPercentage() * 100.0f));
        adv7180.setBrightness(pg_brightness->getCurrentValue());
    });
    
    pg_contrast->addListener([](abstract::Event e) -> void {
        pg_contrast->setText("");
        pg_contrast->printInt( (int)(pg_contrast->getPercentage() * 100.0f));
        adv7180.setContrast(pg_contrast->getCurrentValue());
    });
    
    CAM_CFG cam_cfg = parseArgs(size, (char *)argv);
    

    static FG_CopyHandle fgCpy;
    fgCpy.dest.x = 0;
    fgCpy.dest.y = 0;
    fgCpy.dest.w = TFT_WIDTH;
    fgCpy.dest.h = TFT_HEIGHT;
    fgCpy.dest.leg = TFT_HEIGHT;
    fgCpy.src.x = 320;
    fgCpy.src.y = 0;
    fgCpy.src.w = 160;
    fgCpy.src.h = TFT_WIDTH;
    fgCpy.src.leg = TFT_WIDTH;
    fgCpy.scaleX = 1;
    fgCpy.scaleY = 1;
    fgCpy.mirrorX = 1;
    fgCpy.mirrorY = 0;
    fgCpy.swapN = 1;
    fgCpy.preprocess = 1;
    fgCpy.interlace = 1;
    fgCpy.mask = 0x0;
    
    static uint8_t screen_flip = 0;
    label_flip->addListener([](abstract::Event e) -> void {
        if (e.getCause() == SENSOR_RELEASE) {
            switch (screen_flip) {
                case 0 : 
                            screen_flip = 1;
                            fgCpy.mirrorX = 1;
                    break;
                case 1 :    
                            screen_flip = 2;
                            fgCpy.mirrorX = 0;
                            fgCpy.mirrorY = 1;
                    break;
                case 2 :
                            screen_flip = 3;
                            fgCpy.mirrorY = 1;
                            fgCpy.mirrorX = 1;
                    break;
                case 3 :
                            screen_flip = 0;
                            fgCpy.mirrorY = 0;
                            fgCpy.mirrorX = 0;
                    break;
                default :
                    break;
            }
        }
    });
    
    vm::unlock(MEMORY_ALLOC_LOCK_ID);
    
    static uint32_t mills, frame_count;
    mills = sys_time_milis;
	for (;;) { 
        vm::lock(FRAME_RENDER_LOCK_ID);
        if (adv7180.inLock ()) {
            gpu_wait();
            DCMIsnapshotStart(cf);
            label_fps_info->setText("FPS : ");
            label_fps_info->printInt(frame_count / ((sys_time_milis - mills) / 1000));
            if (adv_info->isVisible() == true) {
                adv_info->clearText();
                adv7180.printInfo(adv_info);
            }
            static uint16_t video_cy = 0;
            video_cy = (video_cy + CCPU_READ_REG(CCPU_READ_AMPL_CY)) >> 1;
            adv_info->print("\nLUMINANCE : ");
            adv_info->printInt(video_cy >> 8);
            timeout = 0;
            while ((DCMIisBusy() == true) && (timeout++ < 1000000)) {}
            if (timeout >= 1000000) {
                DCMIrestart();
                mills = 0;
                frame_count = 0;
                pane_nosignal->mute(false);
                label_nosignal->setText("NO SIGNAL : DCMI TIMEOUT");
            } else {
                frame_count++;
                gpu_wait();
                gpu_copy_2d(fgCpy);
                gpu_wait();
                fgCpy.dest.y ^= 1;
                pane_nosignal->mute(true);
            } 
        } else {
            mills = 0;
            frame_count = 0;
            pane_nosignal->setVisible(true);
            label_nosignal->setText("NO SIGNAL : LOST LOCK");
        }
        if (adv_info->isVisible() == false) {
            pane_nosignal->wakeUp();
            pane_nosignal->repaint(*engine->getViewPort());
        }
        pane->wakeUp();
        pane->repaint(*engine->getViewPort());
        vm::unlock(FRAME_RENDER_LOCK_ID);
        if (touchTimeout < 1000) {
            touchTimeout++;
        } else {
            pane->mute(true);
        }
        camAppTouchSensor.invokeEvent();
    }
     
    //APP_CLEANUP(engine);
    //return 0;
}

#include "time.h"
#include "camera.h"

static int camera_init ()
{   
    adv7180.powerUp();
    time::delay_ms(50);
    
    if (DCMIinit() < 0) {
        DCMIDeInit();
        return -1;
    }
    return 0;
}

static void camera_deinit ()
{ 
    adv7180.powerDown();
    
    DCMIDeInit();
}

static CAM_CFG parseArgs (int size, char *arg)
{
    CAM_CFG cam_cfg;
    cam_cfg.fpsReduced = false;
    cam_cfg.fps = 25;
    cam_cfg.channel = 0;
    cam_cfg.mode = 0;
    if (size == 0) {
        return cam_cfg;
    }
    /*TODO : remove whitespaces*/
    int index = 0;
    int argsPassed = 0;
    while (1) {
        if (arg[index++] != '-') {
            if (argsPassed > 0) {
                pane->openAlert("ARGS PASSED\nOK !", nullptr);
            }
            return cam_cfg;
        }
        if (arg[index++] != '-') {
            pane->openAlert("MISSING\n\'-\' KEY !", nullptr);
            return cam_cfg;
        }
        switch (arg[index++]) {
            case 'F' : /*set frame rate, need to find two decimal digits - 1..25, 0 - deprecated*/
                        if ((arg[index] == '0') && (arg[index + 1] == '0')) {
                            index++;
                            break;
                        }
                        if (arg[index] == '\0') {
                            pane->openAlert("WRONG\nARGUMENTS !", nullptr);
                            return cam_cfg;
                        }
                        if ((arg[index] <= '2') && (arg[index] >= '0')) {
                            cam_cfg.fps = (arg[index] - '0') * 10;
                            cam_cfg.fpsReduced = true;
                        } else {
                            pane->openAlert("MISSING\n 1-ST CHARACTER !\n(--F?X)", nullptr);
                            return cam_cfg;
                        }
                        index++;
                        if ((arg[index] <= '2') && (arg[index] >= '0')) {
                            cam_cfg.fps += arg[index] - '0';
                            argsPassed++;
                        } else {
                            cam_cfg.fps = 0;
                            cam_cfg.fpsReduced = false;
                            pane->openAlert("MISSING\n 2-ND CHARACTER !\n(--FX?)", nullptr);
                            return cam_cfg;
                        }
                        index++;
                break;
            case 'C' : /*set input channel - 0..2*/
                        if (arg[index++] != 'H') {
                            pane->openAlert("MISSING\n 1-ND CHARACTER !\n(--C?)", nullptr);
                            return cam_cfg;
                        }
                        if ((arg[index] >= '0') && (arg[index] <= '2')) {
                            cam_cfg.channel = arg[index] - '0';
                            argsPassed++;
                        } else {
                            pane->openAlert("MISSING\n CHANNEL NUM !\n(--CH?)", nullptr);
                            return cam_cfg;
                        }
                        index++;
                break;
            case 'M' : /*set mode - 0..2*/
                        if ((arg[index] >= '0') && (arg[index] <= '2')) {
                            cam_cfg.mode = arg[index] - '0';
                            argsPassed++;
                        } else {
                            pane->openAlert("MISSING\n MODE NUM !\n(--M?)", nullptr);
                            return cam_cfg;
                        }
                        index++;
                break;
            default :   pane->openAlert("MISSING\n ARGUMENT !", nullptr);
                        return cam_cfg;
                break;               
        }
    }
    return cam_cfg;
}


static void parse_conf ()
{
    FIL *file = new FIL;
    if (file == nullptr) {
        return;
    }
    FRESULT res = FR_OK;
    res = f_open(file, "boot.inf", FA_READ);
    if (res != FR_OK) {
        delete file;
        return;
    }
    //char buf[36];
    collect_color(file, "CAM_TEXT", &textColor);
    collect_color(file, "CAM_SCREEN", &screenColor);
    collect_color(file, "CAM_EXIT", &exitColor);
    collect_color(file, "CAM_HIDE", &hideColor);
    collect_color(file, "CAM_FLIP", &flipColor);
    collect_color(file, "CAM_FPS", &fpsColor);
    collect_color(file, "CAM_OPEN", &openColor);
    collect_color(file, "CAM_RESET", &resetColor);
    collect_color(file, "CAM_KNOB", &knobColor);
    collect_color(file, "CAM_INFO", &infoColor);
    
    f_close(file);
    delete file;
    
    return;
}
