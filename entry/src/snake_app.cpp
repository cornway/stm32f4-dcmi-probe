#include "snake_app.h"
#include "sprite.h"
#include "bitmap.h"
#include "time.h"



TouchSensor snakeAppTouchSensor;

static SpriteRAM spriteRAM;
static BitmapFactory bmp;

static _Sprite sprites[SPRITE_COUNT];
_Sprite megaSprites[3];

static Snake *snake = nullptr;
static BonusEngine *bonusEngine = nullptr;
int8_t skill = 1;


static GuiEngine<color_t, range_t, COLOR_WHITE> *engine;

static GContentPane<color_t, range_t, COLOR_WHITE> *pane;

static GDialog<color_t, range_t, COLOR_WHITE> *dialog;
static GAlert<color_t, range_t, COLOR_WHITE> *alert;

static GLabel<color_t, range_t, COLOR_WHITE>    *label_exp,
                                                *label_restart,
                                                *label_exit,
                                                *label_pause;

static NonPalette<color_t, range_t, COLOR_WHITE> *plotter;

INT_T snake_app (WORD_T size, void *argv)
{
    snakeAppTouchSensor.clearEvent();
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
    
    label_exp = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 12>
    ("exp", "EXP : 0", 0, 290, 120, 30);
    pane->addLabel(label_exp);
    label_exp->setSilent(true);
    label_exp->setForeground(COLOR_RED);
    label_exp->setBackground(RGB24_TO_RGB16(188,143,143));
    
    label_restart = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 8>
    ("restart", "RESTART", 120, 290, 120, 30);
    pane->addLabel(label_restart);
    label_restart->setForeground(COLOR_RED);
    label_restart->setBackground(RGB24_TO_RGB16(188,143,143));
    
    label_exit = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 8>
    ("exit", "EXIT", 240, 290, 120, 30);
    pane->addLabel(label_exit);
    label_exit->setForeground(COLOR_RED);
    label_exit->setBackground(RGB24_TO_RGB16(188,143,143));
    
    label_pause = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 8>
    ("pause", "PAUSE", 360, 290, 120, 30);
    pane->addLabel(label_pause);
    label_pause->setForeground(COLOR_RED);
    label_pause->setBackground(RGB24_TO_RGB16(188,143,143));
    
    plotter = pane->createNonPaletteComponent (0, 0, 480, 290
    , draw_snake);
    pane->addNonPalette(plotter);
    plotter->setForeground(COLOR_WHITE);
    plotter->setBackground(0);
    plotter->setVisible(false);
    
    snakeAppTouchSensor.addListener([](abstract::Event e) -> void {
            pane->fireSensor(e.getSource(), e.getCause());         
    });
    
    plotter->addListener(snake_listener);
    
    label_exit->addListener([](abstract::Event e) -> void {
        if (e.getCause() == SENSOR_RELEASE) {
            pane->openDialog("EXIT ? ", [] (abstract::Event e) -> void {
                delete snake;
                delete bonusEngine;
                appOnActionHook();
                APP_CLEANUP(engine, snakeAppTouchSensor);
                vm::exit(0);
            }, nullptr);     
        }
    });
    
    label_restart->addListener([](abstract::Event e) -> void {
        if (e.getCause() == SENSOR_RELEASE) {
            pane->openDialog("RESTART ? ", [] (abstract::Event e) -> void {
                start(); 
                label_exp->setText("EXP : 0");
            }, nullptr);     
        }
    });
    
    label_pause->addListener([](abstract::Event e) -> void {
        if (e.getCause() == SENSOR_RELEASE) {
            pane->openAlert("PAUSE", nullptr);     
        }
    });
    
    vm::unlock(MEMORY_ALLOC_LOCK_ID);
    
    vm::lock(FRAME_RENDER_LOCK_ID);
    pane->fill();
    vm::unlock(FRAME_RENDER_LOCK_ID);
    
    update_sprites();
    pane->wakeUp();
    
    pane->openAlert("READY ? ", [] (abstract::Event e) -> void {
         start();
         plotter->setVisible(true);
    });
    
    
    for (;;) {
        if (applicationControl.tftControl.backlight == TFT_BACKLIGHT_OFF) {
            vm::wait_event("wakeup");
        }
        snakeAppTouchSensor.invokeEvent();
        vm::lock(FRAME_RENDER_LOCK_ID);
        pane->repaint(*engine->getViewPort());
        vm::unlock(FRAME_RENDER_LOCK_ID);
        if ((dialog->isOpened() == false) && (alert->isOpened() == false)) {
            pane->wakeUp();
        }
        time::delay_ms(100 + (300 / skill));
    }
}

