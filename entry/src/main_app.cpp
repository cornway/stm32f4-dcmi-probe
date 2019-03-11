#include "main_app.h"
#include "wave.h"
#include "wave_sample.h"
#include "sprite.h"
#include "bitmap.h"

static SpriteRAM spriteRAM;
static BitmapFactory bmp;
static uint8_t bmp_dithering = 0;
static uint8_t bmp_alpha = 64;
static color_t text_color = 0;
static char clickWaveName[14];
static char confirmWaveName[14];

color_t batteryFullColor = RGB24_TO_RGB16(0, 128, 0);
color_t batteryEmptycolor = RGB24_TO_RGB16(128, 0, 0);
color_t batteryMetalColor = RGB24_TO_RGB16(128, 128, 128);
color_t batteryTextColor = RGB24_TO_RGB16(255, 255, 255);
color_t clockTextColor = RGB24_TO_RGB16(255, 255, 255);
color_t clockBackColor = RGB24_TO_RGB16(210,105,30);

ACCESS_CONTROL globalAccessControl = {PRIVILEGE_USER, DEFAULT_ADMIN_PASSWORD}; /*global access control struct*/
TouchSensor mainAppTouchSensor;

double PI_CONST = 3.1400000;
char adv_conf_file_name[24] = "";

void drawBattery (NonPalette<color_t, range_t, COLOR_WHITE> *component);
void drawClock (NonPalette<color_t, range_t, COLOR_WHITE> *component);

static GuiEngine<color_t, range_t, COLOR_WHITE> *engine;

static GContentPane<color_t, range_t, COLOR_WHITE> *pane;
static GAlert<color_t, range_t, COLOR_WHITE> *alert;

static GLabel<color_t, range_t, COLOR_WHITE>    *label_cam,
                                                *label_expl,
                                                *label_setup,
                                                *label_usb,
                                                *label_exec,
                                                *label_scope,
                                                *label_info,
                                                *label_reset,
                                                *label_touch_cal,
                                                *label_pnesx,
                                                *label_snake;

NonPalette<color_t, range_t, COLOR_WHITE> *clockPlotter;


