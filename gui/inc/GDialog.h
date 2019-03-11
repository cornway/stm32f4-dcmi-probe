#ifndef GDIALOG_H
#define GDIALOG_H

#include "graphic.h"
#include "printer.h"
#include "iterable.h"
#include "abstract.h"
#include "touch.h"

#define GDKey_BORDER_ALLIGN    (10U)

template <typename Color, typename Range, Color white>
class GDialog : public Dimension<Range>,
                public std_printer::Printer<char, '\0', '\n', '0', '-'> {
    private :
        Graphic<Color, Range, white> *graphic;
        char name[GCOMPONENT_NAME_LEN];
        Dimension<Range> dAccept, dDecline, dClose;
        uint8_t image_or;
        tImage<Color> icon; 
        bool enabled;
        uint8_t glow;
        Color background, foreground, keycolor;
        uint8_t selected;
        TouchPointTypeDef touchPoint;
    
        void (*acceptListener) (abstract::Event e);
        void (*declineListener) (abstract::Event e);
    public :
        GDialog (char *name, int size, Graphic<Color, Range, white> *g, Box<Range> b) :  std_printer::Printer<char, '\0', '\n', '0', '-'> (size)
        {
            this->graphic = g;
            this->setSize(b);
            
            this->dClose(0, 0, 30, 30);
            this->dClose.setTopRightCorner(this->getTopRightCorner());
            
            this->dAccept(0, 0, 50, this->graphic->getFontHeight());
            this->dAccept.setBottomLeftCorner(this->getBottomLeftCorner());
            this->dAccept.moveUp();
            this->dAccept.x += GDKey_BORDER_ALLIGN;
            
            this->dDecline(0, 0, 50, this->graphic->getFontHeight());
            this->dDecline.setBottomRightCorner(this->getBottomRightCorner());
            this->dDecline.moveUp();
            this->dDecline.x -= GDKey_BORDER_ALLIGN;
            
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
            this->selected = 0;
            this->icon.Image = nullptr;
            this->acceptListener = nullptr;
            this->declineListener = nullptr;
        }
        
        ~GDialog ()
        {
            
        }
        
        int fireSensorListeners (TouchPointTypeDef arg, uint32_t cause)
        {
            if (cause != SENSOR_RELEASE) {
                return -1;
            }
            this->selected = 0;
            this->touchPoint = arg;
            this->glow = 0;
            if (this->dClose.testPoint(arg.point) == true) {
                if (cause == SENSOR_RELEASE) {
                    this->enabled = false;
                    if (declineListener != nullptr) {
                        declineListener(abstract::Event(&this->touchPoint, cause));
                    }
                    return 0;
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
                    this->selected = 1;
                }
            }
            
            if (this->dDecline.testPoint(arg.point) == true) {
                if (cause == SENSOR_RELEASE) {
                    this->enabled = false;
                    if (declineListener != nullptr) {
                        declineListener(abstract::Event(&this->touchPoint, cause));
                    }
                    return 0;
                } else {
                    this->selected = 2;
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
            
            this->graphic->drawString(body, "YES", this->foreground | this->glow);
            
            body = this->dDecline.getBox();
            body.x += 4;
            
            this->graphic->drawString(body, "NO", this->foreground | this->glow);
            
            Color color = this->keycolor;
            if (this->selected == 1) {
                color = 0;
            }   
            this->graphic->rectangle(this->dAccept.getBox(), 4, color | this->glow);
            color = this->keycolor;
            if (this->selected == 2) {
                color = 0;
            } 
            this->graphic->rectangle(this->dDecline.getBox(), 4, color | this->glow);
            
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
        
        void open (
                            char *text,
                            void (*acceptListener) (abstract::Event e), 
                            void (*declineListener) (abstract::Event e)   
                  )
        {
            this->enabled = true;
            this->setText(text);
            this->acceptListener = acceptListener;
            this->declineListener = declineListener;     
        }
        
        bool isOpened ()
        {
            return this->enabled;
        }
        
        
        
}; /*class GDialog*/

#endif  /*GDIALOG_H*/
