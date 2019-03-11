#ifndef GLINE_CLASS
#define GLINE_CLASS

#define SOFTWARE_GL

#ifdef SOFTWARE_GL

#include "Dimensions.h"
#include "frame.h"
#include <math.h>


#define labs(x)  ((x) < 0 ? (-x) : (x))

template <typename Color, typename Range>
class Line_Class  : public virtual Dimension<Range>,
                    public virtual Frame<Color> {
    private:
	  int32_t Pixel (int32_t, int32_t, Color);
	  int32_t Point (int32_t, int32_t, int32_t, int32_t, Color);
	  int32_t fBotToTop (int32_t, int32_t, int32_t, Color, uint8_t = 1);
	  int32_t fLeftToRight (int32_t, int32_t, int32_t, Color, uint8_t = 1);
	  int8_t log2LaySize;
	public:
	  Line_Class (Color *f, Dimension<Range> d);
      ~Line_Class ();
	  int32_t DrawDda (int32_t, int32_t, int32_t, int32_t, Color);
      template <typename Plane>
        int32_t lineBold (Plane p0, Plane p1, int z, Color);
      template <typename Plane>
        int32_t line (Plane p0, Plane p1, Color);
	  int32_t lineVertical (Range x0, Range y0, Range y, Color);
	  int32_t lineHorizontal (Range x0, Range y0, Range x, Color);
	  int32_t lineDdaBold (int32_t, int32_t, int32_t, int32_t, int32_t z, Color);
      
	  int32_t lineHorizontalBold (Range, Range, Range, Color, Range);
      int32_t lineVerticalBold (Range, Range, Range, Color, Range);
      
	  int32_t lineVector (int32_t, int32_t, int32_t, int32_t, Color);
	  int32_t lineVectorBold (int32_t, int32_t, int32_t, int32_t, Color);	
};

template <typename Color, typename Range>
Line_Class<Color, Range>::Line_Class (Color *f, Dimension<Range> d)
{
    this->frame = f;
    this->setSize(d);
}

template <typename Color, typename Range>
Line_Class<Color, Range>::~Line_Class ()
{
    
}
		
template <typename Color, typename Range>
int32_t Line_Class<Color, Range>::Pixel (int32_t x, int32_t y, Color color)
{
  *(this->frame + y + x * this->h) = color;
	return 0;
}

template <typename Color, typename Range>
int32_t Line_Class<Color, Range>::DrawDda (int32_t xt0, int32_t yt0, int32_t xt, int32_t yt, Color color)
{
        Box<int32_t> rect = {xt0, yt0, xt, yt};// = this->Screen->TruncLine(xt0, yt0, xt, yt);
        int32_t Xo = rect.x;
        int32_t Yo = rect.y;
        int32_t X  = rect.w;
        int32_t Y  = rect.h;
        int32_t dx = labs(X - Xo);
        int32_t dy = labs(Y - Yo);
        int32_t sx = X >= Xo ? 1 : -1;
        int32_t sy = Y >= Yo ? 1 : -1;
		if(dy <= dx)                    
		{
			int32_t d =  (dy<<1)-dx;                     
			int32_t d1 = dy<<1;
			int32_t d2 = (dy-dx)<<1;
			this->Pixel(Xo, Yo, color);            
			for(int32_t x = Xo+sx, y = Yo, i = 1; i<=dx; i++, x+=sx) 
			{
				if(d>0)            
				{
					d +=d2;
					y +=sy;
				}
				else d +=d1;
				this->Pixel(x, y,color);            
			}
		}
		else
		{
			int32_t d = (dx<<1)-dy;
			int32_t d1 = dx<<1;
			int32_t d2 = (dx-dy)<<1;
			this->Pixel(Xo, Yo,  color);
			for(int32_t x = Xo,y = Yo+sy,i = 1;i<=dy;i++,y+=sy)
			{
				if(d>0)
				{
					d+=d2;
					x+=sx;
				}
				else d+=d1;
				this->Pixel(x, y, color);
			}
		}  
  return 0;	
}

template <typename Color, typename Range> template <typename Plane>
int32_t Line_Class<Color, Range>::line (Plane p0, Plane p1, Color color)
{
	return DrawDda(p0.x, p0.y, p1.x, p1.y, color);
}