INT_T main_app (WORD_T size, void *argv)
{
    mainAppTouchSensor.clearEvent();
    vm::lock(MEMORY_ALLOC_LOCK_ID);
    
    vm::lock(FILE_SYSTEM_LOCK_ID);
    vm::lock(SENSOR_LOCK_ID);
    parse_conf();
    updateAudio(&clickWave, clickWaveName); 
    updateAudio(&confirmWave, confirmWaveName); 
    vm::unlock(SENSOR_LOCK_ID);
    vm::unlock(FILE_SYSTEM_LOCK_ID);
    
    spriteRAM.init(SPRITE_MEMORY_BASE, SPRITE_MEMORY_SIZE);
    engine = new GuiEngine<color_t, range_t, COLOR_WHITE>(fontArray.font_mono, (color_t *)FRAME_MEMORY_BASE, 0, 0, TFT_WIDTH, TFT_HEIGHT);
    pane = engine->newContentPane(0, 0, TFT_WIDTH, TFT_HEIGHT);
    pane->setVisible(true);
    
    label_cam = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 12>
                ("camera", "Video", 0, 210, 120, 100);
    pane->addLabel(label_cam);
    label_cam->setOpaque(0);
    label_cam->setForeground(text_color);
    
    label_expl = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 12>
                ("explorer", "Explorer", 120, 210, 120, 100);
    pane->addLabel(label_expl);
    label_expl->setOpaque(0);
    label_expl->setForeground(text_color);
    
    label_setup = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 12>
                ("setup", "Setup", 240, 210, 120, 100);
    pane->addLabel(label_setup);
    label_setup->setOpaque(0);
    label_setup->setForeground(text_color);
    
    label_usb = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 4>
                ("usb", "Usb", 360, 210, 120, 100);
    pane->addLabel(label_usb);
    label_usb->setOpaque(0);
    label_usb->setForeground(text_color);
    
    label_exec = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 12>
                ("execute", "Execute", 0, 110, 120, 100);
    pane->addLabel(label_exec);
    label_exec->setOpaque(0);
    label_exec->setForeground(text_color);
    
    label_scope = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 8>
                ("analog", "Analog", 120, 110, 120, 100);
    pane->addLabel(label_scope);
    label_scope->setOpaque(0);
    label_scope->setForeground(text_color);
    
    label_info = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 5>
                ("info", "Info", 240, 110, 120, 100);
    pane->addLabel(label_info);
    label_info->setOpaque(0);
    label_info->setForeground(text_color);
    
    label_reset = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 6>
                ("reset", "Reset", 360, 110, 120, 100);
    pane->addLabel(label_reset);
    label_reset->setOpaque(0);
    label_reset->setForeground(text_color);
    
    label_touch_cal = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 7>
                ("calibration", "Sensor", 0, 10, 120, 100);
    pane->addLabel(label_touch_cal);
    label_touch_cal->setOpaque(0);
    label_touch_cal->setForeground(text_color);
    
    label_pnesx = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 7>
                ("pnesx", "Pnesx", 120, 10, 120, 100);
    pane->addLabel(label_pnesx);
    label_pnesx->setOpaque(0);
    label_pnesx->setForeground(text_color);
    
    label_snake = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 7>
                ("snake", "Snake", 240, 10, 120, 100);
    pane->addLabel(label_snake);
    label_snake->setOpaque(0);
    label_snake->setForeground(text_color);
    
    
    auto NonPalette = pane->createNonPaletteComponent(400, 27, 80, 20, drawBattery);
    pane->addNonPalette(NonPalette);
    
    clockPlotter = pane->createNonPaletteComponent(400, 5, 80, 20, drawClock);
    pane->addNonPalette(clockPlotter);
    clockPlotter->setForeground(clockTextColor);
    clockPlotter->setBackground(clockBackColor);
    
    alert = pane->newAlert("WARNING !", 64, 200, 150);
    alert->setBackground(ALERT_BACK_COLOR);
    alert->setForeground(ALERT_TEXT_COLOR);
    alert->setKeyColor(ALERT_KEY_COLOR);
    
    
     
    mainAppTouchSensor.addListener([](abstract::Event e) -> void {
            pane->fireSensor(e.getSource(), e.getCause());        
    });
    
    label_usb->addListener([](abstract::Event e) -> void {
        if (e.getCause() == SENSOR_RELEASE) {
            if (logon_once() == ACCOUNT_ACCESS_FAILURE) {
                refreshGraphic();
                return;
            }
            refreshGraphic();
            pane->openAlert("Usb Mass \nStorage Connected.\nDisconnect", lUsbDisconnect);
            appOnActionHook();
            vm::lock(USB_LOCK_ID);
            vm::lock(FILE_SYSTEM_LOCK_ID);
            usbdStart();
        }
    });
    
    
    label_pnesx->addListener([](abstract::Event e) -> void {
        if (e.getCause() == SENSOR_RELEASE) {
            appOnActionHook();
            _XCALL(ret, expl_app, EXPLORER_APP_USE_FILTER, (void *)"NES");
            refreshGraphic(); 
            mainAppTouchSensor.clearEvent();
        }
    });
    
    label_snake->addListener([](abstract::Event e) -> void {
        if (e.getCause() == SENSOR_RELEASE) {
            appOnActionHook();
            _XCALL(ret, snake_app, 0, nullptr);
            refreshGraphic(); 
            mainAppTouchSensor.clearEvent();
        }
    });
    
    label_exec->addListener([](abstract::Event e) -> void {
        if (e.getCause() == SENSOR_RELEASE) {
            appOnActionHook();
            _XCALL(ret, exec_app, EXEC_APP_EXECUTE_REQUEST, nullptr);
            refreshGraphic(); 
            mainAppTouchSensor.clearEvent();
        }
    });
    
    label_scope->addListener([](abstract::Event e) -> void {
        if (e.getCause() == SENSOR_RELEASE) {
            appOnActionHook();
            _XCALL(ret, analog_app, 0, nullptr);
            refreshGraphic(); 
            mainAppTouchSensor.clearEvent();
        }
    });
    
    label_cam->addListener([](abstract::Event e) -> void {
        if (e.getCause() == SENSOR_RELEASE) {
            appOnActionHook();
            _XCALL(ret, cam_app, 0, nullptr);
            refreshGraphic();
            mainAppTouchSensor.clearEvent();
        }
    });
    
    static char filePath[24] = "";
    label_expl->addListener([](abstract::Event e) -> void {
        if (e.getCause() == SENSOR_RELEASE) {
            appOnActionHook();
            _XCALL(ret, expl_app, EXPLORER_APP_NO_FILTER, (void *)filePath);
            refreshGraphic();
            mainAppTouchSensor.clearEvent();
        }
    });
    
    label_setup->addListener([](abstract::Event e) -> void {
        if (e.getCause() == SENSOR_RELEASE) {
            appOnActionHook();
            _XCALL(ret, settings_app, 0, nullptr);
            refreshGraphic();
            mainAppTouchSensor.clearEvent();
        }
    });
    
    label_info->addListener([](abstract::Event e) -> void {
        if (e.getCause() == SENSOR_RELEASE) {
            appOnActionHook();
            _XCALL(ret, info_app, 0, nullptr);
            refreshGraphic();
            mainAppTouchSensor.clearEvent();
        }
    });
    
    label_touch_cal->addListener([](abstract::Event e) -> void {
        if (e.getCause() == SENSOR_RELEASE) {
            if (logon_once() == ACCOUNT_ACCESS_FAILURE) {
                refreshGraphic();
                return;
            }
            appOnActionHook();
            _XCALL(ret, touch_app, 0, (void *)"tsc2046.cal");
            refreshGraphic();
            mainAppTouchSensor.clearEvent();
        }
    });
    
    label_reset->addListener([](abstract::Event e) -> void {
        if (e.getCause() == SENSOR_RELEASE) {
            appOnActionHook();
            pane->openAlert("REBOOT", lSystemRestart);            
        }
    });
    
    static char clockData[16];
    
    /*
    clockPlotter->addListener([](abstract::Event e) -> void {
        if (e.getCause() == SENSOR_RELEASE) {
            appOnActionHook();
            strcpy(clockData, (const char *)getClockText());
            _XCALL(ret, exec_app, EXEC_APP_EDIT_REQUEST | 16, clockData);     
            if (ret != 0) {
                pane->openAlert("Unknown clock\nData !", [] (abstract::Event e) -> void {
                    refreshGraphic(); 
                    mainAppTouchSensor.clearEvent();
                });
            } else {
                ret = parse_clock(clockData, &clock);
                if (ret != 0) {
                    pane->openAlert("Cannot Parse\nClock Data !",  [] (abstract::Event e) -> void {
                        refreshGraphic(); 
                        mainAppTouchSensor.clearEvent();
                    });
                }
            }
        }
    });
    */
    
    vm::unlock(MEMORY_ALLOC_LOCK_ID);
    
    update_descktop();
    #if (PLAY_STARTUP_SOUND == 1U)
        updateStartupSound();
    #endif
    update_sprites();
    pane->wakeUp();
    
    for (;;) {
        if (applicationControl.tftControl.backlight == TFT_BACKLIGHT_OFF) {
            vm::wait_event("wakeup");
        }
        clockTick([]() -> void { clockPlotter->wakeUp(); });
        mainAppTouchSensor.invokeEvent();
        repaint();
        vm::yield();
    }
    //APP_CLEANUP(engine);
    //return 0;
}

