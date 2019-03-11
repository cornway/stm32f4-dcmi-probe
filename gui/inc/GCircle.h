#ifndef GCIRCLE_CLASS
#define GCIRCLE_CLASS

#define SOFTWARE_GL

#ifdef SOFTWARE_GL

#include "Dimensions.h"
#include "frame.h"

template <typename Color, typename Range>
class GCircle : public virtual Dimension<Range>,
                public virtual Frame<Color> {
private :
    
public :
    GCircle (Color *f, Dimension<Range> d)
    {
        this->frame = f;
        this->setSize(d);
    }
    
    ~GCircle ()
    {
        
    }

    int32_t Sqr (int32_t val)
    {
      return val * val;
    }


    int32_t Pixel (int32_t x, int32_t y, Color color)
    {
      *(this->frame + y + x * this->h) = color;
        return 0;
    }
    int32_t Point (int32_t x0, int32_t y0, int32_t x, int32_t y, Color color)
    {
        int32_t h0 = (x + x0) * this->h + y0;
        int32_t h1 = (-x + x0) * this->h + y0;
        *(this->frame + y + h0) = color;
        *(this->frame - y + h1) = color;
        *(this->frame - y + h0) = color;
        *(this->frame + y + h1) = color;
        return 0;
    }


    int32_t fBotToTop (int32_t x0, int32_t y0, int32_t x, int32_t y, Color color)
    {
        uint32_t h = this->h;
        int32_t D = x0 * h + y0;
        int32_t D0 = x * h + D;
        int32_t D1 = -x * h + D;
        y0 = - y;
        y  = + y;
        for (; y > y0; y--) {
          *(this->frame + y + D0) = color;
              *(this->frame + y + D1) = color;
        }
        return 0;		
    }

    template <typename Plane>
    int32_t circle (Plane rect, int R, Color color)
    {
        
        //this->Screen->Trunc(&rect);
        int32_t y = R;
        int32_t sR = this->Sqr(R);
        int32_t x = 0;
        int32_t Dg = 0;
        int32_t Dd = 0;
        int32_t Dv = 0;
        int32_t sqrx = 0;
        int32_t sqry = 0;
        this->Pixel(rect.x,  y  + rect.y, color);
        this->Pixel(rect.x,  -y  + rect.y, color);
        for(;y >= 0;){
            sqrx = this->Sqr(x + 1) - sR;
            sqry = this->Sqr(y - 1);
            Dg = (sqrx + this->Sqr(y));
            Dd = (sqrx + sqry);
            Dv = (this->Sqr(x) + sqry - sR);
            if(Dd < 0){
                if(Dd > -labs(Dg)){ 	
                      x++;
                      y--;
                }
                else {
                    x++;
                }
            }
            else if(Dd > 0){
                if(Dd < labs(Dv)){
                      y--;
                      x++;
                }
                else {
                    y--;
                }
            }
            else {
                  y--;
                  x++;
            }
            this->Point(rect.x, rect.y, x, y, color);
        }
        return 0;
    }


    int32_t circle (int x0, int y0, int R, Color color)
    {
        Box<int> rect = {x0, y0, 0, 0};
        x0 = x0 - R;
        y0 = y0 - R;
        return this->circle(rect, R, color);
    }

    template <typename Plane>
    int32_t circleFilled (Plane rect, float R, Color color)
    {
        //this->Screen->Trunc(&rect);
        
        int32_t y = (int32_t)R;
        int32_t sR = this->Sqr((int32_t)R);
        int32_t x = 0;
        int32_t Dg = 0;
        int32_t Dd = 0;
        int32_t Dv = 0;
        int32_t sqrx = 0;
        int32_t sqry = 0;
        uint32_t tx = 0;
        this->fBotToTop(rect.x, rect.y, x, y, color);
        for(;y >= 0 ;){
            sqrx = this->Sqr(x + 1) - sR;
            sqry = this->Sqr(y - 1);
            Dg = (sqrx + this->Sqr(y));
            Dd = (sqrx + sqry);
            Dv = (this->Sqr(x) + sqry - sR);
            if(Dd < 0){
                if(Dd > -labs(Dg)){
                      x++;
                      y--;
                }
                else {
                    x++;
                }
            }
            else if(Dd > 0) {
                if(Dd < labs(Dv)){
                      y--;
                      x++;
                }
                else {
                    y--;
                }
            }
            else {
                  y--;
                  x++;
            }
            if (tx != x) {
                this->fBotToTop(rect.x, rect.y, x, y, color);
            } else {}
            tx = x;
        }
        return 0;
    }
};

#else /*!SOFTWARE_GL*/

#endif /*SOFTWARE_GL*/

#endif

/*End of file*/

