#include "analog_app.h"
#include "explorer_app.h"
#include "sprite.h"
#include "bitmap.h"

TouchSensor analogAppTouchSensor;
static bool adc_finish = false;
static bool adc_pause = false;
static SpriteRAM spriteRAM;
static BitmapFactory bmp;

static GuiEngine<color_t, range_t, COLOR_WHITE> *engine;
static GDialog<color_t, range_t, COLOR_WHITE> *dialog;
static GAlert<color_t, range_t, COLOR_WHITE> *alert;

static GContentPane<color_t, range_t, COLOR_WHITE> *pane,
                                                   *pane_help;

static GLabel<color_t, range_t, COLOR_WHITE>    *label_trigger,
                                                *label_time_dec, 
                                                *label_time_inc,
                                                *label_time,
                                                *label_scale_dec,
                                                *label_scale_inc,
                                                *label_scale,
                                                *label_mode,
                                                *label_exit,
                                                *label_store,
                                                *label_load,
                                                *label_help,
                                                *label_fft_grain,
                                                *label_view,
                                                *label_auto_zero,
                                                *label_snapshot,
                                                *label_pause;

static GProgressBar<color_t, range_t, COLOR_WHITE>  *slide_alpha_edge;

GTextField<color_t, range_t, COLOR_WHITE> *help_info;

static NonPalette<color_t, range_t, COLOR_WHITE> *plotter;