static inline void update_descktop ()
{
    vm::lock(FILE_SYSTEM_LOCK_ID);
    vm::lock(MEMORY_ALLOC_LOCK_ID);
    vm::lock(SENSOR_LOCK_ID);
    vm::lock(FRAME_RENDER_LOCK_ID);
    
    FIL *file = new FIL;
    if (file == nullptr) {
        vm::fault("enomem");
    }
    Bitmap *b = bmp.open("1.bmp", file);
    if (b != nullptr) {
        b->draw((void *)FRAME_MEMORY_BASE, 2, 0);
        bmp.close(b);
    }
    
    delete file;
    
    vm::unlock(FILE_SYSTEM_LOCK_ID);
    vm::unlock(MEMORY_ALLOC_LOCK_ID);
    vm::unlock(SENSOR_LOCK_ID);
    vm::unlock(FRAME_RENDER_LOCK_ID);
}

static inline void repaint ()
{
    vm::lock(FRAME_RENDER_LOCK_ID);
    pane->repaint(*engine->getViewPort());
    vm::unlock(FRAME_RENDER_LOCK_ID);
}

static inline void refreshGraphic ()
{
    update_descktop();
    clear_sprites();
    update_sprites();
    pane->wakeUp();
}

static void update_sprites ()
{
    vm::lock(FILE_SYSTEM_LOCK_ID);
    vm::lock(MEMORY_ALLOC_LOCK_ID);
    vm::lock(SENSOR_LOCK_ID);
    vm::lock(FRAME_RENDER_LOCK_ID);
    
    FIL *file = new FIL;
    if (file == nullptr) {
        vm::fault("enomem");
    }
    
    Bitmap *b = nullptr;
    Sprite *s = nullptr;
    
    GET_SPRITE(label_pnesx, s, b, "nespad.bmp", "nespad");
    GET_SPRITE(label_snake, s, b, "snakeico.bmp", "snakeico");
    GET_SPRITE(label_cam, s, b, "camico.bmp", "cam");
    GET_SPRITE(label_expl, s, b, "folder.bmp", "folder");
    GET_SPRITE(label_setup, s, b, "setting.bmp", "setting");
    GET_SPRITE(label_usb, s, b, "usb.bmp", "usb");
    GET_SPRITE(label_exec, s, b, "exec.bmp", "exec");
    GET_SPRITE(label_scope, s, b, "scope.bmp", "scope");
    GET_SPRITE(label_info, s, b, "info.bmp", "info");
    GET_SPRITE(label_reset, s, b, "exit.bmp", "exit");
    GET_SPRITE(label_touch_cal, s, b, "touch.bmp", "touch");
    GET_SPRITE(alert, s, b, "cancel.bmp", "cancel");
    
    delete file;
    
    vm::unlock(FILE_SYSTEM_LOCK_ID);
    vm::unlock(MEMORY_ALLOC_LOCK_ID);
    vm::unlock(SENSOR_LOCK_ID);
    vm::unlock(FRAME_RENDER_LOCK_ID);
}