uint32_t time_ms;
int maxExp = 0;

static void draw_snake (NonPalette<color_t, range_t, COLOR_WHITE> *component)
{
    snake->step();
    if (snake->testSelf() == 1) { /*gameover*/
        pane->openDialog("GAMEOVER !\n TRY AGAIN ?\n", [] (abstract::Event e) -> void {
                    start ();
                    skill = 1;
        }, [] (abstract::Event e) -> void {
                delete snake;
                delete bonusEngine;
                APP_CLEANUP(engine, snakeAppTouchSensor);
                vm::exit(0);
        });
        int exp = snake->getExp();
        dialog->apendText("Your : ");
        dialog->printInt(exp);
        dialog->apendText("\nTop :");
        dialog->printInt((maxExp > exp ? maxExp : exp));
        maxExp = exp;
    }
    if (bonusEngine->test(snake) > 0) {
        label_exp->setText("EXP : ");
        label_exp->printInt(snake->getExp());
        bonusEngine->addRandom(snake, time_ms, SPRITE_COUNT - 1, sprites);
        skill = (snake->getExp() / 8000) + 1;
    }
    component->getGraphic()->fill(component->getBox(), 0);
    snake->draw(component->getGraphic());
    bonusEngine->draw(component->getGraphic());
    time_ms++;
}


static void snake_listener (abstract::Event e)
{
    TouchPointTypeDef tp = *(TouchPointTypeDef *)e.getSource();
    int16_t x = tp.point.x;
    int16_t y = tp.point.y;
    if (x < plotter->w / 3) {
        snake->setDirection(DIR_LEFT);
        return;
    }
    if (x > (2 * (plotter->w / 3))) {
        snake->setDirection(DIR_RIGHT);
        return;
    }
    if (y < plotter->h / 3) {
        snake->setDirection(DIR_BACKWARD);
        return;
    }
    if (y > (2 * (plotter->h / 3))) {
        snake->setDirection(DIR_FORWARD);
        return;
    }
    
}

extern uint32_t sys_time_milis;

static void start ()
{
    vm::lock(MEMORY_ALLOC_LOCK_ID);
    
    if (snake != nullptr) {
        delete snake;
    }
    if (bonusEngine != nullptr) {
        delete bonusEngine; 
    }
    snake = new Snake(plotter->getBox());
    bonusEngine = new BonusEngine(plotter->getBox());
    
    snake->setIcon(sprites[5].getIcon());
    snake->create(4);
    bonusEngine->addRandom(snake, sys_time_milis, SPRITE_COUNT - 1, sprites);
    
    vm::unlock(MEMORY_ALLOC_LOCK_ID);
}

static void update_sprites ()
{
    spriteRAM.clear();
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
    
    GET_SPRITE(sprites, s, b, "save.bmp", "save");
    GET_SPRITE((sprites + 1), s, b, "open.bmp", "open");
    GET_SPRITE((sprites + 2), s, b, "cancel.bmp", "cancel");
    GET_SPRITE((sprites + 3), s, b, "ismall.bmp", "ismall");
    GET_SPRITE((sprites + 4), s, b, "arrow.bmp", "arrow");
    GET_SPRITE((sprites + 5), s, b, "snake.bmp", "snake");
    GET_SPRITE((megaSprites), s, b, "cam.bmp", "cam");
    GET_SPRITE((megaSprites + 1), s, b, "snakeico.bmp", "snakeico");
    GET_SPRITE((megaSprites + 2), s, b, "scope.bmp", "scope");
    
    delete file;
    
    vm::unlock(FILE_SYSTEM_LOCK_ID);
    vm::unlock(MEMORY_ALLOC_LOCK_ID);
    vm::unlock(SENSOR_LOCK_ID);
    vm::unlock(FRAME_RENDER_LOCK_ID);
}

