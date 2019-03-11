#include <stdint.h>
#include "vm.h"
#include "app_def.h"
#include "device_conf.h"
#include "gui.h"
#include <stdio.h>


extern uint32_t sys_time_milis;
static uint32_t sys_time_milis_old = 0;

DAY_TIME clock = {0, 0, 0};

int parse_clock (char *token, DAY_TIME *clock);
char *getClockText ();
char str[10] = "00:00:00\0";
static bool beep = false;

void clockTick (void (*hook) (void))
{
    if ((sys_time_milis - sys_time_milis_old) > 1000) {
        clock.seconds = (sys_time_milis / 1000) % 60;
        clock.minutes = (sys_time_milis / 60000) % 60;
        clock.hours = (sys_time_milis / 3600000) % 24;
        
        if (beep == false) {
            beep = true;
        } else {
            beep = false;
        }
        
        str[0] = (clock.hours / 10) + '0';
        str[1] = (clock.hours % 10) + '0';
        str[2] = '.';
        str[3] = (clock.minutes / 10) + '0';
        str[4] = (clock.minutes % 10) + '0';
        if (beep == true) {
            str[5] = '.';
        } else {
            str[5] = ' ';
        }
        str[6] = (clock.seconds / 10) + '0';
        str[7] = (clock.seconds % 10) + '0';
        str[8] = '\0';
        sys_time_milis_old = sys_time_milis;
        if (hook != nullptr) {
            (*hook)();
        }
    }    
}

void drawClock (NonPalette<color_t, range_t, COLOR_WHITE> *component)
{
    Box<range_t> box = component->getBox();
    component->getGraphic()->fill(box, component->getBackground() | component->getGlow());
    component->getGraphic()->drawString(box, str, component->getForeground());
}

#include "pp.h"

int parse_clock (char *token, DAY_TIME *clock)
{
    DAY_TIME _clock;
    int ret = parse_int_enum(token, 4, '.', [&] (char *s, int num) -> int {
        switch (num) {
            case 0 :    if (parse_uint32(s, (uint32_t *)&_clock.hours) < 0) { return -1; }
                break;
            case 1 :    if (parse_uint32(s, (uint32_t *)&_clock.minutes) < 0) { return -1; }
                break;
            case 2 :    if (parse_uint32(s, (uint32_t *)&_clock.seconds) < 0) { return -1; }
                break;
            default : return -1;
                break;
        };
        return 0;
    });
    if (ret < 0) {
        return -1;
    } 
    *clock = _clock;
    sys_time_milis = _clock.seconds * 1000 + (_clock.minutes * 60 * 1000) + (_clock.hours * 3600 * 1000);
    sys_time_milis_old = sys_time_milis;
    return 0;
}

char *getClockText ()
{
    return str;
}