template <typename Color, typename Range>
int32_t Line_Class<Color, Range>::lineDdaBold (int32_t xt0, int32_t yt0, int32_t xt, int32_t yt, int32_t z, Color color)
{
	int32_t Xo = xt0;
    int32_t Yo = yt0;
    int32_t X  =  xt;
    int32_t Y  =  yt;
	int32_t dx = labs(X - Xo);
    int32_t dy = labs(Y - Yo);
    int32_t sx = X >= Xo ? z : -z;
    int32_t sy = Y >= Yo ? z : -z;
		if(dy <= dx)                    
		{
			int32_t d =  (dy<<1)-dx;                     
			int32_t d1 = dy<<1;
			int32_t d2 = (dy-dx)<<1;           
			for(int32_t x = Xo+sx, y = Yo, i = 1; i<=dx/z; i++, x+=sx) 
			{
				if(d>0)            
				{
					d +=d2;
					y +=sy;
				}
				else {
					  d +=d1;
				}
				this->fBotToTop(x, y, y + z, color, z);           
			}
		}
		else
		{
			int32_t d = (dx<<1)-dy;
			int32_t d1 = dx<<1;
			int32_t d2 = (dx-dy)<<1;
			for(int32_t x = Xo,y = Yo+sy,i = 1;i<=dy/z;i++,y+=sy)
			{
				if(d>0)
				{
					d+=d2;
					x+=sx;
				}
				else {
					  d+=d1;
				}
				this->fLeftToRight(x, y, x + z, color, z);
			}
		}  
  return 0;	
}

template <typename Color, typename Range> template <typename Plane>
int32_t Line_Class<Color, Range>::lineBold (Plane p0, Plane p1, int z, Color color)
{
	return lineDdaBold(p0.x, p0.y, p1.x, p1.y, z, color);
}

template <typename Color, typename Range>
int32_t Line_Class<Color, Range>::lineVector (int32_t x0, int32_t y0, int32_t R, int32_t A, Color color)
{
  //if (this->Screen->Test()) return -1;
	float x, y;
    x = x0 + R * sin((float)A);
    y = y0 + R * cos((float)A);
	return this->DrawDda(x0, y0, x, y, color);
}



template <typename Color, typename Range>
int32_t Line_Class<Color, Range>::lineVectorBold (int32_t x0, int32_t y0, int32_t R, int32_t A, Color color)
{
    //if (this->Screen->Test()) return -1;
	float x, y;
    x = x0 + R * sin((float)A);
    y = y0 + R * cos((float)A);	
	return this->DrawDdaBold(x0, y0, x, y, color);	
}

template <typename Color, typename Range>
int32_t Line_Class<Color, Range>::lineVertical (Range x0, Range y0, Range y, Color color)
{
  //if (this->Screen->Test()) return -1;
	this-fBotToTop(
									x0, 
									y0 > y ? y : y0,
									y0 > y ? y0 : y,
									color
								);
  return 0;	
}

template <typename Color, typename Range>
int32_t Line_Class<Color, Range>::lineHorizontal (Range x0, Range y0, Range x, Color color)
{
  //if (this->Screen->Test()) return -1;
	this-fLeftToRight(
									x0 > x ? x : x0, 
									y0,
									x0 > x ? x0 : x,
									color
								);
  return 0;	
}

template <typename Color, typename Range>
int32_t Line_Class<Color, Range>::lineHorizontalBold (Range x0, Range y0, Range x, Color color, Range z)
{
  Range x_max = x0 > x ? x0 : x;
  Range x_min = x0 > x ? x : x0;
    
  for (int i = 0; i < z; i++) {
      this->fLeftToRight (
									x_min, 
									y0 + i,
									x_max,
									color
								);
  }  
  return 0;	
}

template <typename Color, typename Range>
int32_t Line_Class<Color, Range>::lineVerticalBold (Range x0, Range y0, Range y, Color color, Range z)
{
  Range y_max = y0 > y ? y0 : y;
  Range y_min = y0 > y ? y : y0;
    
  for (int i = 0; i < z; i++) {
      this-fBotToTop(
									x0 + i, 
									y_min,
									y_max,
									color
								);
  }
  return 0;	
}

template <typename Color, typename Range>
int32_t Line_Class<Color, Range>::fBotToTop (int32_t x0, int32_t y0, int32_t y, Color color, uint8_t z)
{
	int32_t D = x0 * this->h;
	
	y0 += D;
	y += D;
	for (; y > y0; y--) {
	    for (int32_t tz = z * this->h; tz >= 0; tz -= this->h) {
          *(this->frame + y + tz) = color;
			}
	}
	return 0;		
}

template <typename Color, typename Range>
int32_t Line_Class<Color, Range>::fLeftToRight (int32_t x0, int32_t y0, int32_t x, Color color,  uint8_t z)
{
	int32_t h0 = this->h;
	x0 = x0 * h0 + y0;
	x = x * h0 + y0;
	
  for (; x > x0; x -= h0) {
	    for (int32_t tz = z; tz >= 0; tz--) {
          *(this->frame + x + tz) = color;
			}
	}
	return 0;		
}

#else /*!SOFTWARE_GL*/

#endif /*SOFTWARE_GL*/


#endif


