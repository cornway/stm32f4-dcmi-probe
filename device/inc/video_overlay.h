#ifndef VIDEO_OVERLAY_H__
#define VIDEO_OVERLAY_H__

#include "guiEngine.h"
#include "abstract.h"
#include "device_conf.h"
#include "device_gui_conf.h"

class Voverlay {
    private :
        ViewPort<range_t> wp;
        GuiEngine<color_t, range_t, devgui::COLOR_WHITE> guiEngine;
        GContentPane<color_t, range_t, devgui::COLOR_WHITE> *contentPane;
        int screenWidth, screenHeight;
    public :
        Voverlay ();
        void init (color_t *memory, range_t w, range_t h);
        void repaint ();
        void setText (char *name, const char *value);
        void appendText (char *name, const char *value);
    
}; /*Voverlay*/

#endif /*VIDEO_OVERLAY_H__*/