#ifndef GWINDOW_H
#define GWINDOW_H

#include "gui.h"
#pragma anon_unions

extern FontArray fontArray;



template <typename Color, typename Range, Color white>
class GWindowFactory;

template <typename Color, typename Range, Color white>
class GWindow;

template <typename Color, typename Range, Color white>
class GWindowSkin {
    private :
        __packed struct {
            Color colorHead, colorControl, colorBorder, colorText;
            Color colorAlertText, colorAlertBody;
        };
    
    
        friend GWindow<Color, Range, white>;
        friend GWindowFactory<Color, Range, white>;
    public :
};

template <typename Color, typename Range, Color white>
class GWindow : public Link<GWindow<Color, Range, white> > {
    private :
        GLabel<Color, Range, white>                         *labelOnClose, *labelOnSave, *labelOnLoad;
        GLabel<Color, Range, white>                         *labelHeader;
        GuiEngine<color_t, range_t, COLOR_WHITE>            *engine;
        GContentPane<color_t, range_t, COLOR_WHITE>         *pane;
    public :
        GWindow (GuiEngine<color_t, range_t, COLOR_WHITE> engine*, GWindowSkin<Color, Range, white> *skin, const char *name, const char *text, Range x, Range y, Range w, Range h)        
        {
            this->engine = engine;
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
            
            label_close = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 2>
                ("close", "X", 0, 280, 40, 40);
            pane->addLabel(label_close);
            label_close->setForeground(RGB24_TO_RGB16(173,216,230));
            
            labelOnLoad = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 2>
                ("open", "O", 0, 240, 40, 40);
            pane->addLabel(labelOnLoad);
            labelOnLoad->setForeground(RGB24_TO_RGB16(173,216,230));
            
            labelOnSave = pane->createComponent<GLabel<color_t, range_t, COLOR_WHITE>, 2>
                ("load", "L", 40, 280, 40, 40);
            pane->addLabel(labelOnSave);
            labelOnSave->setForeground(COLOR_RED);
            labelOnSave->setBackground(RGB24_TO_RGB16(188,143,143));
        }
}; /*class GWindow*/

#endif  /*GWINDOW_H*/