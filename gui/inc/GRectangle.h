#ifndef GRECTANGLE_CLASS
#define GRECTANGLE_CLASS

#include "Dimensions.h"
#include "frame.h"

template <typename Color, typename Range>
class GRectangle : public virtual Dimension<Range>,
                   public virtual Frame<Color> {
    private :
        template <typename D>
        int32_t fillRectPart (D rect, Color color)
        {
            register int32_t x = rect.x;
            register int32_t y = rect.y;
            register int32_t h = this->h;
            register int32_t S = h * x;
            
            for (int32_t __x = S + h * rect.w; __x > S; __x -= h) {
                for (uint32_t __y = rect.h + y - 1; __y > y; __y--) {
                    this->frame[__y + __x] = color;
                }
            }
            return 0;
        }
    
    public :
        GRectangle (Color *f, Dimension<Range> d)
        {
            this->frame = f;
            this->setSize(d);
        }
        
        ~GRectangle ()
        {
            
        }
        
        int32_t rectangle (Box<Range> r, uint32_t width, Color color)
        {
            Dimension<Range> b0;
            b0(r.x, r.y, r.w, width);
            Dimension<Range> b1;
            b1(r.x + r.w - width, r.y, width, r.h);
            Dimension<Range> b2;
            b2(r.x, r.y + r.h - width, r.w, width);
            Dimension<Range> b3;
            b3(r.x, r.y, width, r.h);
            
            fillRectPart(b0, color);
            fillRectPart(b1, color);
            fillRectPart(b2, color);
            fillRectPart(b3, color);
            
            return 0;
        }

        template <typename D>
        int32_t DrawHatched (D rect, Color color, uint8_t hatch)
        {
            
            return 0;
        }
};    




#endif

/*End of file*/

