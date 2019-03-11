#ifndef GALERT_H
#define GALERT_H

#include "graphic.h"
#include "printer.h"
#include "iterable.h"
#include "abstract.h"
#include "touch.h"

#define GDKey_BORDER_ALLIGN    (10U)

template <typename Color, typename Range, Color white>
class GAlert : public Dimension<Range>,
               public std_printer::Printer<char, '\0', '\n', '0', '-'> {
    private :
        Graphic<Color, Range, white> *graphic;
        char name[GCOMPONENT_NAME_LEN];
        Dimension<Range> dAccept, dClose;
        uint8_t image_or;
        tImage<Color> icon; 
        bool enabled;
        uint8_t glow;
        Color background, foreground, keycolor;
        void (*acceptListener) (abstract::Event e);
        bool acceptSelected;
        TouchPointTypeDef touchPoint;
    
    public :
        GAlert (char *name, int size, Graphic<Color, Range, white> *g, Box<Range> b) :  std_printer::Printer<char, '\0', '\n', '0', '-'> (size)
        {
            this->graphic = g;
            this->setSize(b);
            
            this->dClose(0, 0, 30, 30);
            this->dClose.setTopRightCorner(this->getTopRightCorner());
            
            this->dAccept(0, 0, 50, this->graphic->getFontHeight());
            this->dAccept.setBottomCenter(this->getBottomCenter());
            this->dAccept.moveUp();
            
            int index = 0;
            while ( (name[index] != '\0') && (index < GCOMPONENT_NAME_LEN - 1) ) {
                this->name[index] = name[index];
                index++;
            }
            this->name[index] = '\0';
            
            this->image_or = 0;
            this->enabled = false;
            this->glow = 0;
            this->background = 0;
            this->foreground = white;
            this->keycolor = 0;
            this->icon.Image = nullptr;
            this->acceptSelected = false;
            this->acceptListener = nullptr;
        }
        
        ~GAlert ()
        {
            
        }
        
        int fireSensorListeners (TouchPointTypeDef arg, uint32_t cause)
        {
            this->touchPoint = arg;
            this->acceptSelected = false;
            this->glow = 0;
            if (this->dClose.testPoint(arg.point) == true) {
                if (cause == SENSOR_RELEASE) {
                    this->enabled = false;
                    if (this->acceptListener != nullptr) {
                        this->acceptListener(abstract::Event(&this->touchPoint, cause));
                    }
                    return 0;
                } else {
                    
                }
                
            }
            
            if (this->dAccept.testPoint(arg.point) == true) {
                if (cause == SENSOR_RELEASE) {
                    this->enabled = false;
                    if (this->acceptListener != nullptr) {
                        this->acceptListener(abstract::Event(&this->touchPoint, cause));
                    }
                    return 0;
                } else {
                    this->acceptSelected = true;
                }
            }
            return -1;
        }
        
        void repaint ()
        {
            Range w = 0;
            if (this->icon.Image != nullptr) {
                w = this->icon.W;
            } else {
                w = 0;
            }
            Dimension<Range> header(0, 0, this->w, this->dClose.h);
            header.setTopCenter(this->getTopCenter());
            header.w -= w;
            
            this->graphic->fill(header.getBox(), this->keycolor | this->glow);
            
            Box<Range> body = this->getBox();
            body.h -= this->dClose.h;
            
            this->graphic->fill(body, this->background | this->glow);
            
            body.x += 4;
            
            this->graphic->drawString(body, this->getText(), this->foreground | this->glow);
            
            body = header.getBox();
            
            body.x += 4;
            
            this->graphic->drawString(body, this->name, this->foreground | this->glow);
            
            body = this->dAccept.getBox();
            body.x += 4;
            
            Color color;
            if (this->acceptSelected == true) {
                color = this->keycolor;
            } else {
                color = white;
            }
            this->graphic->drawString(body, "OK", this->foreground | this->glow);
            this->graphic->rectangle(this->dAccept.getBox(), 4, color | this->glow);
            
            if ( this->icon.Image != nullptr ) {
                this->graphic->fill(dClose.getBox(), &this->icon, 0, this->glow, this->image_or);
            } else {
                this->graphic->drawString(dClose.getBox(), "X", this->foreground | this->glow);
            }
        }
        
        void setIcon (tImage<Color> *icon)
        {
            this->icon.Image = icon->Image;
            int w = icon->W, h = icon->H;
            this->icon.W = w;
            this->icon.H = h;
            this->dClose.w = w;
            this->dClose.h = h;
            this->dClose.setTopRightCorner(this->getTopRightCorner());
        }
        
        void setForeground (Color color)
        {
            this->foreground = color;
        }
        
        void setBackground (Color color)
        {
            this->background = color;
        }
        
        void setKeyColor (Color color)
        {
            this->keycolor = color;
        }
        
        void setGlow (uint8_t glow)
        {
            switch (sizeof(Color)) {
                case 1 :    this->glow = 0;
                    break;
                case 2 :    this->glow = GET_GREY_16BPP(glow);
                    break;
                case 3 :    this->glow = 0;
                    break;
                case 4 :    this->glow = 0;
                    break;
            }
        }
        
        void alert (char *text, void (*acceptListener) (abstract::Event e))
        {
            this->enabled = true;
            this->setText(text); 
            this->acceptListener = acceptListener;
        }     
        
        bool isOpened ()
        {
            return this->enabled;
        }
           
}; /*class GDialog*/

#endif  /*GALERT_H*/
