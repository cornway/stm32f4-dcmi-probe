#include <stdint.h>
#include "vm.h"
#include "app_def.h"
#include "device_conf.h"
#include "gui.h"
#include <stdio.h>

extern uint16_t batteryValue;
extern color_t batteryFullColor;
extern color_t batteryEmptycolor;
extern color_t batteryMetalColor;
extern color_t batteryTextColor;


static char batteryValueStr[8];
void drawBattery (NonPalette<color_t, range_t, COLOR_WHITE> *component)
{
    float batteryLevel = ((2.5f * (float)batteryValue) * 2.0f / 4096.0f) / 3.3f;
    
    
    //component->getGraphic()->drawString(component->getBox(), "BATTERY :", 0x0);
    range_t w = component->w - 4;
    range_t h = component->h;
    range_t batFull = (range_t)((float)w * batteryLevel);

    Box<range_t> bfull = {component->x, component->y, batFull, h};
    Box<range_t> bempty = {component->x + batFull, component->y, w - batFull, h};
    Box<range_t> dmetal = {component->x + w, component->y + h / 2 - h / 4, 4, h / 2};

    auto g = component->getGraphic();
    
    g->fill(bfull, batteryFullColor | component->getGlow());
    g->fill(bempty, batteryEmptycolor | component->getGlow());
    g->fill(dmetal, batteryMetalColor | component->getGlow());
    
    sprintf(batteryValueStr, "  %d%%", (int)(100.0f * batteryLevel));
    //d(component->getTextBox(batteryValueStr, g->getFontWidth(), g->getFontHeight()));
    //d.setOrigins(component->getOrigins());
    component->getGraphic()->drawString(component->getBox(), batteryValueStr, batteryTextColor);
}