static void clear_sprites ()
{
    spriteRAM.clear();
}

#include "time.h"

#if (PLAY_STARTUP_SOUND == 1U)
static void updateStartupSound ()
{
    vm::lock(FILE_SYSTEM_LOCK_ID);
    vm::lock(MEMORY_ALLOC_LOCK_ID);
    vm::lock(SENSOR_LOCK_ID);
    vm::lock(FRAME_RENDER_LOCK_ID);
    
    FIL *file = new FIL;
    if (file == nullptr) {
        return;
    }
    WAVE *wave = new WAVE("welcome.wav");
    if (wave == nullptr) {
        delete file;
        return;
    }
    if (wave->open(file) != WAVE_OK) {
        delete file;
        delete wave;
        return;
    }
    WAVE_DSC waveDsc = wave->getInfo();

    wave->loadSample(file, (uint8_t *)SPRITE_MEMORY_BASE);
    WaveSample *waveSample = new WaveSample("welcome", waveDsc, (uint8_t *)SPRITE_MEMORY_BASE);
    if (waveSample == nullptr) {
        delete file;
        delete wave;
        return;
    }
    delete file;
    delete wave;
    waveSample->play();
    time::delay_s(4);
    delete waveSample;
    
    vm::unlock(FILE_SYSTEM_LOCK_ID);
    vm::unlock(MEMORY_ALLOC_LOCK_ID);
    vm::unlock(SENSOR_LOCK_ID);
    vm::unlock(FRAME_RENDER_LOCK_ID);
}
#endif /*(PLAY_STARTUP_SOUND == 1U)*/

static void lUsbDisconnect (abstract::Event e)
{
    if (e.getCause() == SENSOR_RELEASE) {
            usbdStop();
            vm::unlock(USB_LOCK_ID);
            vm::unlock(FILE_SYSTEM_LOCK_ID);
            update_descktop();
            clear_sprites();
            update_sprites();
    }
}

static void lSystemRestart (abstract::Event e)
{
    if (e.getCause() == SENSOR_RELEASE) {
        vm::lock(FILE_SYSTEM_LOCK_ID);
        vm::lock(MEMORY_ALLOC_LOCK_ID);
        vm::lock(SENSOR_LOCK_ID);
        vm::lock(FRAME_RENDER_LOCK_ID);
        vm::reset();
    }    
}