INT_T analog_app (WORD_T size, void *argv)
{
    analogAppTouchSensor.clearEvent();
    
    vm::lock(MEMORY_ALLOC_LOCK_ID);
    
    if (adc_core_init() < 0) {
        vm::unlock(MEMORY_ALLOC_LOCK_ID);
        return -1;
    }
    
    spriteRAM.init(SPRITE_MEMORY_BASE, SPRITE_MEMORY_SIZE);
    
    engine = new GuiEngine<color_t, range_t, COLOR_WHITE>(fontArray.font_mono, (color_t *)FRAME_MEMORY_BASE, 0, 0, TFT_WIDTH, TFT_HEIGHT);
    pane = engine->newContentPane(0, 0, TFT_WIDTH, TFT_HEIGHT);
    pane->setVisible(true);
    
    pane_help = engine->newContentPane(0, 0, TFT_WIDTH, TFT_HEIGHT);
    pane_help->mute(true);
    
    alert = pane->newAlert("WARNING !", 64, 200, 150);
    alert->setBackground(ALERT_BACK_COLOR);
    alert->setForeground(ALERT_TEXT_COLOR);
    alert->setKeyColor(ALERT_KEY_COLOR);
    
    dialog = pane->newDialog("ATTENTION !", 64, 200, 150);
    dialog->setBackground(ALERT_BACK_COLOR);
    dialog->setForeground(ALERT_TEXT_COLOR);
    dialog->setKeyColor(ALERT_KEY_COLOR);

    label_exit = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 12>
                ("exit", "", 0, 280, 40, 40);
    pane->addLabel(label_exit);
    label_exit->setForeground(COLOR_WHITE);
    label_exit->setBackground(0);
    
    label_store = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 0>
                ("store", "", 40, 280, 40, 40);
    pane->addLabel(label_store);
    label_store->setForeground(COLOR_WHITE);
    label_store->setBackground(0);
    
    label_load = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 0>
                ("load", "", 80, 280, 40, 40);
    pane->addLabel(label_load);
    label_load->setForeground(COLOR_WHITE);
    label_load->setBackground(0);
    
    help_info = pane->createComponent<GTextField<color_t, range_t, COLOR_WHITE>, 512>
    ("help", (char *)helpText, 0, 0, TFT_WIDTH, TFT_HEIGHT);
    pane_help->addTextField(help_info);
    help_info->setBackground(0x00);
    help_info->setForeground(COLOR_WHITE);
    
    label_trigger = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 0>
                ("trigger", "", 360, 280, 60, 40);
    pane->addLabel(label_trigger);
    label_trigger->setForeground(0);
    label_trigger->setBackground(COLOR_WHITE);
    
    label_view = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 0>
                ("view", "", 420, 280, 60, 40);
    pane->addLabel(label_view);
    label_view->setForeground(0);
    label_view->setBackground(COLOR_WHITE);
    
    label_time_dec = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 3>
                ("time_dec", "<<", 120, 300, 60, 20);
    pane->addLabel(label_time_dec);
    label_time_dec->setForeground(COLOR_WHITE);
    label_time_dec->setBackground(RGB24_TO_RGB16(64, 64, 128));
    label_time_dec->setTextAllign(GTEXT_ALLIGN_CENTER);
    
    label_time_inc = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 3>
                ("time_inc", ">>", 180, 300, 60, 20);
    pane->addLabel(label_time_inc);
    label_time_inc->setForeground(COLOR_WHITE);
    label_time_inc->setBackground(RGB24_TO_RGB16(64, 64, 128));
    label_time_inc->setTextAllign(GTEXT_ALLIGN_CENTER);
    
    label_time = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 12>
                ("time", "1uS/Div", 120, 280, 120, 20);
    pane->addLabel(label_time);
    label_time->setForeground(COLOR_WHITE);
    label_time->setBackground(RGB24_TO_RGB16(64, 64, 128));
    
    label_scale_dec = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 3>
                ("scale_dec", "<<", 240, 300, 60, 20);
    pane->addLabel(label_scale_dec);
    label_scale_dec->setForeground(COLOR_WHITE);
    label_scale_dec->setBackground(adc_ctl.skin.color[SKIN_CHA_COLOR]);
    label_scale_dec->setTextAllign(GTEXT_ALLIGN_CENTER);
    
    label_scale_inc = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 3>
                ("scale_inc", ">>", 300, 300, 60, 20);
    pane->addLabel(label_scale_inc);
    label_scale_inc->setForeground(COLOR_WHITE);
    label_scale_inc->setBackground(adc_ctl.skin.color[SKIN_CHA_COLOR]);
    label_scale_inc->setTextAllign(GTEXT_ALLIGN_CENTER);
    
    label_scale = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 12>
                ("scale", "1V/Div", 240, 280, 120, 20);
    pane->addLabel(label_scale);
    label_scale->setForeground(COLOR_WHITE);
    label_scale->setBackground(adc_ctl.skin.color[SKIN_CHA_COLOR]);
    
    label_mode = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 12>
                ("mode", "", 360, 260, 60, 20);
    pane->addLabel(label_mode);
    label_mode->setForeground(COLOR_WHITE);
    label_mode->setBackground(RGB24_TO_RGB16(0, 128, 0));
    
    label_auto_zero = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 12>
                ("zero", "MAN", 420, 260, 60, 20);
    pane->addLabel(label_auto_zero);
    label_auto_zero->setForeground(COLOR_WHITE);
    label_auto_zero->setBackground(RGB24_TO_RGB16(0, 128, 0));

    label_help = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 12>
                ("help", "HELP", 0, 260, 60, 20);
    pane->addLabel(label_help);
    label_help->setForeground(COLOR_WHITE);
    label_help->setBackground(RGB24_TO_RGB16(0, 128, 0));
    
    
    label_fft_grain = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 12>
    ("grain", "fft:256", 60, 260, 60, 20);
    pane->addLabel(label_fft_grain);
    label_fft_grain->setForeground(COLOR_WHITE);
    label_fft_grain->setBackground(RGB24_TO_RGB16(0, 128, 0));
    
    label_snapshot = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 12>
    ("snap", "Save Bmp", 120, 260, 100, 20);
    pane->addLabel(label_snapshot);
    label_snapshot->setForeground(COLOR_WHITE);
    label_snapshot->setBackground(RGB24_TO_RGB16(128, 128, 0));
    
    slide_alpha_edge = pane->createComponent<GProgressBar<color_t, range_t, COLOR_WHITE>, 12>
                 ("tresh", "Treshold", 220, 260, 100, 20);
    pane->addProgressBar(slide_alpha_edge);
    slide_alpha_edge->setBackground(0x0);
    slide_alpha_edge->setForeground(RGB24_TO_RGB16(128, 128, 128));
    slide_alpha_edge->setTrackColor(0x00);
    slide_alpha_edge->setMinValue(0);
    slide_alpha_edge->setMaxValue(255);
    slide_alpha_edge->setCurrentValue(127);
    slide_alpha_edge->setKnobColor(RGB24_TO_RGB16(127, 0, 0));
    slide_alpha_edge->setSelectColor(0x001f);
    
    label_pause = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 6>
    ("pause", "  ||", 320, 260, 40, 20);
    pane->addLabel(label_pause);
    label_pause->setForeground(COLOR_WHITE);
    label_pause->setBackground(RGB24_TO_RGB16(64, 64, 64));
    
    plotter = pane->createNonPaletteComponent (0, 0, 480, 260, plotter_draw);
    pane->addNonPalette(plotter);
    plotter->setForeground(COLOR_WHITE);
    plotter->setBackground(0);
    plotter->addListener(plotter_listener);
    
    strip_h1 = plotter->getBox();
    strip_h2 = plotter->getBox();
    strip_v1 = plotter->getBox();
    strip_v2 = plotter->getBox();
    
    strip_h1.y = 10;
    strip_h1.h = 3;
    
    strip_h2.y = strip_h2.y + strip_h2.h - 10;
    strip_h2.h = 3;
    
    strip_v1.x = 10;
    strip_v1.w = 3;
    
    strip_v2.x = strip_h1.x + strip_h1.w - 10;
    strip_v2.w = 3;
       
    analogAppTouchSensor.addListener([](abstract::Event e) -> void {
            if (pane->fireSensor(e.getSource(), e.getCause()) < 0) {
                if (pane_help->fireSensor(e.getSource(), e.getCause()) < 0) {
                    menu_invoke_event(e);
                }
            }                
    });
    
    label_exit->addListener(lExit);
    label_load->addListener(lLoad);
    label_store->addListener(lStore);
    
    label_help->addListener([](abstract::Event e) -> void {
        if (e.getCause() == SENSOR_RELEASE) { 
            pane->mute(true);
            pane_help->mute(false);
            help_info->wakeUp();
        }
    });
    
    help_info->addListener([](abstract::Event e) -> void {
        if (e.getCause() == SENSOR_RELEASE) { 
            pane->mute(false);
            pane_help->mute(true);
            pane->wakeUp();
        }
    });
    
    label_trigger->addListener([](abstract::Event e) -> void {
        if ((e.getCause() == SENSOR_RELEASE) || (e.getCause() == SENSOR_HOLD)) { 
            adc_ctl.trigger++;
            if (adc_ctl.trigger >= T_ARRAY_END) {
                adc_ctl.trigger = T_ARRAY_START + 1;
            }
            label_trigger->setIcon(tSpritesArray[adc_ctl.trigger].getImage());
            label_trigger->wakeUp();
        }
    });
    
    label_view->addListener([](abstract::Event e) -> void {
        if ((e.getCause() == SENSOR_RELEASE) || (e.getCause() == SENSOR_HOLD)) { 
            adc_ctl.view++;
            if (adc_ctl.view >= AD_ARRAY_END) {
                adc_ctl.view = AD_ARRAY_START + 1;
            }
        }
    });
    
    label_time_dec->addListener([](abstract::Event e) -> void {
        if ((e.getCause() == SENSOR_RELEASE) || (e.getCause() == SENSOR_HOLD)) { 
            adc_ctl.timebase--;
            if (adc_ctl.timebase <= TM_ARRAY_START) {
                adc_ctl.timebase = TM_ARRAY_END - 1;
            }
            label_time->setText((char *)tmTextArray[adc_ctl.timebase]);
            label_time->wakeUp();
        }
    });
    
    label_time_inc->addListener([](abstract::Event e) -> void {
        if ((e.getCause() == SENSOR_RELEASE) || (e.getCause() == SENSOR_HOLD)) { 
            adc_ctl.timebase++;
            if (adc_ctl.timebase >= TM_ARRAY_END) {
                adc_ctl.timebase = TM_ARRAY_START + 1;
            }
            label_time->setText((char *)tmTextArray[adc_ctl.timebase]);
            label_time->wakeUp();
        }
    });
    
    
    current_scale_p = &adc_ctl.scale_a;
    static color_t ch_color;
    ch_color = adc_ctl.skin.color[SKIN_CHA_COLOR];
    label_scale->addListener([](abstract::Event e) -> void {
        if ((e.getCause() == SENSOR_RELEASE) || (e.getCause() == SENSOR_HOLD)) { 
            if (current_scale_p == &adc_ctl.scale_a) {
                current_scale_p = &adc_ctl.scale_b;
                ch_color = adc_ctl.skin.color[SKIN_CHB_COLOR];
            } else {
                current_scale_p = &adc_ctl.scale_a;
                ch_color = adc_ctl.skin.color[SKIN_CHA_COLOR];
            }
            label_scale->setText((char *)sTextArray[*current_scale_p]);
            label_scale->setBackground(ch_color);
            label_scale_dec->setBackground(ch_color);
            label_scale_inc->setBackground(ch_color);
        }
    });
    
    label_scale_dec->addListener([](abstract::Event e) -> void {
        if ((e.getCause() == SENSOR_RELEASE) || (e.getCause() == SENSOR_HOLD)) { 
            (*current_scale_p)--;
            if (*current_scale_p <= S_ARRAY_START) {
                *current_scale_p = S_ARRAY_END - 1;
            }
            label_scale->setText((char *)sTextArray[*current_scale_p]);
            label_scale->wakeUp();
        }
    });
    
    label_scale_inc->addListener([](abstract::Event e) -> void {
        if ((e.getCause() == SENSOR_RELEASE) || (e.getCause() == SENSOR_HOLD)) { 
            (*current_scale_p)++;
            if (*current_scale_p >= S_ARRAY_END) {
                *current_scale_p = S_ARRAY_START + 1;
            }
            label_scale->setText((char *)sTextArray[*current_scale_p]);
            label_scale->wakeUp();
        }
    });
    
    label_mode->addListener([](abstract::Event e) -> void {
        if ((e.getCause() == SENSOR_RELEASE) || (e.getCause() == SENSOR_HOLD)) { 
            adc_ctl.mode++;
            if (adc_ctl.mode >= M_ARRAY_END) {
                adc_ctl.mode = M_ARRAY_START + 1;
            }
            label_mode->setText((char *)mTextArray[adc_ctl.mode]);
            label_mode->wakeUp();
        }
    });
    
    label_auto_zero->addListener([](abstract::Event e) -> void {
        if ((e.getCause() == SENSOR_RELEASE) || (e.getCause() == SENSOR_HOLD)) { 
            adc_ctl.auto_zero ^= 1;
            if (adc_ctl.auto_zero == 1) {
                label_auto_zero->setText("AUTO");
            } else {
                label_auto_zero->setText("MAN");
            }
        }
    });
    
    
    
    label_fft_grain->addListener([](abstract::Event e) -> void {
        if ((e.getCause() == SENSOR_RELEASE) || (e.getCause() == SENSOR_HOLD)) { 
            if (adc_ctl.fft_grain < 7) {
                adc_ctl.fft_grain++;
            } else {
                adc_ctl.fft_grain = 0;
            }
            label_fft_grain->setText("fft:");
            label_fft_grain->printInt(1 << (7 - adc_ctl.fft_grain));
        }
    });  

    
    label_snapshot->addListener([](abstract::Event e) -> void {
        if (e.getCause() == SENSOR_RELEASE) { 
            SNAPSHOT(ret, "snapshot.bmp");
            if (ret >= 0) {
                pane->openAlert("Snapshot Saved\nSuccessfully !", nullptr);
                appOnActionHook();
            }
        }
    });  
    
    label_pause->addListener([](abstract::Event e) -> void {
        if (e.getCause() == SENSOR_RELEASE) { 
            if (adc_pause == true) {
                adc_pause = false;
                label_pause->setText("  ||");
            } else {
                adc_pause = true;
                label_pause->setText("  |>");
            }
        }
    });  
    
    vm::unlock(MEMORY_ALLOC_LOCK_ID);
    
    update_sprites();
    update_settings(&adc_ctl);
    
    
    adc_finish = false;
    vm::create(adc_app, "adc_app", 512, 2, size, argv);
    
    pane->openDialog("LOAD\nDEFAULTS ?\n(From default.sco)", [](abstract::Event e) -> void {
            if (load_cfg("default.sco") == 0) {
                update_settings(&adc_ctl);
                appOnActionHook();
            }
    }, nullptr);
                
    for (;;) {
        if (applicationControl.tftControl.backlight == TFT_BACKLIGHT_OFF) {
            vm::wait_event("wakeup");
        }
        analogAppTouchSensor.invokeEvent();
        vm::lock(FRAME_RENDER_LOCK_ID);
        plotter->wakeUp();
        pane->repaint(*engine->getViewPort());
        pane_help->repaint(*engine->getViewPort());
        menu_repaint(engine);
        vm::unlock(FRAME_RENDER_LOCK_ID);
        vm::sleep(2);
    }    
}


