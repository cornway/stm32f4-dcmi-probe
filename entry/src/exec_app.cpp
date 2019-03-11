#include "exec_app.h"
#include <string.h>


TouchSensor execAppTouchSensor;

static GuiEngine<color_t, range_t, COLOR_WHITE> *engine;

static GContentPane<color_t, range_t, COLOR_WHITE> *pane;
static GAlert<color_t, range_t, COLOR_WHITE> *alert;
static GDialog<color_t, range_t, COLOR_WHITE> *dialog;


static GKeypad<color_t, range_t, COLOR_WHITE> *keyPad;

static GTextField<color_t, range_t, COLOR_WHITE> *textField;

static GLabel<color_t, range_t, COLOR_WHITE>    *label_close;


INT_T exec_app (WORD_T size, void *argv)
{
    static uint32_t usage_request, text_size;
    usage_request = size;
    
    execAppTouchSensor.clearEvent();
    vm::lock(MEMORY_ALLOC_LOCK_ID); 

    engine = new GuiEngine<color_t, range_t, COLOR_WHITE>(fontArray.font_mono, (color_t *)FRAME_MEMORY_BASE, 0, 0, TFT_WIDTH, TFT_HEIGHT);
    pane = engine->newContentPane(0, 0, TFT_WIDTH, TFT_HEIGHT);
    pane->setVisible(true);
    
    alert = pane->newAlert("WARNING !", 40, 200, 150);
    alert->setBackground(ALERT_BACK_COLOR);
    alert->setForeground(ALERT_TEXT_COLOR);
    alert->setKeyColor(ALERT_KEY_COLOR);
    
    dialog = pane->newDialog("ATTENTION !", 40, 200, 150);
    dialog->setBackground(ALERT_BACK_COLOR);
    dialog->setForeground(ALERT_TEXT_COLOR);
    dialog->setKeyColor(ALERT_KEY_COLOR);
    
    Box<range_t> box = {0, 0, TFT_WIDTH, 120};
    keyPad = pane->getKeyPad(box);
    keyPad->setBackground(RGB24_TO_RGB16(210,105,30));
    keyPad->setForeground(COLOR_WHITE);
    keyPad->setSelectColor(0xf800);
    
    label_close = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 6>
    ("close", "CLOSE", 0, 290, TFT_WIDTH, 30);
    pane->addLabel(label_close);
    label_close->setBackground(0xf060);
    label_close->setForeground(COLOR_WHITE);
    label_close->setTextAllign(GTEXT_ALLIGN_CENTER);
    
    textField = pane->createComponent<GTextField<color_t, range_t, COLOR_WHITE>, 384>
    ("text", "", 0, 120, TFT_WIDTH, 170);
    pane->addTextField(textField);
    textField->setBackground(COLOR_WHITE);
    textField->setForeground(0);
    textField->setCursorColor(RGB24_TO_RGB16(255, 0, 255));
    
    
    
    if (usage_request == EXEC_APP_PASSWORD_REQUEST) {
        pane->openAlert("REQUIRED PASSWORD TO USE THIS FEATURE", nullptr);
    } else if (usage_request == EXEC_APP_EDIT_REQUEST) {
        textField->setText((char *)argv);
    }
    
    execAppTouchSensor.addListener([](abstract::Event e) -> void {
            pane->fireSensor(e.getSource(), e.getCause());   
    });
    
    label_close->addListener([](abstract::Event e) -> void {
        if (e.getCause() == SENSOR_RELEASE) { 
            appOnActionHook();
            APP_CLEANUP(engine, execAppTouchSensor);
            vm::exit(0);
        }
    });
    
    static char *text;
    static char *dest;
    
    dest = (char *)argv;
    
    keyPad->addListener([](abstract::Event e) -> void {
        GKey <color_t, range_t, COLOR_WHITE> *gkey;
        if (e.getCause() == SENSOR_RELEASE) {
            gkey = keyPad->getSelectedKey();
            if (gkey->getFunc() == KEY_FUNC_CHAIR) {
                textField->apendText(gkey->getName());
            } else { 
                switch (gkey->getFunc()) {
                    case KEY_FUNC_DELETE :  textField->clearText();
                        break;
                    case KEY_FUNC_BCKSPC :  textField->removeLastChar();
                        break;
                    case KEY_FUNC_ENT :     
                                            switch (usage_request) {
                                                case EXEC_APP_PASSWORD_REQUEST :    if (check_password(textField->getText()) < 0) {
                                                                                        alert->alert("INVALID PASSWORD! TRY AGAIN.", nullptr);
                                                                                    } else {
                                                                                        APP_CLEANUP(engine, execAppTouchSensor);
                                                                                        vm::exit(1);
                                                                                    }
                                                    break;
                                                case EXEC_APP_EXECUTE_REQUEST :     
                                                                                    textField->setText( execute(textField->getText()) );
                                                                                    
                                                    break;
                                                case EXEC_APP_EDIT_REQUEST :        text = textField->getText();
                                                                                    pane->openDialog("SAVE\nCHANGES ?", [](abstract::Event e) -> void {
                                                                                        strcpy((char *)dest, text);
                                                                                        APP_CLEANUP(engine, execAppTouchSensor);
                                                                                        vm::exit(0);
                                                                                    }, nullptr);
                                                                                    
                                                    break;
                                                default : alert->alert("UNKNOWN REQUEST.", nullptr);
                                                    break;
                                            }
                        break;
                    case KEY_FUNC_TAB :     textField->apendText("    ");
                        break;
                    case KEY_FUNC_SPACE :   textField->apendText(" ");
                        break;
                }
            }
        }
    });
    
    vm::unlock(MEMORY_ALLOC_LOCK_ID); 
    
    for (;;) {
        if (applicationControl.tftControl.backlight == TFT_BACKLIGHT_OFF) {
            vm::wait_event("wakeup");
        }
        execAppTouchSensor.invokeEvent();
        vm::lock(FRAME_RENDER_LOCK_ID); 
        pane->wakeUp();
        pane->repaint(*engine->getViewPort());
        vm::unlock(FRAME_RENDER_LOCK_ID);
    }
    
}


#include <string.h>
extern ACCESS_CONTROL globalAccessControl;
static int check_password (char *password)
{
    if (strcmp(password, globalAccessControl.password) == 0) {
        return 0;
    }
    return -1;
}