static int logon_once ()
{
    int res = ACCOUNT_ACCESS_GRANTED_ONCE;
    if (globalAccessControl.privilege != PRIVILEGE_ADMIN) { 
        _XCALL(xret, exec_app, EXEC_APP_PASSWORD_REQUEST, nullptr);
        if (xret == 0) { /*invalid password or cancel*/
            res =  ACCOUNT_ACCESS_FAILURE;
        } else if (xret == 1) { /*access granted once*/
            
        } else { /*unknown*/
            res = ACCOUNT_ACCESS_FAILURE;
        }
    } else {
        res = ACCOUNT_ACCESS_GRANTED_FOREVER;
    } /*access granted forever*/
    return res;
}

int snapshot_1bpp (char *name)
{
    vm::lock(FILE_SYSTEM_LOCK_ID);
    vm::lock(MEMORY_ALLOC_LOCK_ID);
    vm::lock(SENSOR_LOCK_ID);
    vm::lock(FRAME_RENDER_LOCK_ID);
    
    static BITMAPFILEHEADER bmHeader;
    static BITMAPINFOHEADER bmInfo;
    
    FRESULT res = FR_OK;
    int ret = -1;
    FIL *file = new FIL;
    if (file != nullptr) {
        res = f_open(file, name, FA_WRITE | FA_CREATE_NEW);
        if (res == FR_EXIST) {
            res = f_unlink(name);
            res = f_open(file, name, FA_WRITE | FA_CREATE_NEW);
        }
        if (res == FR_OK) {
            
            int foffset = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD);
            int fsize = foffset + (TFT_WIDTH * TFT_HEIGHT) / 8;
            bmHeader.bfType[0] = 'B';
            bmHeader.bfType[1] = 'M';
            bmHeader.bfOffBits = foffset;
            bmHeader.bfSize = fsize;
            bmHeader.bfUnused[0] = 0;
            bmHeader.bfUnused[1] = 0;
            
            bmInfo.biSize = sizeof(BITMAPINFOHEADER) - sizeof(RGBQUAD);
            bmInfo.biBitCount = 1; /*monochrome*/
            bmInfo.biClrImportant = 2;
            bmInfo.biClrUsed = 2;
            bmInfo.biPlanes = 1;
            bmInfo.biCompression = 0;
            bmInfo.biSizeImage = 0;
            bmInfo.biWidth = TFT_HEIGHT;
            bmInfo.biHeight = TFT_WIDTH;
            bmInfo.biXRes = 0;
            bmInfo.biYRes = 0;
            
            RGBQUAD bmBlack = {0, 0, 0, 0};
            RGBQUAD bmWhite = {255, 255, 255, 0};
            bmInfo.biClrMap[0] = bmBlack;
            uint32_t f_bytes_write;
            res = f_write(file, &bmHeader, sizeof(BITMAPFILEHEADER), &f_bytes_write);
            if (res == FR_OK) {
                res = f_write(file, &bmInfo, sizeof(BITMAPINFOHEADER), &f_bytes_write);
                if (res == FR_OK) {
                    res = f_write(file, &bmWhite, sizeof(RGBQUAD), &f_bytes_write);
                    color_t *frame = (color_t *)FRAME_MEMORY_BASE;
                    uint8_t alpha = 0;
                    uint16_t weight = 0;
                    
                    int offset, numBytes = ((TFT_HEIGHT + 31) / 32) * 4;
                    uint8_t* bytes = new uint8_t[numBytes];

                    for(int y = TFT_WIDTH - 1; y >= 0; --y){
                        offset = y * TFT_HEIGHT;
                        
                        memset(bytes, 0, (numBytes * sizeof(uint8_t)));
                        
                        for(int x = 0; x < TFT_HEIGHT; ++x) {
                            GET_ALPHA_16BPP(alpha, frame[offset]);
                            weight += (alpha << 1) / (bmp_dithering + 1);
                            if (weight > bmp_alpha) {
                                bytes[x / 8] |= 1 << (7 - x % 8);
                                weight = 0;
                            }
                            offset++;
                        }
                        res = f_write(file, bytes, numBytes, &f_bytes_write);
                        if (res != FR_OK) {
                            break;
                        }
                    };
                    delete bytes; 
                }             
            }
            f_close(file);
                
            auto b = bmp.open(name, file);
            if (b != nullptr) {
                bmp.close(b);
                ret = 0;
            } 
        }
    }
    delete file;
    
    vm::unlock(FILE_SYSTEM_LOCK_ID);
    vm::unlock(MEMORY_ALLOC_LOCK_ID);
    vm::unlock(SENSOR_LOCK_ID);
    vm::unlock(FRAME_RENDER_LOCK_ID);
    return ret;
}