static GContentPane<color_t, range_t, COLOR_WHITE> *menu_pane = nullptr;
static GLabel<color_t, range_t, COLOR_WHITE>    *label_color_select,
                                                *label_color_show,
                                                *label_menu_exit;

static GProgressBar<color_t, range_t, COLOR_WHITE>  *slide_color_r,
                                                    *slide_color_g,
                                                    *slide_color_b,
                                                    *slide_ch_width,
                                                    *slide_strip_width;


static bool menu_created = false;
static bool menu_destroyed = false;

static void update_color (ADC_CTL *ctl, int8_t selectedColor);

static int menu_init (GuiEngine<color_t, range_t, COLOR_WHITE> *engine)
{
    vm::lock(MEMORY_ALLOC_LOCK_ID);
    menu_pane = engine->newContentPane(0, 0, TFT_WIDTH, TFT_HEIGHT);
    if (menu_pane == nullptr) {
        vm::unlock(MEMORY_ALLOC_LOCK_ID);
        return -1;
    }
    menu_pane->setVisible(true);
    
    label_color_select = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 18>
                ("", "", 0, 0, 160, 40);
    menu_pane->addLabel(label_color_select);
    label_color_select->setForeground(0);
    label_color_select->setBackground(ALERT_KEY_COLOR);
    //label_color_select->setTextAllign(GTEXT_ALLIGN_CENTER);
    
    label_menu_exit = menu_pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 0>
                ("", "", 0, 0, 40, 40);
    menu_pane->addLabel(label_menu_exit);
    label_menu_exit->setForeground(0);
    label_menu_exit->setBackground(ALERT_KEY_COLOR);
    
    slide_color_r = menu_pane->createComponent<GProgressBar<color_t, range_t, COLOR_WHITE>, 0>
                 ("", "", 0, 0, 160, 40);
    menu_pane->addProgressBar(slide_color_r);
    slide_color_r->setBackground(0x0);
    slide_color_r->setForeground(0x001f);
    slide_color_r->setTrackColor(COLOR_WHITE);
    slide_color_r->setMinValue(0);
    slide_color_r->setMaxValue(255);
    slide_color_r->setCurrentValue(127);
    slide_color_r->setKnobColor(RGB24_TO_RGB16(127, 0, 0));
    slide_color_r->setSelectColor(0x001f);
    
    slide_color_g = menu_pane->createComponent<GProgressBar<color_t, range_t, COLOR_WHITE>, 0>
                 ("", "", 0, 0, 160, 40);
    menu_pane->addProgressBar(slide_color_g);
    slide_color_g->setBackground(0x0);
    slide_color_g->setForeground(0x001f);
    slide_color_g->setTrackColor(COLOR_WHITE);
    slide_color_g->setMinValue(0);
    slide_color_g->setMaxValue(255);
    slide_color_g->setCurrentValue(127);
    slide_color_g->setKnobColor(RGB24_TO_RGB16(0, 127, 0));
    slide_color_g->setSelectColor(0x001f);
    
    slide_color_b = menu_pane->createComponent<GProgressBar<color_t, range_t, COLOR_WHITE>, 0>
                 ("", "", 0, 0, 160, 40);
    menu_pane->addProgressBar(slide_color_b);
    slide_color_b->setBackground(0x0);
    slide_color_b->setForeground(0x001f);
    slide_color_b->setTrackColor(COLOR_WHITE);
    slide_color_b->setMinValue(0);
    slide_color_b->setMaxValue(255);
    slide_color_b->setCurrentValue(127);
    slide_color_b->setKnobColor(RGB24_TO_RGB16(0, 0, 127));
    slide_color_b->setSelectColor(0x001f);
    
    slide_strip_width = menu_pane->createComponent<GProgressBar<color_t, range_t, COLOR_WHITE>, 6>
                 ("", "strip", 0, 0, 80, 20);
    menu_pane->addProgressBar(slide_strip_width);
    slide_strip_width->setBackground(0x0);
    slide_strip_width->setForeground(0x001f);
    slide_strip_width->setTrackColor(COLOR_WHITE);
    slide_strip_width->setMinValue(0);
    slide_strip_width->setMaxValue(10);
    slide_strip_width->setCurrentValue(4);
    slide_strip_width->setKnobColor(RGB24_TO_RGB16(127, 0, 0));
    slide_strip_width->setSelectColor(0x001f);
    
    slide_ch_width = menu_pane->createComponent<GProgressBar<color_t, range_t, COLOR_WHITE>, 6>
                 ("", "curve", 0, 0, 80, 20);
    menu_pane->addProgressBar(slide_ch_width);
    slide_ch_width->setBackground(0x0);
    slide_ch_width->setForeground(0x001f);
    slide_ch_width->setTrackColor(COLOR_WHITE);
    slide_ch_width->setMinValue(0);
    slide_ch_width->setMaxValue(10);
    slide_ch_width->setCurrentValue(4);
    slide_ch_width->setKnobColor(RGB24_TO_RGB16(127, 0, 0));
    slide_ch_width->setSelectColor(0x001f);

    label_color_show = menu_pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 0>
                ("", "", 0, 0, 40, 140);
    menu_pane->addLabel(label_color_show);
    label_color_select->setForeground(0);
    label_color_select->setBackground(RGB24_TO_RGB16(127, 127, 127));
    
    
    static int8_t selectedColor = SKIN_GRID_COLOR;
    static color_t color = adc_ctl.skin.color[selectedColor];
    label_color_select->setText("< ");
    label_color_select->apendText((char *)tColorArray[selectedColor]);
    label_color_select->apendText(" >");
    
    label_menu_exit->addListener([](abstract::Event e) -> void {
        if (e.getCause() == SENSOR_RELEASE) { 
            menu_destroyed = true;
            pane->setSilent(false);
            appOnActionHook();
        }
    });
    
    
    slide_color_r->setCurrentValue(RGB16_GET_R(color) << 3);
    slide_color_g->setCurrentValue(RGB16_GET_G(color) << 2);
    slide_color_b->setCurrentValue(RGB16_GET_B(color) << 3);
    
    slide_color_r->setKnobColor(RGB24_TO_RGB16(slide_color_r->getCurrentValue(), 0, 0));
    slide_color_g->setKnobColor(RGB24_TO_RGB16(0, slide_color_g->getCurrentValue(), 0));
    slide_color_b->setKnobColor(RGB24_TO_RGB16(0, 0, slide_color_b->getCurrentValue()));
    
    label_color_show->setBackground( RGB24_TO_RGB16
                                                        (
                                                            slide_color_r->getCurrentValue(),
                                                            slide_color_g->getCurrentValue(),
                                                            slide_color_b->getCurrentValue()
                                                        ));
    
    
    label_color_select->addListener([](abstract::Event e) -> void {
        if (e.getCause() == SENSOR_RELEASE) {
            TouchPointTypeDef tp = *(TouchPointTypeDef *)e.getSource();
            if (tp.x < 40) {
                selectedColor--;
            } else if (tp.x > 120) {
                selectedColor++;
            } else {
                return;
            }
            if (selectedColor >= SKIN_ARRAY_END) {
                selectedColor = SKIN_ARRAY_START + 1;
            } else if (selectedColor <= SKIN_ARRAY_START) {
                selectedColor = SKIN_ARRAY_END - 1;
            }
            label_color_select->setText("<");
            label_color_select->apendText((char *)tColorArray[selectedColor]);
            label_color_select->apendText(">");
            
            color = adc_ctl.skin.color[selectedColor];
            
            slide_color_r->setCurrentValue(RGB16_GET_R(color) << 3);
            slide_color_g->setCurrentValue(RGB16_GET_G(color) << 2);
            slide_color_b->setCurrentValue(RGB16_GET_B(color) << 3);
            
            slide_color_r->setKnobColor(RGB24_TO_RGB16(slide_color_r->getCurrentValue(), 0, 0));
            slide_color_g->setKnobColor(RGB24_TO_RGB16(0, slide_color_g->getCurrentValue(), 0));
            slide_color_b->setKnobColor(RGB24_TO_RGB16(0, 0, slide_color_b->getCurrentValue()));
            
            label_color_show->setBackground( RGB24_TO_RGB16
                                                        (
                                                            slide_color_r->getCurrentValue(),
                                                            slide_color_g->getCurrentValue(),
                                                            slide_color_b->getCurrentValue()
                                                        ));
        }
    });
    
    slide_color_r->addListener([](abstract::Event e) -> void {
        update_color(&adc_ctl, selectedColor);
        label_color_show->setBackground(adc_ctl.skin.color[selectedColor]);
        slide_color_r->setKnobColor(RGB24_TO_RGB16(slide_color_r->getCurrentValue(), 0, 0));
    });
    
    slide_color_g->addListener([](abstract::Event e) -> void {
        update_color(&adc_ctl, selectedColor);
        label_color_show->setBackground(adc_ctl.skin.color[selectedColor]);
        slide_color_g->setKnobColor(RGB24_TO_RGB16(0, slide_color_g->getCurrentValue(), 0));
    });
    
    slide_color_b->addListener([](abstract::Event e) -> void {
        update_color(&adc_ctl, selectedColor);
        label_color_show->setBackground(adc_ctl.skin.color[selectedColor]);
        slide_color_b->setKnobColor(RGB24_TO_RGB16(0, 0, slide_color_b->getCurrentValue()));
    });
    
    
    slide_strip_width->addListener([](abstract::Event e) -> void {
        adc_ctl.stripWidth = slide_strip_width->getCurrentValue();
    });
    
    slide_ch_width->addListener([](abstract::Event e) -> void {
        adc_ctl.chWidth = slide_ch_width->getCurrentValue();
    });
    
    label_menu_exit->setIcon(label_exit->getIcon());
    
    vm::unlock(MEMORY_ALLOC_LOCK_ID);
    
    Dimension<range_t> d(0, 0, 200, 180);
    d.setOrigins(menu_pane->getOrigins());
    label_color_select->setTopLeftCorner(d.getTopLeftCorner());
    label_menu_exit->setLeftCenter(label_color_select->getRightCenter());
    slide_color_r->setTopLeftCorner(label_color_select->getBottomLeftCorner());
    slide_color_g->setTopCenter(slide_color_r->getBottomCenter());
    slide_color_b->setTopCenter(slide_color_g->getBottomCenter());
    label_color_show->setTopCenter(label_menu_exit->getBottomCenter());
    slide_ch_width->setBottomLeftCorner(d.getBottomLeftCorner());
    slide_strip_width->setLeftCenter(slide_ch_width->getRightCenter());
    
    menu_created = true;
    menu_destroyed = false;
    return 0;
}

