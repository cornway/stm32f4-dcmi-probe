#include "video_overlay.h"
#include "font_16bpp.h"

Voverlay::Voverlay ()
{
    
}

void Voverlay::init (color_t *memory, range_t w, range_t h)
{
    this->guiEngine.init(font_16bpp, 0, 0, w, h);
    this->wp.setSize(0, 0, w, h);
    auto *mainFrame = guiEngine.newStaticFrame(memory, 0, 0, w, h);
    
    this->contentPane = this->guiEngine.newContentPane( mainFrame );
    this->contentPane->setVisible(true);
    this->contentPane->setEnable(true);
    
    auto label = this->contentPane->createComponent<GLabel<color_t, range_t, devgui::COLOR_WHITE> >
                (0, 280, 120, 20);
                label->setName("fps_rate");
                label->setText("2425263");
                label->setBackground(devgui::COLOR_BLACK);
                label->setForeground(devgui::COLOR_WHITE);
                label->setSelectColor(devgui::COLOR_WHITE);
    this->contentPane->addLabel(label);
    
    this->contentPane->pack();
}

void Voverlay::repaint ()
{
    this->contentPane->fill(devgui::COLOR_BLACK);
    this->contentPane->repaint(this->wp);
}

void
Voverlay::setText (char *name, const char *value)
{
    ComponentDsc dsc = this->guiEngine.getComponentByName(name);
    this->contentPane->setComponentText(dsc, value);
}
		
void
Voverlay::appendText (char *name, const char *value)
{
    ComponentDsc dsc = this->guiEngine.getComponentByName(name);
    this->contentPane->appendComponentText(dsc, value);
}