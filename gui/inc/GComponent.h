#ifndef GCOMPONENT_INTERFACE
#define GCOMPONENT_INTERFACE


#include "graphic.h"
#include "iterable.h"
#include "abstract.h"
#include "touch.h"
#include <string.h>


#define EVENT_SIMPLE_SCHEME     (1U)

enum {
    GANC_TOP_LEFT,
    GANC_TOP_CENTER,
    GANC_TOP_RIGHT,
    GANC_RIGHT_CENTER,
    GANC_BOTTOM_RIGHT,
    GANC_BOTTOM_CENTER,
    GANC_BOTTOM_LEFT,
    GANC_LEFT_CENTER,
};

template <typename Range>
class GAnchor : public Point<Range> {
    private :
        Point<Range> p;
    public :
        GAnchor ()
        {
            
        }
        
        ~GAnchor ()
        {
            
        }
}; /*class GAnchor*/

template <typename Color, typename Range, Color white>
class GComponent : public Dimension<Range> {
    protected :
        
        #if (EVENT_SIMPLE_SCHEME == 1U)
            void (*eventListener) (abstract::Event);
        #else
            abstract::EventBurner userActionEventBurner;
        #endif
		_PACKED struct {
			char name[GCOMPONENT_NAME_LEN];
            bool silent;
            bool sleep;
            Color glow;
            Color maskColor;
        
            Color background;
            Color foreground;
            bool visible;
        
            Graphic<Color, Range, white> *graphic;
		};
        TouchPointTypeDef touchPoint;
		tImage<Color> icon;   
        
        virtual
        int fireSensorListeners (TouchPointTypeDef arg, uint32_t cause)
        {
            if (this->silent == true) {
                return -1;
            }
            if (this->testPoint(arg.point) == false) {
                return -1;
            }
            arg.point = this->normalize(arg.point);
            this->touchPoint = arg;
            switch (sizeof(Color)) {
                case 1 :    this->glow = 0;
                    break;
                case 2 :    this->glow = GET_GREY_16BPP(100U);
                    break;
                case 3 :    this->glow = 0;
                    break;
                case 4 :    this->glow = 0;
                    break;
            }
            #if (EVENT_SIMPLE_SCHEME == 1U)
                if (eventListener != nullptr)
                    (*eventListener) (abstract::Event(&this->touchPoint, cause));
            #else
                userActionEventBurner.fireEvents(abstract::Event(&this->touchPoint, cause));
            #endif
            
            return 1;
        }
    
    public :
        template <typename G>
        GComponent (G *g) : Dimension<Range>()
        {
            this->icon.Image = nullptr;
            this->silent = false;
            this->visible = true;
            memset(this->name, '0', GCOMPONENT_NAME_LEN);
            this->graphic = g;
            this->glow = 0;
            this->sleep = false;
            this->maskColor = white;
            #if (EVENT_SIMPLE_SCHEME == 1U)
                this->eventListener = nullptr;
            #endif
        }
        ~GComponent ()
        {
            #if (EVENT_SIMPLE_SCHEME == 1U)
                this->eventListener = nullptr;
            #else
                userActionEventBurner.removeAll();
            #endif
        }
        
        
        template <typename L>
        void 
        addListener (L l)
        {
            #if (EVENT_SIMPLE_SCHEME == 1U)
                this->eventListener = l;
            #else
                this->userActionEventBurner.addListener
                (
                    new abstract::EventListener(l)
                );
            #endif
        }
        
        void 
        removeAllLiteners ()
        {
            #if (EVENT_SIMPLE_SCHEME == 1U)
                this->eventListener = nullptr;
            #else
                this->userActionEventBurner.removeAll();
            #endif
        }            

        int fireSensorComponent (TouchPointTypeDef arg, uint32_t cause)
        {
            int ret = this->fireSensorListeners(arg, cause);
            if (ret >= 0) {
                this->sleep = false;
            }
            return ret;
        }
        
        void wakeUp ()
        {
            this->sleep = false;
        }
        
        virtual void repaint ()
        {
            
        }
        
        void repaintComponent ()
        {
            if (this->sleep == true) {
                return;
            }
            this->repaint();
            this->sleep = true;
        }
        
        void setGlow (int glow)
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
            this->sleep = false;
        }
        
        template <typename C>
        void setProperties (C *c)
        {
            this->silent = c->silent;
            this->sleep = c->sleep;
            this->glow = c->glow;
            this->background = c->background;
            this->foreground = c->foreground;
            this->maskColor = c->maskColor;
            this->visible = c->visible;
        }
        
        template <typename C>
        void setColorProperties (C *c)
        {
            this->glow = c->glow;
            this->background = c->background;
            this->foreground = c->foreground;
            this->maskColor = c->maskColor;
        }
        
        void setIcon (tImage<Color> *img)
        {
            this->icon.Image = img->Image;
            this->icon.W = img->W;
            this->icon.H = img->H;
        }
        
		void setSilent (bool silent)
        {
            this->silent = silent;
            this->sleep = false;
        }	
		
		void setName (char *name)
		{
            uint32_t i = 0;
            while ((name[i] != '\0') && (i < GCOMPONENT_NAME_LEN)) {
                this->name[i] = name[i];
                i++;
            }
            this->sleep = false;
		}
		
		
               
        void setBackground (Color color)
        {
            background = color;
            this->sleep = false;
        }
        void setForeground (Color color)
        {
            foreground = color;
            this->sleep = false;
        }
        void setMaskColor (Color color)
        {
            this->maskColor = color;
        }
        virtual void setVisible (bool value)
        {
            if (value == true) {
                this->sleep = false;
            }
            this->visible = value;
        } 

        void mute (bool value)
        {
            this->visible = !value;
            this->silent = value;
        }

        char *getName ()
		{
			return this->textName;
		}
        
        tImage<Color> *
        getIcon ()
        {
            return &this->icon;
        }
        
        Color getBackground ()
        {
            return background;
        }
        Color getForeground ()
        {
            return foreground;
        }
        Color getGlow ()
        {
            return this->glow;
        }
        Graphic<Color, Range, white> *getGraphic ()
        {
            return this->graphic;
        }
        
        bool isVisible ()
        {
            return this->visible;
        }
        
        Box<Range> getTextBox (char *text, Range width, Range height)
        {
            Box<Range> box = {0, 0, width, height};
            uint32_t i = 0;
            uint32_t wmax = width;
            while (text[i] != '\0') {
                if (text[i] == '\n') {
                    box.h += height;
                    if (wmax < box.w) {
                        wmax = box.w;
                    }
                    box.w = width;
                } else {
                    box.w += width;
                }
                i++;
            }
            box.w += wmax;
            return box;
        }
        
 
};



#endif


/*End of file*/