static int menu_deinit (GuiEngine<color_t, range_t, COLOR_WHITE> *engine)
{
    menu_created = false;
    vm::lock(MEMORY_ALLOC_LOCK_ID);
    engine->destroyPane(menu_pane);
    menu_pane = nullptr;
    vm::unlock(MEMORY_ALLOC_LOCK_ID);
    
    return 0;
}

static int menu_repaint (GuiEngine<color_t, range_t, COLOR_WHITE> *engine)
{
    if (menu_created == false) {
        return -1;
    }
    if (menu_destroyed == true) {
        menu_deinit(engine);
        return 1;
    }
    menu_pane->wakeUp();
    menu_pane->repaint(*engine->getViewPort());
    return 0;
}

static int menu_invoke_event (abstract::Event e)
{
    if (menu_created == false) {
        return -1;
    }
    return menu_pane->fireSensor(e.getSource(), e.getCause());
}

static void update_color (ADC_CTL *ctl, int8_t selectedColor)
{
    adc_ctl.skin.color[selectedColor] = RGB24_TO_RGB16
                                                        (
                                                            slide_color_r->getCurrentValue(),
                                                            slide_color_g->getCurrentValue(),
                                                            slide_color_b->getCurrentValue()
                                                        );
    if (current_scale_p == &ctl->scale_a) {
        label_scale->setBackground(adc_ctl.skin.color[SKIN_CHA_COLOR]);
        label_scale_dec->setBackground(adc_ctl.skin.color[SKIN_CHA_COLOR]);
        label_scale_inc->setBackground(adc_ctl.skin.color[SKIN_CHA_COLOR]);
    } else {
        label_scale->setBackground(adc_ctl.skin.color[SKIN_CHB_COLOR]);
        label_scale_dec->setBackground(adc_ctl.skin.color[SKIN_CHB_COLOR]);
        label_scale_inc->setBackground(adc_ctl.skin.color[SKIN_CHB_COLOR]);
    }
}

