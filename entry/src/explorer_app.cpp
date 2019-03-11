#include "explorer_app.h"
#include "exec_app.h"
#include "sprite.h"
#include "bitmap.h"

TouchSensor explAppTouchSensor;

static SpriteRAM spriteRAM;
static BitmapFactory bmp;


static GuiEngine<color_t, range_t, COLOR_WHITE> *engine;

static GContentPane<color_t, range_t, COLOR_WHITE> *pane;

static GDialog<color_t, range_t, COLOR_WHITE> *dialog;
static GAlert<color_t, range_t, COLOR_WHITE> *alert;

static GLabel<color_t, range_t, COLOR_WHITE>    *label_up,
                                                *label_down,
                                                *label_close,
                                                *label_open,
                                                *label_rename,
                                                *label_info;

GBrowser<color_t, range_t, COLOR_WHITE> *tf_file_browser;

INT_T expl_app (WORD_T size, void *argv)
{
    explAppTouchSensor.clearEvent();
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
    
    label_up = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 0>
    ("up", "", 40, 280, 40, 40);
    pane->addLabel(label_up);
    
    label_down = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 0>
    ("down", "", 40, 0, 40, 40);
    pane->addLabel(label_down);
    label_down->setImageOr(2);
    
    
    
    label_close = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 0>
    ("close", "", 0, 280, 40, 40);
    pane->addLabel(label_close);
    label_close->setForeground(RGB24_TO_RGB16(173,216,230));
    
    label_open = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 0>
    ("open", "", 0, 240, 40, 40);
    pane->addLabel(label_open);
    label_open->setForeground(RGB24_TO_RGB16(173,216,230));
    
    label_info = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 0>
    ("info", "", 0, 200, 40, 40);
    pane->addLabel(label_info);
    label_info->setForeground(RGB24_TO_RGB16(173,216,230));
    
    label_rename = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 3>
    ("rename", "**", 0, 160, 40, 40);
    pane->addLabel(label_rename);
    label_rename->setForeground(RGB24_TO_RGB16(255,0,0));
    
    
    pane->addPad(RGB24_TO_RGB16(255,255,255), 0, 0, 40, 160);
    pane->addPad(RGB24_TO_RGB16(255,255,255), 40, 40, 40, 240);
    
    
    tf_file_browser = pane->createComponent<GBrowser<color_t, range_t, COLOR_WHITE>, 12>
    ("list", "", 80, 0, 400, 320);
    pane->addBrowser(tf_file_browser);
    tf_file_browser->setBackground(0x00);
    tf_file_browser->setForeground(COLOR_WHITE);
    tf_file_browser->setHeaderColor(RGB24_TO_RGB16(139,69,19));
    tf_file_browser->setSelectColor(RGB24_TO_RGB16	(165,42,42));
    //tf_file_browser->setSilent(true);
    
    explAppTouchSensor.addListener([](abstract::Event e) -> void {
            pane->fireSensor(e.getSource(), e.getCause());  
    });

    label_close->addListener([](abstract::Event e) -> void {
        if (e.getCause() == SENSOR_RELEASE) {
            pane->openDialog("EXIT ?", [](abstract::Event e) -> void {
                appOnActionHook();
                APP_CLEANUP(engine, explAppTouchSensor);
                vm::exit(0);
            }, nullptr);
        }
    });
    
    label_up->addListener([](abstract::Event e) -> void {
        if ((e.getCause() == SENSOR_RELEASE) || (e.getCause() == SENSOR_HOLD)) {
            tf_file_browser->moveDown();
        }
    });
    
    label_down->addListener([](abstract::Event e) -> void {
        if ((e.getCause() == SENSOR_RELEASE) || (e.getCause() == SENSOR_HOLD)) {
            tf_file_browser->moveUp();
        }
    });
    
    static int option = 0;
    static char *path;
    path = (char *)argv;
    option = (size & EXPLORER_APP_GET_PATH) == EXPLORER_APP_GET_PATH ? 1 : 0;
    label_open->addListener([](abstract::Event e) -> void {
        if (e.getCause() == SENSOR_RELEASE) {
            if (tf_file_browser->getSelectedNode() == nullptr) {
                return;
            }
            if (option == 0) {
                pane->openDialog("OPEN FILE : \n", lOpenFile, lDecline);
                dialog->apendText(tf_file_browser->getSelectedNode()->getText());
                dialog->apendText(" ?");
            } else {
                pane->openDialog("OPEN FILE : \n", [] (abstract::Event e) -> void {
                    strcpy((char *)path, tf_file_browser->getSelectedNode()->getText());
                    appOnActionHook();
                    APP_CLEANUP(engine, explAppTouchSensor);
                    vm::exit(1);
                }, nullptr);
                dialog->apendText(tf_file_browser->getSelectedNode()->getText());
                dialog->apendText(" ?");
                
            }
            
        }
    });   

    label_info->addListener([](abstract::Event e) -> void {
        if (e.getCause() == SENSOR_RELEASE) {
            get_fs_info ();
        }
    });  

    label_rename->addListener([](abstract::Event e) -> void {
        if (e.getCause() == SENSOR_RELEASE) {
            pane->openDialog("Rename Selected\nFile ?", [] (abstract::Event e) -> void {
                rename();
            }, nullptr);
        }
    });     

    vm::unlock(MEMORY_ALLOC_LOCK_ID);
    
    clear_sprites();
    update_sprites();
    vm::lock(FRAME_RENDER_LOCK_ID);
    pane->fill(0x0);
    vm::unlock(FRAME_RENDER_LOCK_ID);
    if (scan_dir_filter(size & (~EXPLORER_APP_GET_PATH), (char *)argv) != FR_OK) {
        pane->openAlert("BAD PARAMETERS :\n\"", [] (abstract::Event e) -> void {
            APP_CLEANUP(engine, explAppTouchSensor);
            vm::exit(1);
        });
        alert->apendText((char *)argv);
        alert->apendText("\"");
    }
    
    for (;;) {   
        if (applicationControl.tftControl.backlight == TFT_BACKLIGHT_OFF) {
            vm::wait_event("wakeup");
        }
        explAppTouchSensor.invokeEvent();
        repaint();
        vm::yield();
    }
    
    //APP_CLEANUP(engine);
    //return 0;
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
    
    GET_SPRITE(label_close, s, b, "cancel.bmp", "cancel");
    SET_SPRITE(dialog, s);
    SET_SPRITE(alert, s);
    GET_SPRITE(label_open, s, b, "open.bmp", "open");
    GET_SPRITE(label_down, s, b, "arrow.bmp", "arrow");
    SET_SPRITE(label_up, s);
    GET_SPRITE(label_info, s, b, "ismall.bmp", "ismall");
    GET_SPRITE(label_rename, s, b, "apply.bmp", "apply");
    
    delete file;
    
    vm::unlock(FRAME_RENDER_LOCK_ID);
    vm::unlock(FILE_SYSTEM_LOCK_ID);
    vm::unlock(MEMORY_ALLOC_LOCK_ID);
    vm::unlock(SENSOR_LOCK_ID);
}

