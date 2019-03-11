#include "info_app.h"

TouchSensor infoAppTouchSensor;

static GuiEngine<color_t, range_t, COLOR_WHITE> *engine;
static GContentPane<color_t, range_t, COLOR_WHITE> *pane;

static GDialog<color_t, range_t, COLOR_WHITE> *dialog;

GTextField<color_t, range_t, COLOR_WHITE> *tf_info;

INT_T info_app (WORD_T size, void *argv)
{
    infoAppTouchSensor.clearEvent();
    vm::lock(MEMORY_ALLOC_LOCK_ID);
    
    engine = new GuiEngine<color_t, range_t, COLOR_WHITE>(fontArray.font_mono, (color_t *)FRAME_MEMORY_BASE, 0, 0, TFT_WIDTH, TFT_HEIGHT);
    pane = engine->newContentPane(0, 0, TFT_WIDTH, TFT_HEIGHT);
    pane->setVisible(true);
    
    dialog = pane->newDialog("ATTENTION !", 40, 200, 150);
    dialog->setBackground(ALERT_BACK_COLOR);
    dialog->setForeground(ALERT_TEXT_COLOR);
    dialog->setKeyColor(ALERT_KEY_COLOR);
    
    tf_info = pane->createComponent<GTextField<color_t, range_t, COLOR_WHITE>, 240>
    ("list", "", 0, 0, TFT_WIDTH, TFT_HEIGHT);
    pane->addTextField(tf_info);
    tf_info->setBackground(0x00);
    tf_info->setForeground(COLOR_WHITE);
    tf_info->setSilent(false);
    
    infoAppTouchSensor.addListener([](abstract::Event e) -> void {
        pane->fireSensor(e.getSource(), e.getCause());    
    });
    
    tf_info->addListener([](abstract::Event e) -> void {
            if (e.getCause() == SENSOR_RELEASE) {
                pane->openDialog("EXIT ?", [](abstract::Event e) -> void {
                     appOnActionHook();
                     APP_CLEANUP(engine, infoAppTouchSensor);
                     vm::exit(0);
                }, nullptr);
            }   
    });
    
    tf_info->setText("SYSTEM INFO :\n");
    tf_info->apendText("AUTOR : ROMAN PUSTOBAYEV\n");
    tf_info->apendText("piececakeof@mail.ru\n");
    tf_info->apendText("VERSION : 0.011\n");
    tf_info->apendText("ASSEMBLY DATE AND TIME :\n");
    tf_info->apendText(__DATE__);
    tf_info->apendText(", ");
    tf_info->apendText(__TIME__);
    tf_info->apendText("\nPRESS SCREEN TO EXIT");
    
    vm::unlock(MEMORY_ALLOC_LOCK_ID);
    
    infoAppTouchSensor.clearEvent();
    
    for (;;) {    
        infoAppTouchSensor.invokeEvent();
        vm::lock(FRAME_RENDER_LOCK_ID);                    
        pane->repaint(*engine->getViewPort());
        vm::unlock(FRAME_RENDER_LOCK_ID);
        vm::yield();
    }
    
    //APP_CLEANUP(engine);
    //return 0;
}