static void update_settings (ADC_CTL *ctl)
{
    label_trigger->setIcon(tSpritesArray[ctl->trigger].getImage());
    label_mode->setText((char *)mTextArray[ctl->mode]);
    label_scale->setText((char *)sTextArray[*current_scale_p]);
    if (current_scale_p == &ctl->scale_a) {
        label_scale->setBackground(adc_ctl.skin.color[SKIN_CHA_COLOR]);
        label_scale_dec->setBackground(adc_ctl.skin.color[SKIN_CHA_COLOR]);
        label_scale_inc->setBackground(adc_ctl.skin.color[SKIN_CHA_COLOR]);
    } else {
        label_scale->setBackground(adc_ctl.skin.color[SKIN_CHB_COLOR]);
        label_scale_dec->setBackground(adc_ctl.skin.color[SKIN_CHB_COLOR]);
        label_scale_inc->setBackground(adc_ctl.skin.color[SKIN_CHB_COLOR]);
    }
    label_time->setText((char *)tmTextArray[ctl->timebase]);
    label_fft_grain->setText("fft:");
    label_fft_grain->printInt(1 << (7 - adc_ctl.fft_grain));
}
static void lExit (abstract::Event e)
{
    if (e.getCause() == SENSOR_RELEASE) { 
        pane->openDialog("EXIT ?", [](abstract::Event e) -> void {
            adc_finish = true;
            while (adc_finish == true) { vm::yield(); }
                appOnActionHook();
                APP_CLEANUP(engine, analogAppTouchSensor);
                adc_core_destroy();
                vm::exit(0);
        }, nullptr); 
    }
}

#include <string.h>
static char path[24];

static void lStore (abstract::Event e)
{ 
    if (e.getCause() == SENSOR_RELEASE) { 
        pane->openDialog("SAVE\nSETTINGS ?", [](abstract::Event e) -> void {
            memset(path, 0, 24);
            strcpy(path, "SCO");
            _XCALL(ret, expl_app, EXPLORER_APP_GET_PATH | EXPLORER_APP_USE_FILTER, (void *)path);
            if (ret == 1) {
                store_cfg(path);
            }
            update_sprites();
        }, nullptr);
    }
}

static void lLoad (abstract::Event e)
{
    if (e.getCause() == SENSOR_RELEASE) { 
        pane->openDialog("LOAD\nSETTINGS ?", [](abstract::Event e) -> void {
            memset(path, 0, 24);
            strcpy(path, "SCO");
            _XCALL(ret, expl_app, EXPLORER_APP_GET_PATH | EXPLORER_APP_USE_FILTER, (void *)path);
            if (ret == 1) {
                if (load_cfg(path) == 0) {
                    update_settings(&adc_ctl);
                }
            }
            update_sprites();
        }, nullptr);
    }    
}