static void clear_sprites ()
{
    spriteRAM.clear();
}

static void repaint ()
{
    vm::lock(FRAME_RENDER_LOCK_ID);                    
    pane->repaint(*engine->getViewPort());
    vm::unlock(FRAME_RENDER_LOCK_ID);
}

#include <string.h>

static void rename ()
{
    static char name[24];
    static GBNode *node;
    node = tf_file_browser->getSelectedNode();
    static char *text;
    text = node->getText();
    strcpy(name, text);
    _XCALL(ret, exec_app, EXEC_APP_EDIT_REQUEST, name);
    
    if (ret != 0) {
        pane->openAlert("INVALID VALUE !", nullptr);
        return;
    }
    
    pane->openDialog("Rename : \n\"", [] (abstract::Event e) -> void {
        FRESULT res = FR_NO_FILE;
        //res = f_rename(text, name); /*bug somewere around*/
        if (res != FR_OK) {
            pane->openAlert("Cannot Rename\nSelected File !", nullptr);
            return;
        } else {
           node->setText(name); 
        }
    }, nullptr);
    dialog->apendText(text);
    dialog->apendText("\"\n");
    dialog->apendText("To : \"");
    dialog->apendText(name);
    dialog->apendText("\"\n");
    
    pane->wakeUp();
}


