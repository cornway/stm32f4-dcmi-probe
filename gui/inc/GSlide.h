#ifndef GSLIDE_OBJECT
#define GSLIDE_OBJECT
#include "graphic.h"
#include "printer.h"
#include "gComponent.h"

#ifndef __abs__
#define __abs__(x) ((x) < 0 ? (-x) : (x))
#endif



template <typename Color, typename Range, Color white>
class GSlide : public Link<GSlide<Color, Range, white> >,
               public GComponent<Color, Range, white> 
{
               private :
                   int32_t value;
                   int32_t topValue;
                   int32_t bottomValue;
                   Color stripColor;
                   Color trackColor;
                   Color trackDelayColor;
                   Color selectColor;
                   int32_t DelayValue;
                   GSlide<Color, Range, white> *link;
                   int32_t anchor;
                   int32_t anchorLevel;
               public :
                   GSlide (Graphic<Color, Range, white> *graphic) : GComponent<Color, Range, white> (graphic),
																	Link<GSlide<Color, Range, white> > ()
                   {
                       value = 50;
                       topValue = 100;
                       bottomValue = 0;
                       stripColor = 0x7a77;
                       trackColor = 0xFccc;
                       trackDelayColor = 0x000F;
                       DelayValue = value;
                       anchor = 5;
                       anchorLevel = 53;
                       link = nullptr;
                       this->setSelectBorderWidth(5);
                   } 

                   ~GSlide ()
                   {
                      delete(this);  
                   }
                   
                   virtual void repaint () final
                   {
                       this->value += (DelayValue - this->value) / 4;
                       int32_t delaySide = (this->w * DelayValue) / (topValue - bottomValue);
                       int32_t bottomSide = (this->w * value) / (topValue - bottomValue);
                       int32_t topSide = this->w - delaySide;
                       Dimension<Range> box0;
                       Dimension<Range> box1;
                       Dimension<Range> box2;
                       if (delaySide - bottomSide > 0) {
                           box0(this->x, this->y, bottomSide, this->h);
                           box1(this->x + bottomSide, this->y, delaySide - bottomSide, this->h);
                           box2(this->x + delaySide, this->y, topSide, this->h);
                       } else if (delaySide - bottomSide < 0) {
                           box0(this->x, this->y, delaySide, this->h);
                           box1(this->x + delaySide, this->y, bottomSide - delaySide, this->h);
                           box2(this->x + bottomSide, this->y, this->w - bottomSide, this->h); 
                       } else {
                           box0(this->x, this->y, bottomSide, this->h);
                           box1(0, 0, 0, 0);
                           box2(this->x + bottomSide, this->y, topSide, this->h);   
                       }
                       this->graphic->fill(box0, stripColor);
                       this->graphic->fill(box1, trackDelayColor);
                       this->graphic->fill(box2, trackColor);
                       Point<Range> p1 = this->getLeftCenter();;
                       p1.x = p1.x + bottomSide;
                       p1.y = this->y + this->h / 2;
                       /*
                       if (this->selected == true) {
                           this->graphic->circleFilled(p1, 10, this->selectColor);
                       } else {
                           this->graphic->circleFilled(p1, 10, 0);
                       }
                       */
  
                       if (link != nullptr) {   
                           Point<Range> p0 = link->getLeftCenter();
                           p0.x = p0.x + (link->w * link->value) / (link->topValue - link->bottomValue);;
                           this->graphic->lineBold(p1, p0, 7, white);
                           
                           /*
                           if (__abs__(link->value - value) > anchorLevel) {
                                   link->value = (link->value > value ? link->value - 10 : link->value + 10);
                           }
                           */
                       }
                   }
                   
                   void setSelectColor (Color color)
                   {
                       this->selectColor = color;
                   }
                   
                   void setTop (int32_t value)
                   {
                       topValue = value;
                   }
                   
                   void setBottom (int32_t value)
                   {
                       bottomValue = value;
                   }
                   
                   void setValue (int32_t value)
                   {
                       DelayValue = value;
                   }
                   
                   void setLink (GSlide<Color, Range, white> *l)
                   {
                       link = l;
                   }
                   
                   void setText (char *text)
                   {
                       
                   }
                    
                   void appendText (char *text)
                   {
                        
                   }
                   
                   
                   int32_t getTop ()
                   {
                       return topValue;
                   }
                   
                   int32_t getBottom ()
                   {
                       return bottomValue;
                   }
                   
                   int32_t getValue ()
                   {
                       return this->value;
                   }
                   
                   
                   
                   
                   
};                   



#endif

/*End of file*/