static void update_sprites ()
{
    vm::lock(FILE_SYSTEM_LOCK_ID);
    vm::lock(MEMORY_ALLOC_LOCK_ID);
    vm::lock(SENSOR_LOCK_ID);
    vm::lock(FRAME_RENDER_LOCK_ID);
    
    spriteRAM.clear();
    
    FIL *file = new FIL;
    if (file == nullptr) {
        vm::fault("enomem");
    }
    
    Bitmap *b = nullptr;
    Sprite *s = nullptr;
    
    GET_SPRITE(label_store, s, b, "save.bmp", "save");
    GET_SPRITE(label_load, s, b, "open.bmp", "open");
    GET_SPRITE(label_exit, s, b, "cancel.bmp", "cancel");
    SET_SPRITE(dialog, s);
    SET_SPRITE(alert, s);
    GET_SPRITE(label_view, s, b, "adstyle.bmp", "adstyle");
    
    GET_SPRITE((&tSpritesArray[T_RISE]), s, b, tIconPaths[T_RISE], "rise");
    GET_SPRITE((&tSpritesArray[T_FALL]), s, b, tIconPaths[T_FALL], "fall");
    GET_SPRITE((&tSpritesArray[T_LOW]), s, b, tIconPaths[T_LOW], "low");
    GET_SPRITE((&tSpritesArray[T_HIGH]), s, b, tIconPaths[T_HIGH], "high");
    GET_SPRITE((&tSpritesArray[T_PEAK]), s, b, tIconPaths[T_PEAK], "peak");
    GET_SPRITE((&tSpritesArray[T_PIT]), s, b, tIconPaths[T_PIT], "pit");
    GET_SPRITE((&tSpritesArray[T_ANY]), s, b, tIconPaths[T_ANY], "any");
    
    
    delete file;
    
    vm::unlock(FILE_SYSTEM_LOCK_ID);
    vm::unlock(MEMORY_ALLOC_LOCK_ID);
    vm::unlock(SENSOR_LOCK_ID);
    vm::unlock(FRAME_RENDER_LOCK_ID);
}

#include "ff.h"