inline static int str_cat(char *dest, char *str, char *post)
{
    int i = 0;
    int c = 0;
    while (str[i] != '\0') {
        dest[c++] = str[i++];
    }
    i = 0;
    while (post[i] != '\0') {
        dest[c++] = post[i++];
    }
    dest[c] = '\0';
    return 0;
}

static void get_fs_info ()
{
    FATFS *fs;
    DWORD fre_clust, fre_sect, tot_sect;
    FRESULT res;

    /* Get volume information and free clusters of drive 1 */
    res = f_getfree("", &fre_clust, &fs);
    if (res != FR_OK) {
        pane->openAlert("CANT GET\nFS INFO !", nullptr);
        return;
    }

    /* Get total sectors and free sectors */
    tot_sect = (fs->n_fatent - 2) * fs->csize;
    fre_sect = fre_clust * fs->csize;

    /* Print the free space (assuming 4096 bytes/sector) */
    pane->openAlert("DRIVE 1 :\n", nullptr);
    alert->apendText("TOTAL SPACE :\n");
    alert->printInt(tot_sect * 4);
    alert->apendText("Kib\n");
    alert->apendText("FREE SPACE :\n");
    alert->printInt(fre_sect * 4);
    alert->apendText("Kib");
}

static int scan_dir_filter (int option, char *arg)
{
    int index = 0;
    char *path, *ext;
    static char path_buf[24];
    static char ext_buf[4];
    /*
        first in arg will be path, then after comma will be extension
        if  there is no comma, we have only extension with (option == EXPLORER_APP_USE_FILTER) and root directory, 
        otherwise we have only path;
    */
    while (arg[index] != ',') {
        if (arg[index] == '\0') {
            index = -1;
            break;
        }
        index++;
    }
    if (index < 0) { /*path or extension only*/
        if (option == EXPLORER_APP_USE_FILTER) { /*extension only*/
            ext = arg;
            path = "";
        } else { /*path only*/
            ext = "";
            path = arg;
        }
        return scan_dir(path, ext);
    } else { /*path and extension, path - before comma, ext - after*/
        int i = 0, id = 0;
        for (; i < index; i++) { /*copy and skip whitespace*/
            if (arg[i] != ' ') {
                path_buf[id++] = arg[i];
            }
        }
        path_buf[id] = '\0';
        id = 0;
        i = index + 1;
        while (arg[i] != '\0') {
            if (arg[i] != ' ') {
                ext_buf[id++] = arg[i];
            }
            i++;
        }
        ext_buf[id] = '\0';
    }
    
    return scan_dir(path_buf, ext_buf);
}