int snapshot_4bpp (char *name)
{
    vm::lock(FILE_SYSTEM_LOCK_ID);
    vm::lock(MEMORY_ALLOC_LOCK_ID);
    vm::lock(SENSOR_LOCK_ID);
    vm::lock(FRAME_RENDER_LOCK_ID);
    
    static BITMAPFILEHEADER bmHeader;
    static BITMAPINFOHEADER bmInfo;
    
    FRESULT res = FR_OK;
    int ret = -1;
    FIL *file = new FIL;
    if (file != nullptr) {
        res = f_open(file, name, FA_WRITE | FA_CREATE_NEW);
        if (res == FR_EXIST) {
            res = f_unlink(name);
            res = f_open(file, name, FA_WRITE | FA_CREATE_NEW);
        }
        if (res == FR_OK) {
            
            int foffset = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 15;
            int fsize = foffset + (TFT_WIDTH * TFT_HEIGHT) / 2;
            bmHeader.bfType[0] = 'B';
            bmHeader.bfType[1] = 'M';
            bmHeader.bfOffBits = foffset;
            bmHeader.bfSize = fsize;
            bmHeader.bfUnused[0] = 0;
            bmHeader.bfUnused[1] = 0;
            
            bmInfo.biSize = sizeof(BITMAPINFOHEADER) - sizeof(RGBQUAD);
            bmInfo.biBitCount = 4; /*4bpp*/
            bmInfo.biClrImportant = 16;
            bmInfo.biClrUsed = 16;
            bmInfo.biPlanes = 1;
            bmInfo.biCompression = 0;
            bmInfo.biSizeImage = 0;
            bmInfo.biWidth = TFT_HEIGHT;
            bmInfo.biHeight = TFT_WIDTH;
            bmInfo.biXRes = 0;
            bmInfo.biYRes = 0;
            
            RGBQUAD bmBlack = {0, 0, 0, 0};
            bmInfo.biClrMap[0] = bmBlack;
            uint32_t f_bytes_write;
            res = f_write(file, &bmHeader, sizeof(BITMAPFILEHEADER), &f_bytes_write);
            if (res == FR_OK) {
                res = f_write(file, &bmInfo, sizeof(BITMAPINFOHEADER), &f_bytes_write);
                if (res == FR_OK) {
                    for (int i = 0; i < 15; i++) {
                        bmBlack.r += 16;
                        bmBlack.g += 16;
                        bmBlack.b += 16;
                        res = f_write(file, &bmBlack, sizeof(RGBQUAD), &f_bytes_write);
                        if (res != FR_OK) {
                            break;
                        }
                    }
                    color_t *frame = (color_t *)FRAME_MEMORY_BASE;
                    uint8_t alpha = 0;
                    
                    int offset, numBytes = ((TFT_HEIGHT + 31) / 32) * 16;
                    uint8_t* bytes = new uint8_t[numBytes];

                    for(int y = TFT_WIDTH - 1; y >= 0; --y){
                        offset = y * TFT_HEIGHT;
                        memset(bytes, 0, (numBytes * sizeof(uint8_t)));
                        for(int x = 0; x < TFT_HEIGHT; ++x) {
                            GET_ALPHA_16BPP(alpha, frame[offset]);
                            bytes[x / 2] |= (alpha >> 4) << ((x % 2) == 0 ? 0 : 4);
                            offset++;
                        }
                        res = f_write(file, bytes, numBytes, &f_bytes_write);
                        if (res != FR_OK) {
                            break;
                        }
                    };
                    delete bytes; 
                }             
            }
            f_close(file);
                
            auto b = bmp.open(name, file);
            if (b != nullptr) {
                bmp.close(b);
                ret = 0;
            } 
        }
    }
    delete file;
    
    vm::unlock(FILE_SYSTEM_LOCK_ID);
    vm::unlock(MEMORY_ALLOC_LOCK_ID);
    vm::unlock(SENSOR_LOCK_ID);
    vm::unlock(FRAME_RENDER_LOCK_ID);
    return ret;
}

#include "pp.h"