static int load_cfg (char *filename)
{
    vm::lock(FILE_SYSTEM_LOCK_ID);
    vm::lock(MEMORY_ALLOC_LOCK_ID);
    vm::lock(SENSOR_LOCK_ID);
    
    int ret = -1;
    FRESULT res;
    uint32_t f_bytes_read;
    
    FIL *file = new FIL;
    if (file != nullptr) {
        res = f_open(file, filename, FA_READ);
        if (res == FR_OK) {
            if (file->fsize == 0) {
                pane->openAlert("FILE IS EMPTY !", nullptr);
            } else {
                res = f_read(file, (void *)&adc_ctl, sizeof(ADC_CTL), &f_bytes_read);
                if (res != FR_OK) {
                    pane->openAlert("FILE WRITE\nERROR !\nTRY AGAIN !", nullptr);
                } else {
                    ret = 0;
                    pane->openAlert("LOAD\nSUCCESSFUL !", nullptr);
                }
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

static int store_cfg (char *filename)
{
    vm::lock(FILE_SYSTEM_LOCK_ID);
    vm::lock(MEMORY_ALLOC_LOCK_ID);
    vm::lock(SENSOR_LOCK_ID);
    
    int ret = -1;
    FRESULT res;
    uint32_t f_bytes_write;
    FIL *file = new FIL;
    if (file != nullptr) {
        res = f_open(file, filename, FA_WRITE | FA_CREATE_NEW);
        if (res == FR_EXIST) {
            res = f_unlink(filename);
            res = f_open(file, filename, FA_WRITE | FA_CREATE_NEW);
        }
        if (res == FR_OK) {
            res = f_write(file, (void *)&adc_ctl, sizeof(ADC_CTL), &f_bytes_write);
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

#define STRIP_FOCUS_DISTANCE    (10U)

Box<range_t> *strip = nullptr;
bool stripOri = false; /*horizontal*/

static void plotter_listener (abstract::Event e)
{
    TouchPointTypeDef p = *(TouchPointTypeDef *)e.getSource();
    if (p.hits > 1) {
        if (menu_init(engine) == 0) {
            pane->setSilent(true);
            return;
        }
    }
    if ((adc_ctl.mode == M_AB_FUNC) || (adc_ctl.mode == M_AS_FUNC)) {
        return;
    }
    if (e.getCause() == SENSOR_RELEASE) {
        strip = nullptr;
    } else if (e.getCause() == SENSOR_TOUCH) {
        p.x += plotter->x;
        p.y += plotter->y;
        if (strip != nullptr) {
            
            if (stripOri == true) {
                strip->x = CLIP(p.x + adc_ctl.stripWidth, plotter->x, plotter->w);
            } else {
                strip->y = CLIP(p.y + adc_ctl.stripWidth, plotter->y, plotter->h);
            }
            return;
        }
        if ((p.y > (strip_h1.y - (STRIP_FOCUS_DISTANCE / 2))) && (p.y < (strip_h1.y + STRIP_FOCUS_DISTANCE))) {
            strip_h1.y = p.y;
            strip = &strip_h1;
            stripOri = false;
            return;
        } else if ((p.y > (strip_h2.y - (STRIP_FOCUS_DISTANCE / 2))) && (p.y < (strip_h2.y + STRIP_FOCUS_DISTANCE))) {
            strip_h1.y = p.y;
            strip = &strip_h2;
            stripOri = false;
            return;
        } else if ((p.x > (strip_v1.x - (STRIP_FOCUS_DISTANCE / 2))) && (p.x < (strip_v1.x + STRIP_FOCUS_DISTANCE))) {
            strip_v1.x = p.x;
            strip = &strip_v1;
            stripOri = true;
            return;
        } else if ((p.x > (strip_v2.x - (STRIP_FOCUS_DISTANCE / 2))) && (p.x < (strip_v2.x + STRIP_FOCUS_DISTANCE))) {
            strip_v2.x = p.x;
            strip = &strip_v2;
            stripOri = true;
            return;
        } 
    }
}

static void plotter_draw (NonPalette<color_t, range_t, COLOR_WHITE> *component)
{
    static int16_t offset_a = 0, offset_b = 0;
    Point<range_t> maxMin_a, maxMin_b;
    
    Dimension<range_t> d(component->getDimension());
    auto g = component->getGraphic();
    g->fill(d, adc_ctl.skin.color[SKIN_BACK_COLOR]);
    if ((AdModes[adc_ctl.view] & AD_WATCH_GRID) == AD_WATCH_GRID) {
        uint8_t tiny = 1;
        if ((adc_ctl.view & AD_GRID_GRAIN_MODE) == AD_GRID_GRAIN_MODE) {
            tiny = 1;
        } else {
            tiny = 2;
        }
        g->grid(
                d, 
                d.w / (GRID_CELLS_WIDTH * tiny), 
                d.h / (GRID_CELLS_HEIGHT * tiny), 
                GRID_GRAIN / tiny, 
                adc_ctl.skin.color[SKIN_GRID_COLOR]
            );
    }
    
    if ((AdModes[adc_ctl.view] & AD_WATCH_BORDER) == AD_WATCH_BORDER) {
        g->rectangle(d.getBox(), 3, COLOR_WHITE);
    }
    
    if (adc_ctl.auto_zero == 1) {
        offset_b = (plotter->h / 2) - (maxMin_b.x / 2);
        offset_a = (plotter->h / 2) - (maxMin_a.x / 2);
    }
    
    uint8_t sw = adc_ctl.stripWidth;
    
    if ((AdModes[adc_ctl.view] & AD_WATCH_COORDS) == AD_WATCH_COORDS) {
        Box<range_t> cbox = {0, 0, 0, 0};
        Point<range_t> cpoint = {0, 0};
        
        cpoint = plotter->getLeftCenter();
        cbox.x = cpoint.x;
        cbox.y = cpoint.y - sw / 2;
        cbox.w = plotter->w;
        cbox.h = sw;
        
        g->fill(cbox, adc_ctl.skin.color[SKIN_COORDS_COLOR]);
        
        cpoint = plotter->getTopCenter();
        cbox.y = cpoint.y - plotter->h;
        cbox.x = cpoint.x - sw / 2;
        cbox.h = plotter->h;
        cbox.w = sw;
        
        g->fill(cbox, adc_ctl.skin.color[SKIN_COORDS_COLOR]);
    }
    
    strip_v1.w = sw;
    strip_v2.w = sw;
    strip_h1.h = sw;
    strip_h2.h = sw;
    
    if ((AdModes[adc_ctl.view] & AD_WATCH_VSTRIPS) == AD_WATCH_VSTRIPS) {
        g->fill(strip_v1, adc_ctl.skin.color[SKIN_VSTRIP_COLOR]);
        g->fill(strip_v2, adc_ctl.skin.color[SKIN_VSTRIP_COLOR]);
    }  
    g->fill(strip_h1, adc_ctl.skin.color[SKIN_HSTRIP_COLOR]);
    g->fill(strip_h2, adc_ctl.skin.color[SKIN_HSTRIP_COLOR]); 
    
    if (adc_ctl.auto_zero == 0) {
        offset_a = (strip_h1.y - plotter->getRightCenter().y);    
        offset_b = (strip_h2.y - plotter->getRightCenter().y); 
    }
    
    Box<range_t> b = {0, 0, 0, 0};
    uint8_t fft_size = (1 << (7 - adc_ctl.fft_grain));
    BaseComplex *complex;
    range_t clip_high = component->h - adc_ctl.chWidth, clip_width = component->w - adc_ctl.chWidth;
    vm::lock(ADC_LOCK_ID);
    switch (adc_ctl.mode) {
        case M_AB : addc_buf(buffer_a, buffer_a, offset_a, component->w, clip_high);
                    addc_buf(buffer_b, buffer_b, offset_b, component->w, clip_high);
                    maxMin_a = curve(component, buffer_a, clip_width, adc_ctl.skin.color[SKIN_CHA_COLOR]);
                    maxMin_b = curve(component, buffer_b, clip_width, adc_ctl.skin.color[SKIN_CHB_COLOR]);
            break;
        case M_ABSUM : addc_buf(buffer_a, buffer_a, offset_a, component->w, clip_high);
                       add_buf(buffer_temp, buffer_a, buffer_b, component->w, clip_high);
                       maxMin_a = curve(component, buffer_temp, clip_width, adc_ctl.skin.color[SKIN_CHA_COLOR]);
            break;
        case M_ABDIF : addc_buf(buffer_a, buffer_a, offset_a, component->w, clip_high);
                       dif_buf(buffer_temp, buffer_a, buffer_b, component->w, clip_high / 2, clip_high);
                       maxMin_a = curve(component, buffer_temp, clip_width, adc_ctl.skin.color[SKIN_CHA_COLOR]);
            break;
        case M_AB_MUL : addc_buf(buffer_a, buffer_a, offset_a, component->w, clip_high);
                        mul_buf(buffer_temp, buffer_a, buffer_b, component->w, clip_high);
                        maxMin_a = curve(component, buffer_temp, clip_width, adc_ctl.skin.color[SKIN_CHA_COLOR]);
            break;
        case M_AB_DIV : addc_buf(buffer_a, buffer_a, offset_a, component->w, clip_high);
                        div_buf(buffer_temp, buffer_a, buffer_b, component->w, clip_high);
                        maxMin_a = curve(component, buffer_temp, clip_width, adc_ctl.skin.color[SKIN_CHA_COLOR]);
            break;
        case M_AS_FUNC :
        case M_AB_FUNC :    b = curve_lisajou(component, buffer_a, buffer_b, component->w, adc_ctl.skin.color[SKIN_CHA_COLOR]);
                            strip_h1.y = b.y;
                            strip_h2.y = b.y + b.h;
                            strip_v1.x = b.x;
                            strip_v2.x = b.x + b.w;
            break;
        case M_FFT_A :  addc_buf(buffer_a, buffer_a, offset_a, component->w, component->h);
                        complex = make_fft(buffer_a, component->w, fft_size, 0);
                        maxMin_a = spectre(component, complex, component->w, fft_size, adc_ctl.skin.color[SKIN_CHA_COLOR]);
            break;
        case M_FFT_B :  addc_buf(buffer_b, buffer_b, offset_b, component->w, component->h);
                        complex = make_fft(buffer_b, component->w, fft_size, 0);
                        maxMin_a = spectre(component, complex, component->w, fft_size, adc_ctl.skin.color[SKIN_CHB_COLOR]);
            break;
    };
    vm::unlock(ADC_LOCK_ID);
}

static Point<range_t> curve (NonPalette<color_t, range_t, COLOR_WHITE> *component, adc_word_t *buffer, int size, color_t color)
{
    auto g = component->getGraphic();
    int16_t magnitude;
    Box<range_t> box;
    Point<range_t> p = {0, 0};
    
    for (int i = 1; i < size; i++) {
        magnitude = ABS(buffer[i - 1], buffer[i]);
        p.x = MAX(p.x, magnitude);
        box.y = MIN(buffer[i - 1], buffer[i]) + component->y;
        box.x = i;
        box.w = MAX(magnitude, adc_ctl.chWidth);
        box.w = MIN(box.w, adc_ctl.chWidth);
        box.h = MAX(magnitude, adc_ctl.chWidth);
        g->fill(box, color); 
    }
    return p;
}

static Point<range_t> spectre (NonPalette<color_t, range_t, COLOR_WHITE> *component, BaseComplex *buffer, int bsize, int size, color_t color)
{
    auto g = component->getGraphic();
    Box<range_t> box;
    Point<range_t> p = {0, 0};
    range_t step = bsize / size;
    color_t grey = 0;
    
    for (int i = 0; i < size; i++) {
        box.x = i * step;
        box.y = component->y;
        box.w = step;
        box.h =  (range_t)buffer[i].real;
        box.h = CLIP(box.h, 0, component->h);
        grey = GET_GREY_16BPP((range_t)buffer[i].imag);
        g->fill(box, color | grey); 
    }
    return p;
}

static Box<range_t> curve_lisajou (NonPalette<color_t, range_t, COLOR_WHITE> *component, adc_word_t *buffer_a, adc_word_t *buffer_b, int size, color_t color)
{
    auto g = component->getGraphic();
    Box<range_t> box;
    int16_t max_x = 0, max_y = 0;
    for (int i = 0; i < size; i++) {
        max_x = MAX(max_x, buffer_a[i]);
        max_y = MAX(max_y, buffer_b[i]);
    }  
    max_x = max_x / 2;
    max_y = max_y / 2;
    int16_t zero_x = (component->x + component->w) / 2 - max_x;
    int16_t zero_y = (component->y + component->h) / 2 - max_y;
    int16_t low_x = component->x, low_y = component->y;
    box.w = adc_ctl.chWidth;
    box.h = adc_ctl.chWidth;
    int16_t hi_x = component->x + component->w - 3, hi_y = component->y + component->h - 3;
    for (int i = 0; i < size; i++) {
        box.x = CLIP(buffer_a[i] + zero_x, low_x, hi_x);
        box.y = CLIP(buffer_b[i] + zero_y, low_y, hi_y);
        g->fill(box, color); 
    }
    Box<range_t> b = {zero_x, zero_y, max_x * 2, max_y * 2};
    return b;
}

static float setScale (int8_t scale)
{
    float s = 0.0f;
    switch (scale) {
            case S_5V_DIV : s = 0.02f;
                break;
            case S_2V_DIV : s = 0.05f;
                break;
            case S_1V_DIV : s = 0.1f;
                break;
            case S_500MV_DIV : s = 0.2f;
                break;
            case S_200MV_DIV : s = 0.5f;
                break;
            case S_100MV_DIV : s = 0.7f;
                break;
            case S_50MV_DIV : s = 1.0f;
                break;
        }
    return s;
}

static float setTimeBase (int8_t timeBase)
{
    float period = 1.0f;
    switch (timeBase) {
        case TM_200MS_DIV : period = 0.001f;
            break;
        case TM_100MS_DIV : period = 0.002f;
            break;
        case TM_50MS_DIV : period = 0.003f;
            break;
        case TM_20MS_DIV : period = 0.004f;
            break;
        case TM_10MS_DIV : period = 0.005f;
            break;
        case TM_5MS_DIV : period = 0.006f;
            break;
        case TM_2MS_DIV : period = 0.007f;
            break;
        case TM_1MS_DIV : period = 0.008f;
            break;
        case TM_500US_DIV : period = 0.009f;
            break;
        case TM_200US_DIV : period = 0.01f;
            break;
        case TM_100US_DIV : period = 0.02f;
            break;
        case TM_50US_DIV : period = 0.03f;
            break;
        case TM_20US_DIV : period = 0.04f;
            break;
        case TM_10US_DIV : period = 0.05f;
            break;
        case TM_5US_DIV : period = 0.06f;
            break;
        case TM_2US_DIV : period = 0.07f;
            break;
        case TM_1US_DIV : period = 0.08f;
            break;
        case TM_500NS_DIV : period = 0.09f;
            break;
        case TM_200NS_DIV : period = 0.1f;
            break;
    }
    return period;
}

static INT_T adc_app (WORD_T size, void *argv)
{
    /*adc init*/
    
    for (;;) {
        /*trigger wait*/
        /*adc start*/
        /*exit if app done*/
        trigger_wait(adc_ctl.trigger, 0);
        static float phase = 0.0f;
        static float scale = 1.0f;
        static float period = 0.0f;
        period = setTimeBase(adc_ctl.timebase);
        phase += 0.01f;
        scale = setScale(adc_ctl.scale_a);
        if (adc_pause == false) {
            vm::lock(ADC_LOCK_ID);
            fill_buffer_sin(buffer_a, 480, 3.14f + phase, scale, period);
            scale = setScale(adc_ctl.scale_b);
            fill_buffer_sin(buffer_b, 480, 1.57f + phase * 2, scale, period);
        }
        vm::unlock(ADC_LOCK_ID);
        if (adc_finish == true) {
            adc_finish = false;
            return 0;
        }
        vm::yield();
    }
}


static void trigger_wait (int8_t trigger, int8_t channel)
{
    switch (trigger) {
        case T_RISE :
            break;
        default :
            break;
    }
}

extern uint32_t sys_time_milis;
static int random (int seed)
{
    static int _seed = 0;
    _seed = (seed / (sys_time_milis + 1)) + (sys_time_milis / (seed + 1));
    int i = _seed % 5;
    while (i--) {
       _seed += (_seed << (_seed % 7)) / (_seed * (seed / (_seed << (_seed % 7) + 1)));
       for (int _i = sys_time_milis; _i < sys_time_milis + i; _i++) {
           _seed += (sys_time_milis % 0xff) / (sys_time_milis % 4);
           _seed -= seed;
           _seed = _seed < 0 ? 0 : _seed;
           _i += (sys_time_milis % 0xf) > (_seed % 0xf) ? 1 : 0;
       }
    }
    return _seed;
}