static char dname[48];
static FILINFO fno;
static int scan_dir (const char *path, char *ext)
{
    FRESULT res;
    DIR dir;

    vm::lock(FILE_SYSTEM_LOCK_ID);
    vm::lock(MEMORY_ALLOC_LOCK_ID);
    vm::lock(SENSOR_LOCK_ID);

    res = f_opendir(&dir, path);     /* Open the directory */
    if (res == FR_OK) {
        tf_file_browser->removeAllNodes();
        tf_file_browser->setText((char *)path);
        for (;;) {
            res = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
            if (fno.fattrib & AM_DIR) {                    /* It is a directory */
                if (ext[0] == '\0') {
                    str_cat(dname, fno.fname, "/:");
                    tf_file_browser->addNode(dname, 16);
                }
                //res = scan_dir(path);                    /* Enter the directory */
                if (res != FR_OK) break;
            } else {                      /* It is a file. */
                if (ext[0] != '\0') {
                    if (checkFileExt(fno.fname, ext) == 0) { /*match ok*/
                        str_cat(dname, fno.fname, " ");
                        tf_file_browser->addNode(dname, 16);
                    }
                } else {
                    str_cat(dname, fno.fname, " ");
                    tf_file_browser->addNode(dname, 16);
                }
            }
        }
        f_closedir(&dir);
    }
    tf_file_browser->refresh();
    vm::unlock(FILE_SYSTEM_LOCK_ID);
    vm::unlock(MEMORY_ALLOC_LOCK_ID);
    vm::unlock(SENSOR_LOCK_ID);

    return res;
}

static void getFileInfo (char *path)
{
    FRESULT res;
    DIR dir;
    
    vm::lock(FILE_SYSTEM_LOCK_ID);
    vm::lock(SENSOR_LOCK_ID);

    res = f_opendir(&dir, path); 
    pane->wakeUp();
    if (res != FR_OK) {
        vm::unlock(FILE_SYSTEM_LOCK_ID);
        vm::unlock(SENSOR_LOCK_ID);
        pane->openAlert("UNKNOWN\nFILE PATH!", nullptr);
        return;
    }
    res = f_readdir(&dir, &fno); 
    
    f_closedir(&dir);
    vm::unlock(FILE_SYSTEM_LOCK_ID);
    vm::unlock(SENSOR_LOCK_ID);
    
    if (res != FR_OK) {
        pane->openAlert("FILE READ\nERROR!", nullptr);
        return;
    }
    if (fno.fattrib & AM_DIR) {                    /* It is a directory */
        pane->openAlert("DIR", nullptr);
    } else {                                       /* It is a file. */
        pane->openAlert("FILE : \"", nullptr);
        alert->apendText(fno.fname);
        alert->apendText("\"\nSIZE : \"");
        alert->printInt(fno.fsize);
        alert->apendText("\"BYTES\nATTRIB : \n");
        alert->printInt(fno.fattrib);
    }  
}

static int checkFileExt (char *name, char *ext)
{
    int index = 0;
    while (name[index] != '.') {
        if (name[index] == '\0') {
            return -1; /*file has no extension*/
        }
        index++;
    }
    index++; /*skip dot character*/
    if (strcmp(name + index, ext) == 0) {
        return 0; /*extension ok*/
    }
    return -1; /*no match*/
}

static void lOpenFile (abstract::Event e)
{
    static char *fileName;
    if (e.getCause() == SENSOR_RELEASE) {
        fileName = tf_file_browser->getSelectedNode()->getText();
        _XCALL(xret, file_app, 0, (void *)fileName);
        switch (xret) {
            case UNSUPPORTED_FILE_TYPE : pane->openAlert("UNKNOWN\nFILE TYPE", nullptr);
                break;
            case ADV_CONF_OK : pane->openAlert("ADV CONF OK", nullptr);
                break;
            case  ADV_CONF_FAILED : pane->openAlert("ADV CONF\nFAILED", nullptr);
                break;
            case  FILE_OPEN_ERROR : pane->openAlert("FILE OPEN\nERROR !", nullptr);
                break;
            case  FILE_OPEN_OK : pane->openAlert("FILE OPEN\nOK !", nullptr);
                break;
            default : pane->openAlert("UNKNOWN\nOPERATION", nullptr);
                break;
        }
        clear_sprites();
        update_sprites();
    }
}

static void lDecline (abstract::Event e)
{
    if (e.getCause() == SENSOR_RELEASE) {
        clear_sprites();
        update_sprites();
    }
}

