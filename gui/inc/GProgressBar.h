#ifndef G_PROGRESS_BAR_H
#define G_PROGRESS_BAR_H
#include "graphic.h"
#include "printer.h"
#include "gComponent.h"


template <typename Color, typename Range, Color white>
class GProgressBar :    public GComponent<Color, Range, white>,
                        public Link<GProgressBar<Color, Range, white> >,
                        public std_printer::Printer<char, '\0', '\n', '0', '-'>  {
        private :
                int currentValue;
                int maxValue;
                int minValue;
                Color knobColor;
                Color trackColor;
                Color selectColor;

        public:
            
        GProgressBar (Graphic<Color, Range, white> *graphic, int textSize) : 
                                                               GComponent<Color, Range, white> (graphic), 
                                                               Link<GProgressBar<Color, Range, white> > (),
                                                               std_printer::Printer<char, '\0', '\n', '0', '-'> (textSize)
        {
            this->glow = 0;
            this->currentValue = 0;
            this->maxValue = 100;
            this->minValue = 0;
            this->knobColor = this->background;
            this->trackColor = this->foreground;
        }
        ~GProgressBar ()
        {
            
        }    

        virtual void repaint () final
        {
            Box<Range> box = {this->x, this->y, this->w, this->h};

            float percent = (float)(this->currentValue - this->minValue) / (float)(this->maxValue - this->minValue);
            box.w = (int)((float)this->w * percent);
            if (box.w > 1) {
                this->graphic->fill(box, this->knobColor);
                box.x = box.x + box.w;
                box.w = this->w - box.w;
                this->graphic->fill(box, this->trackColor);
            } else {
                box.w = this->w;
                this->graphic->fill(box, this->trackColor); /*almost empty*/
            }
            
            char *text = this->getText();
            if (text == nullptr) {
                return;
            }
            uint32_t str_len = strlen(text);
            if (str_len == 0) {
                return;
            }
            Dimension<Range> d(0, 0, this->graphic->getFontWidth() * str_len, this->graphic->getFontHeight());
            d.setOrigins(this->getOrigins());
            box.x = d.x;
            box.y = d.y;
            box.w = d.w;
            box.h = d.h;
            this->graphic->drawString(box, this->getText(), this->foreground);
        }
        
        virtual
        int fireSensorListeners (TouchPointTypeDef arg, uint32_t cause) final
        {
            if (this->silent == true) {
                return -1;
            }
            if (this->testPoint(arg.point) == false) {
                return -1;
            }
            arg.point = this->normalize(arg.point);
            this->touchPoint = arg;
            if (cause != SENSOR_RELEASE) {
                float percent = (float)arg.point.x / (float)this->w;
                this->setCurrentValue(percent);
            } 
            #if (EVENT_SIMPLE_SCHEME == 1U)
                if (this->eventListener != nullptr)
                    (*this->eventListener) (abstract::Event(&this->touchPoint, cause));
            #else
                userActionEventBurner.fireEvents(abstract::Event(&this->touchPoint, cause));
            #endif
            return 1;
        }
            
        void setOpaque (int level)
        {
            this->opaqueLevel = level;
        }
        
        void setCurrentValue (float percent)
        {
            if (percent > 1.0f) {
                return;
            }
            this->currentValue = (int)(percent * (float)(this->maxValue - this->minValue));
        }
        
        void setCurrentValue (int value)
        {
            this->currentValue = value;
        }
        
        void setMaxValue (int value)
        {
            this->maxValue = value;
        }
        
        void setMinValue (int value)
        {
            this->minValue = value;
        }
        
        void setKnobColor (Color color)
        {
            this->knobColor = color;
        }
        
        void setTrackColor (Color color)
        {
            this->trackColor = color;
        }
        
        void setSelectColor (Color color)
        {
            this->selectColor = color;
        }
        
        
        
        int getCurrentValue ()
        {
            return this->currentValue;
        }
        
        float getPercentage ()
        {
            float percent = (float)(this->currentValue - this->minValue) / (float)(this->maxValue - this->minValue);
            return percent;
        }
        
        int getMinValue ()
        {
            return this->minValue;
        }
        
        int getMaxValue ()
        {
            return this->maxValue;
        }
        
        Color getKnobColor ()
        {
            return this->knobColor;
        }
        
        Color getTrackColor ()
        {
            return this->trackColor;
        }
};

#endif /*G_PROGRESS_BAR_H*/
