#ifndef VIEWPORT_CLASS
#define VIEWPORT_CLASS

#include "dimensions.h"
#include "gui_defs.h"

template <typename Range>
class ViewPort  : public Dimension<Range>  {
    private :
        int32_t Xm, Ym;
        float Xk, Yk;
        int32_t trueX;
        int32_t trueY;
        int32_t moveX;
        int32_t moveY;

    public :
        ViewPort () {}
        ViewPort (Dimension<Range> d) : Dimension<Range> (d)
        {
            Xm = Ym = 0;
            Xk = Yk = 0.3;
            trueX = 0;
            trueY = 0;
            moveX = moveY = 0;
        }
        ViewPort (ViewPort &wp)
        {
            this->x = wp.x;
            this->y = wp.y;
            this->w = wp.w;
            this->h = wp.h;
            trueX = 0;
            trueY = 0;
            moveX = moveY = 0;
        }
        void operator () (Dimension<Range> d)
        {
            Dimension<Range>::operator = (d);
            Xm = Ym = 0;
            Xk = Yk = 0.3;
            trueX = 0;
            trueY = 0;
            moveX = moveY = 0;
        }
    
        Dimension<Range> translate (Dimension<Range> d)
        {
            return d;
        }
        
        void move (int dx, int dy)
        {
            this->x += dx;
            this->y += dy;
            trueX -= dx;
            trueY -= dy;
            moveX = dx;
            moveY = dy;
        }
        
        void move (Point<Range> p)
        {
            this->x += p.x;
            this->y += p.y;
            trueX -= p.x;
            trueY -= p.y;
            moveX = p.x;
            moveY = p.y;
        }
		
		
		void set (int x, int y)
		{
            Xm = Ym = 0;
            Xk = Yk = 0.6;
			this->x = x;
			this->y = y;
		}
        Box<Range> getView ()
        {
            /*
            Xm += ((float)this->x - Xm) * Xk;
            Ym += ((float)this->y - Ym) * Yk;
            Box<int32_t> box = {Xm, Ym, w, h};
            */
            Box<Range> box = {this->x, this->y, this->w, this->h};
            return box;
        }
        
        
        
        bool testTrueViewPoint (int32_t x, int32_t y)
        {
            return this->testPoint(this->w - this->x, this->h - this->y);
        }
        
        void setTrueView (int32_t x, int32_t y)
        {
            trueX = x - this->x;
            trueY = y - this->y;
        }
        
        void setTrueView (Point<Range> p)
        {
            trueX = p.x - this->x;
            trueY = p.y - this->y;
        }
        
        Point<Range> getTrueView ()
        {
            Point<Range> b = {trueX, trueY};
            return b;
        }
        Point<Range> getMove ()
        {
            Point<Range> b = {moveX, moveY};
            return b;
        }
};



#endif


/*End of file*/