static FIL *log_file = nullptr;
static bool log_opened = false;
static int open_log (char *name)
{
    log_file = new FIL;
    if (log_file == nullptr) {
        return -1;
    }
    FRESULT res = f_open(log_file, name, FA_CREATE_NEW | FA_WRITE);
    if (res == FR_EXIST) {
        res = f_unlink(name);
        if (res != FR_OK) {
            return -1;
        }
        res = f_open(log_file, name, FA_CREATE_NEW | FA_WRITE);
    }
    if (res != FR_OK) {
        return -1;
    }
    log_opened = true;
    return 0;
}

static void close_log (char *name)
{
    log_opened = false;
    f_close(log_file);
    delete log_file;
}

#define log_printf(str, args ...) if (log_opened == true) { f_printf(log_file, str, args); }
#define log_prints(str) if (log_opened == true) { f_puts( str, log_file); }

static int parse_conf ()
{
    FIL *file = new FIL;
    if (file == nullptr) {
        return -1;
    }
    FRESULT res = FR_OK;
    res = f_open(file, "boot.inf", FA_READ);
    if (res != FR_OK) {
        delete file;
        return -1;
    }
    open_log("dev.log");
    
    char buf[36];
    
    if (collect_token(file, "DITHER", buf, 36, 5) == 0) { 
        if (parse_hex8(buf, &bmp_dithering) < 0) {
            bmp_dithering = 32;
        } else {
            log_prints("Dither Get Ok!\n");
        }
    }
    
    if (collect_token(file, "ALPHA", buf, 36, 5) == 0) { 
        if (parse_hex8(buf, &bmp_alpha) < 0) {
            bmp_alpha = 32;
        } else {
            log_prints("Alpha Get Ok!\n");
        }

    }
    
    if (collect_token(file, "PASSWORD", buf, 36, PASSWORD_MAX_SIZE) == 0) { 
        strcpy(globalAccessControl.password, buf);
        log_prints("Password ok !\n");
    }
    
    if (collect_token(file, "PI_CONST", buf, 36, PASSWORD_MAX_SIZE) == 0) { 
        log_prints("Pi Const Found !\n");
        if (parse_double(buf, &PI_CONST) >= 0) {
            log_prints("Pi Const Parsed !\n");
        }
    }
    
    if (collect_token(file, "ADV_DEFAULT", buf, 36, PASSWORD_MAX_SIZE) == 0) { 
        log_prints("ADV Conf Found !\n");
        if (adv7180.setConfFromfile(buf) >= 0) {
            strcpy(adv_conf_file_name, (const char *)buf);
            log_prints("ADV Conf Ok !\n");
        }
    }
    
    if (collect_token(file, "SNAPSHOT", buf, 36, PASSWORD_MAX_SIZE) == 0) { 
        switch (buf[0]) {
            case '1' :  applicationControl.snapshotBpp = 1;
                        log_prints("Snapshot Bpp == 1\n");
                break;
            case '4' :  applicationControl.snapshotBpp = 4;
                        log_prints("Snapshot Bpp == 4\n");
                break;
            default :  log_prints("Snapshot Bpp Unsupported !\n");
                break;
        };
    }
    
    if (collect_token(file, "SOUND", buf, 36, PASSWORD_MAX_SIZE) == 0) { 
        if (strcmp(buf, "ON") == 0) {
            applicationControl.audioControl.soundOn = SOUND_ON;
        } else if (strcmp(buf, "OFF") == 0) {
            applicationControl.audioControl.soundOn = SOUND_OFF;
        } else {
            applicationControl.audioControl.soundOn = SOUND_OFF;
        }
    }
    
    
    
    collect_color(file, "DTEXT", &text_color);
    collect_color(file, "BFULL", &batteryFullColor);
    collect_color(file, "BEMPTY", &batteryEmptycolor);
    collect_color(file, "BMETAL", &batteryMetalColor);
    collect_color(file, "BTEXT", &batteryTextColor);
    collect_color(file, "CLKTEXT", &clockTextColor);
    collect_color(file, "CLKBACK", &clockBackColor);
    
    if (collect_token(file, "CONFIRM", buf, 36, 14) == 0) { 
        strcpy(confirmWaveName, buf);
        log_prints("Confirm Wave Sound ok !\n");
    } else {
        strcpy(confirmWaveName, "confirm.wav");
    }
    
    if (collect_token(file, "CLICK", buf, 36, 14) == 0) { 
        strcpy(clickWaveName, buf);
        log_prints("Click Wave Sound ok !\n");
    } else {
        strcpy(clickWaveName, "click.wav");
    }
    
    if (collect_token(file, "IPV4", buf, 36, 24) == 0) { 
        log_prints("IPV4 String Found !\n");
        int ret = update_ipv4(buf, applicationControl.networkControl.ipv4_ip);
        if (ret >= 0) {
            strcpy(applicationControl.networkControl.ipv4Str, buf);  
            log_prints("IPV4 String Parsed !\n");
        } else {
            log_printf("IPV4 String Parse Error %d !\n", ret);
        }
    } 
    
    if (collect_token(file, "MAC", buf, 36, 24) == 0) { 
        log_prints("MAC String Found !\n");
        int ret = update_mac(buf, applicationControl.networkControl.mac);
        if (ret >= 0) {
            strcpy(applicationControl.networkControl.macStr, buf);  
            log_prints("MAC String Parsed !\n");            
        }  else {
            log_printf("MAC String Parse Error %d !\n", ret);
        }
    }
    
    close_log("dev.log");
    
    f_close(file);
    delete file;
    
    return 0;
}

