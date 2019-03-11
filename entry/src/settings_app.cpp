#include "settings_app.h"
#include "exec_app.h"
#include "sprite.h"
#include "bitmap.h"


TouchSensor settingsAppTouchSensor;

static SpriteRAM spriteRAM;
static BitmapFactory bmp;

bool changesApplied = true;
bool changesSaved = true;
static APPLICATION_CONTROL localAppControl;
static APPLICATION_CONTROL tempLocalAppControl;


static GuiEngine<color_t, range_t, COLOR_WHITE> *engine;

static GContentPane<color_t, range_t, COLOR_WHITE> *pane;

static GDialog<color_t, range_t, COLOR_WHITE> *dialog;
static GAlert<color_t, range_t, COLOR_WHITE> *alert;

static GLabel<color_t, range_t, COLOR_WHITE>    *label_save,
                                                *label_load,
                                                *label_exit,
                                                *label_up,
                                                *label_down,
                                                *label_left,
                                                *label_right,
                                                *label_edit,
                                                *label_info,
                                                *label_apply,
                                                *label_revert;

GBrowser<color_t, range_t, COLOR_WHITE> *item_browser;



INT_T settings_app (WORD_T size, void *argv)
{
    settingsAppTouchSensor.clearEvent();
    vm::lock(MEMORY_ALLOC_LOCK_ID);
    
    spriteRAM.init(SPRITE_MEMORY_BASE, SPRITE_MEMORY_SIZE);
    engine = new GuiEngine<color_t, range_t, COLOR_WHITE>(fontArray.font_mono, (color_t *)FRAME_MEMORY_BASE, 0, 0, TFT_WIDTH, TFT_HEIGHT);
    pane = engine->newContentPane(0, 0, TFT_WIDTH, TFT_HEIGHT);
    pane->setVisible(true);
    
    alert = pane->newAlert("WARNING !", 64, 200, 150);
    alert->setBackground(ALERT_BACK_COLOR);
    alert->setForeground(ALERT_TEXT_COLOR);
    alert->setKeyColor(ALERT_KEY_COLOR);
    
    dialog = pane->newDialog("ATTENTION !", 64, 200, 150);
    dialog->setBackground(ALERT_BACK_COLOR);
    dialog->setForeground(ALERT_TEXT_COLOR);
    dialog->setKeyColor(ALERT_KEY_COLOR);
    
    label_save = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 3>
                ("save", "", 0, 280, 40, 40);
    pane->addLabel(label_save);
    label_save->setForeground(RGB24_TO_RGB16(255, 0, 0));
    label_save->setBackground(RGB24_TO_RGB16(188,143,143));
    
    label_load = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 0>
                ("load", "", 40, 280, 40, 40);
    pane->addLabel(label_load);
    label_load->setForeground(COLOR_RED);
    label_load->setBackground(RGB24_TO_RGB16(188,143,143));
    
    label_exit = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 0>
                ("exit", "", 80, 280, 40, 40);
    pane->addLabel(label_exit);
    label_exit->setForeground(COLOR_RED);
    label_exit->setBackground(RGB24_TO_RGB16(188,143,143));
    
    label_up = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 0>
    ("up", "", 80, 250, 40, 30);
    pane->addLabel(label_up);
    
    label_down = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 0>
    ("down", "", 80, 0, 40, 30);
    pane->addLabel(label_down);
    label_down->setImageOr(2);
    
    label_left = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 3>
    ("left", "<<", 0, 210, 40, 40);
    pane->addLabel(label_left);
    label_left->setSilent(false);
    label_left->setForeground(COLOR_RED);
    label_left->setBackground(RGB24_TO_RGB16(188,143,143));
    
    label_right = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 3>
    ("right", ">>", 40, 210, 40, 40);
    pane->addLabel(label_right);
    label_right->setSilent(false);
    label_right->setForeground(COLOR_RED);
    label_right->setBackground(RGB24_TO_RGB16(188,143,143));
    
    label_apply = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 3>
    ("up", "", 0, 170, 40, 40);
    pane->addLabel(label_apply);
    label_apply->setForeground(RGB24_TO_RGB16(255, 0, 0));
    
    label_revert = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 0>
    ("up", "", 40, 170, 40, 40);
    pane->addLabel(label_revert);
    
    
    item_browser = pane->createComponent<GBrowser<color_t, range_t, COLOR_WHITE>, 12>
    ("list", "SETTINGS", 120, 0, 360, 320);
    pane->addBrowser(item_browser);
    item_browser->setBackground(0x00);
    item_browser->setForeground(COLOR_WHITE);
    item_browser->setHeaderColor(0x001f);
    item_browser->setSelectColor(RGB24_TO_RGB16(255, 140, 0));
    
    label_info = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 48>
                ("info", "", 0, 0, 80, 170);
    pane->addLabel(label_info);
    label_info->setForeground(COLOR_RED);
    
    label_edit = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 6>
                ("edit", "EDIT", 0, 250, 80, 30);
    pane->addLabel(label_edit);
    label_edit->setTextAllign(GTEXT_ALLIGN_CENTER);
    label_edit->setForeground(RGB24_TO_RGB16(176,196,222));
    label_edit->setBackground(RGB24_TO_RGB16(188,143,143));
    label_edit->setSilent(true);
    
    pane->addPad(0xffff, 80, 30, 40, 220);
    
    settingsAppTouchSensor.addListener([](abstract::Event e) -> void {
            pane->fireSensor(e.getSource(), e.getCause());         
    });
    
    label_exit->addListener([](abstract::Event e) -> void {
        if (e.getCause() == SENSOR_RELEASE) {
            if (e.getCause() == SENSOR_RELEASE) { 
               if (changesApplied == false) {
                    pane->openDialog("APPLY BEFORE\nEXIT ?", [](abstract::Event e) -> void {
                        pane->openAlert("APPLY\nSUCCESSFUL !", [](abstract::Event e) -> void {
                            appOnActionHook();
                            save_local_cfg(&applicationControl, &localAppControl);
                            APP_CLEANUP(engine, settingsAppTouchSensor);
                            vm::exit(0);
                        });
                    }, [](abstract::Event e) -> void {
                        pane->openDialog("EXIT ?", [](abstract::Event e) -> void {
                            appOnActionHook();
                            APP_CLEANUP(engine, settingsAppTouchSensor);
                            vm::exit(0);
                        }, nullptr );
                   });
               } else {
                    pane->openDialog("EXIT ?", [](abstract::Event e) -> void {
                        appOnActionHook();
                        APP_CLEANUP(engine, settingsAppTouchSensor);
                        vm::exit(0);
                    }, nullptr );
               }
           } 
                
        }
    });
    
    label_apply->addListener([](abstract::Event e) -> void {
        if (e.getCause() == SENSOR_RELEASE) {
            if (e.getCause() == SENSOR_RELEASE) { 
                pane->openDialog("APPLY\nCHANGES ?", [](abstract::Event e) -> void {
                    pane->openAlert("APPLY\nSUCCESSFUL !", [](abstract::Event e) -> void {
                        save_local_cfg(&tempLocalAppControl, &applicationControl);
                        save_local_cfg(&applicationControl, &localAppControl);
                        setChangesSaved(false);
                        setChangesApplied(true);
                        appOnActionHook();
                    });
                }, nullptr );
            }
        }
    });
    
    label_revert->addListener([](abstract::Event e) -> void {
        if (e.getCause() == SENSOR_RELEASE) {
            if (e.getCause() == SENSOR_RELEASE) { 
                pane->openDialog("REVERT\nCHANGES ?", [](abstract::Event e) -> void {
                    pane->openAlert("REVERT\nSUCCESSFUL !", [](abstract::Event e) -> void {
                        save_local_cfg(&applicationControl, &tempLocalAppControl);
                        load_local_cfg(&localAppControl);
                        setChangesSaved(false);
                        setChangesApplied(true);
                        appOnActionHook();
                    });
                }, nullptr );
            }
        }
    });
    
    
    label_up->addListener([](abstract::Event e) -> void {
        if ((e.getCause() == SENSOR_RELEASE) || (e.getCause() == SENSOR_HOLD)) {
            item_browser->moveDown();
            checkEditable();
        }
    });
    
    label_down->addListener([](abstract::Event e) -> void {
        if ((e.getCause() == SENSOR_RELEASE) || (e.getCause() == SENSOR_HOLD)) {
            item_browser->moveUp();
            checkEditable();
        }
    });
    
    label_left->addListener([](abstract::Event e) -> void {
        if (e.getCause() == SENSOR_RELEASE) {
            if (change_local_param(&localAppControl, -1) == 0) {
                setChangesApplied(false);
            }
        }
    });
    
    label_right->addListener([](abstract::Event e) -> void {
        if (e.getCause() == SENSOR_RELEASE) {
            if (change_local_param(&localAppControl, 1) == 0) {
                setChangesApplied(false);
            }
        }
    });
    
    label_edit->addListener([](abstract::Event e) -> void {
        if (e.getCause() == SENSOR_RELEASE) {
            edit(&localAppControl);
            setChangesApplied(false);
        }
    });
    
    label_save->addListener([](abstract::Event e) -> void {
        if (e.getCause() == SENSOR_RELEASE) { 
            pane->openDialog("SAVE\nSETTINGS ?", [](abstract::Event e) -> void {
                if (changesApplied == true) {
                    if (save_config("device.cfg") == 0) {
                        setChangesSaved(true);
                        appOnActionHook();
                    }
                } else {
                    pane->openAlert("APPLY\nCHANGES\nBEFORE SAVE !", nullptr);
                }
            }, nullptr);
        }
    });
    
    label_load->addListener([](abstract::Event e) -> void {
        if (e.getCause() == SENSOR_RELEASE) { 
            pane->openDialog("LOAD\nSETTINGS ?", [](abstract::Event e) -> void {
                if (load_config("device.cfg") == 0) {
                    setChangesApplied(true);
                    setChangesSaved(true);
                    save_local_cfg(&localAppControl, &applicationControl);
                    save_local_cfg(&tempLocalAppControl, &applicationControl);
                    load_local_cfg(&applicationControl);
                    appOnActionHook();
                }
            }, nullptr);
        }
    });
    
    vm::unlock(MEMORY_ALLOC_LOCK_ID);
    
    save_local_cfg(&localAppControl, &applicationControl);
    load_local_cfg(&localAppControl);
    
    update_descktop();
    update_sprites();
    
    for (;;) {
        if (applicationControl.tftControl.backlight == TFT_BACKLIGHT_OFF) {
            vm::wait_event("wakeup");
        }
        settingsAppTouchSensor.invokeEvent();
        vm::lock(FRAME_RENDER_LOCK_ID);
        pane->repaint(*engine->getViewPort());
        vm::unlock(FRAME_RENDER_LOCK_ID);
        
        vm::yield();
    }
    
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
    
    GET_SPRITE(label_save, s, b, "save.bmp", "save");
    GET_SPRITE(label_load, s, b, "open.bmp", "open");
    GET_SPRITE(label_exit, s, b, "cancel.bmp", "cancel");
    SET_SPRITE(dialog, s);
    SET_SPRITE(alert, s);
    GET_SPRITE(label_down, s, b, "arrow.bmp", "arrow");
    SET_SPRITE(label_up, s); 
    GET_SPRITE(label_apply, s, b, "apply.bmp", "apply");
    GET_SPRITE(label_revert, s, b, "revert.bmp", "revert");
    
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

static inline void update_descktop ()
{
    vm::lock(FRAME_RENDER_LOCK_ID);
    pane->fill();
    vm::unlock(FRAME_RENDER_LOCK_ID);
}

static void checkEditable ()
{
    if (item_browser->getSelectedNode()->getAttribute() < 0) {
        label_edit->setForeground(COLOR_RED);
        label_edit->setSilent(false);
        label_right->setForeground(RGB24_TO_RGB16(176,196,222));
        label_right->setSilent(true);
        label_left->setForeground(RGB24_TO_RGB16(176,196,222));
        label_left->setSilent(true);
    } else {
        label_edit->setForeground(RGB24_TO_RGB16(176,196,222));
        label_edit->setSilent(true);
        label_right->setForeground(COLOR_RED);
        label_right->setSilent(false);
        label_left->setForeground(COLOR_RED);
        label_left->setSilent(false);
    }
}

#include "pp.h"

static void edit (APPLICATION_CONTROL *ctl)
{
    static char name[24];
    static char editBuffer[48];
    memset(name, '\0', 24);
    memset(editBuffer, '\0', 48);
    auto node = item_browser->getSelectedNode();
    char *text = node->getText();
    int src = 0;
    int dest = 0;
    while ((text[src] != '\0') && (src < 24)) {
        if (text[src] == ':') {
            name[dest++] = text[src++];
            name[dest] = ' ';
            break;
        }
        name[dest] = text[src];
        dest++;
        src++;
    }
    if (src >= 24) {
        pane->openAlert("CANNOT EDIT\nCOMPONENT VALUE !", nullptr);
        return;
    }
    dest = 0;
    while ((text[src] != '\0') && (dest < 48)) {
        editBuffer[dest] = text[src];
        dest++;
        src++;
    }
    if (dest >= 48) {
        pane->openAlert("TOO LARGE\nCOMPONENT VALUE !", nullptr);
        return;
    }
    _XCALL(ret, exec_app, EXEC_APP_EDIT_REQUEST, editBuffer);
    if (ret != 0) {
        pane->openAlert("INVALID VALUE !", nullptr);
        return;
    }
    remove_chars(editBuffer, " \t\n\r");
    switch (item_browser->getSelectedNode()->getAttribute()) {
        case APP_IPV4 :
                        if (update_ipv4(editBuffer, ctl->networkControl.ipv4_ip) >= 0) {
                            strcpy(ctl->networkControl.ipv4Str, editBuffer);
                        } else {
                            pane->openAlert("Wrong Ipv4 String !\nBack To Defaults.", nullptr);
                            return;
                        }
            break;
        case APP_MAC :
                        if (update_mac(editBuffer, ctl->networkControl.mac) >= 0) {
                            strcpy(ctl->networkControl.macStr, editBuffer);
                        } else {
                            pane->openAlert("Wrong Mac String !\nBack To Defaults.", nullptr);
                            return;
                        }
            break;
        default : pane->openAlert("Unknown\nParameter !", nullptr);
            break;
    }
    
    
    node->setText(name);
    node->apendText(editBuffer);
    pane->wakeUp();
    pane->openAlert("VALUE PASS OK !", nullptr);
}

#include "ff.h"

static int save_config (char *path)
{
    vm::lock(FILE_SYSTEM_LOCK_ID);
    vm::lock(MEMORY_ALLOC_LOCK_ID);
    vm::lock(SENSOR_LOCK_ID);
    
    int ret = -1;
    FRESULT res;
    uint32_t f_bytes_write;
    FIL *file = new FIL;
    if (file != nullptr) {
        res = f_open(file, path, FA_WRITE | FA_CREATE_NEW);
        if (res == FR_EXIST) {
            res = f_unlink(path);
            res = f_open(file, path, FA_WRITE | FA_CREATE_NEW);
        }
        if (res == FR_OK) {
            res = f_write(file, (void *)&applicationControl, sizeof(APPLICATION_CONTROL), &f_bytes_write);
            if (res != FR_OK) {
                pane->openAlert("FILE WRITE\nFAILED !", nullptr);
            } else {
                pane->openAlert("SAVE\nSUCCESSFUL !", nullptr);
                ret = 0;
            }
        }
        f_close(file);
        delete file;
    } else {
        pane->openAlert("BUSY.\nTRY LATER !", nullptr);
    }
    vm::unlock(FILE_SYSTEM_LOCK_ID);
    vm::unlock(MEMORY_ALLOC_LOCK_ID);
    vm::unlock(SENSOR_LOCK_ID);
    return ret;
}

static int load_config (char *path)
{
    vm::lock(FILE_SYSTEM_LOCK_ID);
    vm::lock(MEMORY_ALLOC_LOCK_ID);
    vm::lock(SENSOR_LOCK_ID);
    
    int ret = -1;
    FRESULT res;
    uint32_t f_bytes_read;
    
    FIL *file = new FIL;
    if (file != nullptr) {
        res = f_open(file, path, FA_READ);
        if (res == FR_OK) {
            res = f_read(file, (void *)&applicationControl, sizeof(APPLICATION_CONTROL), &f_bytes_read);
            if (res != FR_OK) {
                pane->openAlert("FILE WRITE\nERROR !\nTRY AGAIN !", nullptr);
            } else {
                ret = 0;
                pane->openAlert("LOAD\nSUCCESSFUL !", nullptr);
            }
            f_close(file);
        } else {
            pane->openAlert("\"DEVICE.CFG\"\nFILE NOT FOUND !\nSAVE CHANGES BEFORE\nLOAD", nullptr);
        }
        delete file;
    } else {
        pane->openAlert("BUSY.\nTRY LATER !", nullptr);
    }
    
    vm::unlock(FILE_SYSTEM_LOCK_ID);
    vm::unlock(MEMORY_ALLOC_LOCK_ID);
    vm::unlock(SENSOR_LOCK_ID);
    return ret;
}


#include <string.h>

static int load_local_cfg (APPLICATION_CONTROL *app)
{   
    vm::lock(MEMORY_ALLOC_LOCK_ID);
    item_browser->removeAllNodes();
    
    auto node = item_browser->addNode("BRIGHTNESS : ", 36);
    node->setAttribute(APP_BRI_PARAM);
    node->printInt(app->tftControl.defaultBacklightValue);
    
   node = item_browser->addNode("SCREEN SLEEP TIME : ", 36);
    node->setAttribute(APP_SLEEP_PARAM);
    node->printInt(app->sleepTimeout / 1000);
    node->apendText("S");   
    
    node = item_browser->addNode("GRAPHIC QUALITY : ", 28);
    node->setAttribute(APP_GQTY_PARAM);
    switch (app->graphicControl.quality) {
        case GRAPHIC_QUALITY_BEST : node->apendText("BEST");
            break;
        case GRAPHIC_QUALITY_GOOD : node->apendText("GOOD");
            break;
        case GRAPHIC_QUALITY_FAST : node->apendText("FAST");
            break;
    }
    
    node = item_browser->addNode("GRAPHIC ACCELERATION : ", 28);
    node->setAttribute(APP_GACC_PARAM);
    if (app->graphicControl.acceleration == GRAPHIC_ACCELERATION_ON) {
        node->apendText("ON");
    } else {
        node->apendText("OFF");
    }
    
    node = item_browser->addNode("MAC ADDRESS : ", 36);
    node->setAttribute(APP_MAC);
    node->apendText(app->networkControl.macStr);
    
    
    node = item_browser->addNode("IPV4 : ", 28);
    node->setAttribute(APP_IPV4);
    node->apendText(app->networkControl.ipv4Str);
    
    node = item_browser->addNode("DHCP : ", 12);
    node->setAttribute(APP_DHCP_PARAM);
    if (app->networkControl.useDHCP == true) {
        node->apendText("YES");
    } else {
        node->apendText("NO");
    }
    
    node = item_browser->addNode("ENERGY SAVING : NO", 20);
    node->setAttribute(APP_ENRGY_PARAM);
    
    node = item_browser->addNode("SOUND : ", 20);
    node->setAttribute(APP_SOUND_PARAM);
    if (app->audioControl.soundOn == true) {
        node->apendText("YES");
    } else {
        node->apendText("NO");
    }
    
    node = item_browser->addNode("SNAPSHOT : ", 20);
    node->setAttribute(APP_SNAP_PARAM);
    switch (app->snapshotBpp) {
        case 1 : node->apendText("1BPP");
            break;
        case 4 : node->apendText("4BPP");
            break;
        default : 
            break;
    }
    
    item_browser->refresh();
    
    vm::unlock(MEMORY_ALLOC_LOCK_ID);
    return 0;
}

static int save_local_cfg (APPLICATION_CONTROL *dest, APPLICATION_CONTROL *src)
{
    *dest = *src;
    strcpy(dest->networkControl.ipv4Str, src->networkControl.ipv4Str);
    strcpy(dest->networkControl.macStr, src->networkControl.macStr);
    return 0;
}

static int change_local_param (APPLICATION_CONTROL *app, int dir)
{
    int ret = 0;
    auto node = item_browser->getSelectedNode();
    if (node == nullptr) {
        return -1;
    }
    switch (node->getAttribute()) {
        case APP_SLEEP_PARAM : if (dir < 0) {
                                    app->sleepTimeout -= 1000;
                               } else {
                                    app->sleepTimeout += 1000;
                               }
                               if (app->sleepTimeout < 5000) {
                                   app->sleepTimeout = 5000;
                               } else if (app->sleepTimeout > 100000) {
                                   app->sleepTimeout = 100000;
                               }
                               node->setText("SCREEN SLEEP TIME : ");
                               node->printInt(app->sleepTimeout / 1000);
                               node->apendText("S");
            break;
        case APP_GACC_PARAM :   node->setText("GRAPHIC ACCELERATION : ");
                                if (dir < 0) {
                                    app->graphicControl.acceleration = false;
                                    node->apendText("OFF");
                                } else {
                                    app->graphicControl.acceleration = true;
                                    node->apendText("ON");
                                }
            break;
        case APP_GQTY_PARAM : pane->openAlert("YOU DONT HAVE\nRIGHTS\nTO CHANGE\nTHIS !", nullptr);
                              ret = -1;
            break;
        case APP_SOUND_PARAM : if (dir > 0) {
                                    app->audioControl.soundOn = true;
                               } else {
                                   app->audioControl.soundOn = false;
                               }
                               node->setText("SOUND : ");
                               if (app->audioControl.soundOn == true) {
                                   node->apendText("YES");
                               } else {
                                   node->apendText("NO");
                               }
            break;
        case APP_ENRGY_PARAM : pane->openAlert("YOU DONT HAVE\nRIGHTS\nTO CHANGE\nTHIS !", nullptr);
                               ret = -1;
            break;
        case APP_DHCP_PARAM :  pane->openAlert("YOU DONT HAVE\nRIGHTS\nTO CHANGE\nTHIS !", nullptr);
                               ret = -1;
            break;
        case APP_BRI_PARAM :    if (dir < 0) {
                                    app->tftControl.defaultBacklightValue -= 10;
                                } else {
                                    app->tftControl.defaultBacklightValue += 10;
                                }
                                if (app->tftControl.defaultBacklightValue < BRIGHTNESS_LOW_LEVEL) {
                                    app->tftControl.defaultBacklightValue = BRIGHTNESS_LOW_LEVEL;
                                } else if (app->tftControl.defaultBacklightValue > BRIGHTNESS_MAX_LEVEL) {
                                    app->tftControl.defaultBacklightValue = BRIGHTNESS_MAX_LEVEL;
                                }
                                app->tftControl.backlightValue = app->tftControl.defaultBacklightValue;
                                node->setText("BRIGHTNESS : ");
                                node->printInt(app->tftControl.defaultBacklightValue);
            break;
        case APP_SNAP_PARAM :       node->setText("SNAPSHOT : ");
                                    switch (app->snapshotBpp) {
                                    case 1 :    app->snapshotBpp = 4;
                                                node->apendText("4BPP");
                                        break;
                                    case 4 :    app->snapshotBpp = 1;
                                                node->apendText("1BPP");
                                        break;
                                    default :   app->snapshotBpp = 1;
                                                node->apendText("1BPP");
                                        break;
                                }
            break;
        default : ret = -1;
            break;
    }
    return ret;
}


static void setChangesSaved (bool stat)
{
    changesSaved = stat;
    if (stat == false) {
        label_save->setText("**");
    } else {
        label_save->clearText();
    }
    label_save->wakeUp();
}

static void setChangesApplied (bool stat)
{
    changesApplied = stat;
    if (stat == false) {
        label_apply->setText("**");
    } else {
        label_apply->clearText();
    }
    label_apply->wakeUp();
}