int collect_token (FIL *file, char *name, char *buf, int size, int tsize)
{
    int index = 0;
    int ret = -1;
    char *t_buf = new char[size]; /**/
    if (t_buf == nullptr) {
        return -1;
    }
    f_lseek(file, 0);
    char *buf_p = t_buf;
    while (1) {
        buf_p = f_gets(t_buf, size, file);
        if (buf_p != t_buf) {
            break;
        }
        int str_len = strlen(name);
        if (strncmp(buf_p, name, str_len) == 0) {
            buf_p += str_len;
            if (*(buf_p++) != '=') {
                break;
            }
            if (*(buf_p++) != '"') {
                break;
            }
            index = 0;
            while ((buf_p[index] != '"') && (index < tsize)) {
                buf[index] = buf_p[index];
                index++;
            }
            if (index == tsize) {
                break;
            }
            buf[index] = '\0';
            ret = 0;
            break;
        }
    }
    delete t_buf;
    return ret;
}

int collect_color (FIL *file, char *name, color_t *color)
{
    uint32_t r = 0, g = 0, b = 0;
    int ret = 0;
    char buf[36];
    if (collect_token(file, name, buf, 36, 24) == 0) { 
        ret = parse_struct(buf, 24, [&] (char *token, int size) -> int {
            if (*token =='\0') {
                return -1;
            }
            int ret = 0;
            switch (size) {
                case 0 :    ret = parse_uint32(token, &r);
                    break;
                case 1 :    ret = parse_uint32(token, &g);
                    break;
                case 2 :    ret = parse_uint32(token, &b);
                    break;
                default : return -1;
            };
            if (ret < 0) {
                return -1;
            }
            return 0;
        });
    } else {
        return -1;
    }
    if (ret < 0) {
        r = 0;
        b = 0;
        g = 0;
        return -1;
    } else {
        *color = RGB24_TO_RGB16(r, g, b);
    }
    return 0;
}

int update_ipv4 (char *token, uint8_t *ip)
{
    uint32_t _ip[4];
    int ret = -1;
    ret = parse_int_enum(token, 4, '.', [&] (char *s, int num) -> int {
        if (num > 3) {
            return -1;
        }
        if (parse_uint32(s, _ip + num) < 0) {
            return -1;
        }  
        return 0;
    });
    if (ret >= 0) {
        ip[0] = _ip[0];
        ip[1] = _ip[1];
        ip[2] = _ip[2];
        ip[3] = _ip[3];
    }
    return ret;
}

int update_mac (char *token, uint8_t *mac)
{
    uint32_t _mac[6];
    int ret = -1;
    ret = parse_int_enum(token, 4, '.', [&] (char *s, int num) -> int {
        if (num > 5) {
            return -1;
        }
        if (parse_uint32(s, _mac + num) < 0) {
            return -1;
        }  
        return 0;
    });
    if (ret >= 0) {
        mac[0] = _mac[0];
        mac[1] = _mac[1];
        mac[2] = _mac[2];
        mac[3] = _mac[3];
        mac[4] = _mac[4];
        mac[5] = _mac[5];
    }
    return ret;
}
