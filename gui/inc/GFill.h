#ifndef GFILL_INTERFACE
#define GFILL_INTERFACE

#include "Dimensions.h"
#include "frame.h"

template <typename Color, typename Range, Color white>
class GFill : public virtual Dimension<Range>,
              public virtual Frame<Color> {
    private :
        
    public :
        GFill (Color *f, Dimension<Range> d)
        {
            this->frame = f;
            this->setSize(d);
        }
        
        ~GFill ()
        {
            
        }

        template <typename Plane>
        int32_t grid (Plane rect, int size, int grain, Color color)
        {
            register int32_t x = rect.x;
            register int32_t y = rect.y;
            register int32_t h = this->h;
            register int32_t D = h * x;
            
            int32_t step_x = this->h * size;
            for (int32_t __x = D + h * rect.w - step_x; __x > D; __x -= step_x) {
                for (int32_t __y = rect.h + y - grain; __y > y; __y -= grain) {
                    this->frame[__y + __x] = color;
                }
            }
            step_x = this->h * grain;
                for (int32_t __x = D + h * rect.w - step_x; __x > D; __x -= step_x) {
                for (int32_t __y = rect.h + y - size; __y > y; __y -= size) {
                    this->frame[__y + __x] = color;
                }
            }
                
            return 0;
        }

        template <typename Plane>
        int32_t grid (Plane rect, int size_x, int size_y, int grain, Color color)
        {
            register int32_t x = rect.x;
            register int32_t y = rect.y;
            register int32_t h = this->h;
            register int32_t D = h * x;
            
            int32_t step_x = this->h * size_x;
            for (int32_t __x = D + h * rect.w - step_x; __x > D; __x -= step_x) {
                for (int32_t __y = rect.h + y - grain; __y > y; __y -= grain) {
                    this->frame[__y + __x] = color;
                }
            }
            step_x = this->h * grain;
                for (int32_t __x = D + h * rect.w - step_x; __x > D; __x -= step_x) {
                for (int32_t __y = rect.h + y - size_y; __y > y; __y -= size_y) {
                    this->frame[__y + __x] = color;
                }
            }
                
            return 0;
        }

        int32_t fill (Color color)
        {
            
            register int32_t x = this->x;
            register int32_t y = this->y;
            register int32_t D = this->h * x;
            
            for (int32_t __x = D + this->h * this->w; __x > D; __x -= this->h) {
                for (int32_t __y = this->h + y; __y > y; __y--) {
                    this->frame[(uint32_t)__y + (uint32_t)__x] = color;
                }
            }
            return 0;
        }

        template <typename Plane>
        int32_t fill (Plane rect, Color color)
        {
            register int32_t x = rect.x;
            register int32_t y = rect.y;
            register int32_t h = this->h;
            register int32_t D = h * x;
            
            for (int32_t __x = D + h * rect.w; __x > D; __x -= h) {
                for (int32_t __y = rect.h + y; __y > y; __y--) {
                    this->frame[__y + __x] = color;
                }
            }
            return 0;
        }

        template <typename Plane>
        int32_t fill (Plane *rect)
        {
            register int32_t x = rect->x;
            register int32_t y = rect->y;
            register int32_t h = this->h;
            register int32_t D = h * x;
            Color color = rect->getBackground();
            
            for (int32_t __x = D + h * rect->w; __x > D; __x -= h) {
                for (int32_t __y = rect->h + y; __y > y; __y--) {
                    this->frame[__y + __x] = color;
                }
            }
            return 0;
        }

        int32_t fill (int xt0, int yt0, int xt, int yt, Color color)
        {
            Dimension<Range> rect((Range)xt0, (Range)yt0, (Range)xt, (Range)yt);
            return this->fill (rect, color);
        }

        template <typename Plane>
        int32_t fillX (Plane rect, Color color)
        {
            Color *ftarget = this->Frame;
            Color *target = nullptr;
            //this->Screen->Trunc(&rect);
            
            register int32_t H = this->h;
            register int32_t x0 = (uint32_t)rect.x * H;
            register int32_t y0 = (uint32_t)rect.y;
            register int32_t h = (uint32_t)rect.h * H;
            register int32_t w = (uint32_t)rect.w;
            int32_t Key = 0;
            
            for (uint32_t __y = y0 + w; __y > y0; __y--) {  
               for (uint32_t __x = x0 + h; __x > x0; __x -= H) {
                        target = ftarget + __y + __x + H;
                        if (*(target) == color && *(target - H) != color )
                             Key ^= 1;
                      if (Key)*(target) = color;
                    
                    }
                    Key = 0;
                }
            return 0;	
        }

        template <typename Plane>
        int32_t fillY (Plane rect, Color color)
        {
            Color *ftarget = this->Frame;
            Color *target = nullptr;
            this->Screen->Trunc(&rect);
            
            register int32_t H = this->h;
            register int32_t x0 = rect.x * H;
            register int32_t y0 = rect.y;
            register int32_t h = rect.h * H;
            register int32_t w = rect.w;
            register int32_t Key = 0;
            
            for (uint32_t __x = x0 + h; __x > x0; __x -= H) {  
              for (uint32_t __y = y0 + w; __y > y0; __y--) {
                        target = ftarget + __y + __x - 1;
                        if (*(target + 1) == color && *target != color)
                             Key ^= 1;
                      if (Key)*(target + 1) = color;
                    }
                    Key = 0;
                }
            return 0;	
        }
        template <typename Plane, typename img_t>
        void fill (Plane box, img_t *image, Color color, int glow, int8_t direction = 1)
        {

                  register int32_t wm = image->W;
                  register int32_t hm = image->H;

                  Color *dest = this->frame;
                  const Color *src = image->Image;
                  int32_t S = hm * wm;
                  switch (direction) {
                      case 0:              
                            for (int xi = 0, xt = box.x * this->h; xi < wm; xt += this->h, xi++) {  
                                    for (uint32_t yt = box.y + xt, yi = S + xi - image->W; yi > xi; yt++, yi -= image->W) {
                                        if ((src[yi] != color) || (color == 0)) {
                                            dest[yt] = src[yi] | glow;   
                                        } else {}
                                    }
                            }
                            break;
                      case 1:              
                            for (int xi = 0, xt = box.x * this->h; xi < wm; xt += this->h, xi++) {  
                                    for (uint32_t yt = box.y + xt, yi = xi, t = S + xi; yi < t; yt++, yi += image->W) {
                                        if ((src[yi] != color) || (color == 0)) {
                                            dest[yt] = src[yi] | glow;   
                                        } else {}
                                    }
                            }
                            break;
                      case 2:              
                            for (int xi = wm - 1, xt = box.x * this->h; xi >= 0; xt += this->h, xi--) {  
                                    for (uint32_t yt = box.y + xt, yi = S + xi - image->W; yi > xi; yt++, yi -= image->W) {
                                        if ((src[yi] != color) || (color == 0)) {
                                            dest[yt] = src[yi] | glow;   
                                        } else {}
                                    }
                            }
                            break;
                      case 3:              
                            for (int xi = wm - 1, xt = box.x * this->h; xi >= 0; xt += this->h, xi--) {  
                                    for (uint32_t yt = box.y + xt, yi = xi, t = S + xi; yi < t; yt++, yi += image->W) {
                                        if ((src[yi] != color) || (color == 0)) {
                                            dest[yt] = src[yi] | glow;   
                                        } else {}
                                    }
                            }
                            break;
                      default :
                            break;
                  }
        }
                  
        template <typename img_t>
        void fill (int x, int y, img_t *image)
        {
                  register int32_t wm = image->W;
                  register int32_t hm = image->H;

                  Color *dest = this->frame;
                  const Color *src = image->Image;
                  register int32_t S = hm * wm;
                  for (int xi = 0, xt = x * this->h; xi < wm; xt += this->h, xi++) {  
                      for (uint32_t yt = y + xt, yi = S + xi - image->W; yi > xi; yt++, yi -= image->W) {
                          if (src[yi] != white) {
                              dest[yt] = src[yi];   
                          } else {}
                      }
                  }
        }

        template <typename img_t>
        int32_t fillScaled (int x, int y, img_t *image, int scaleX, int scaleY)
        {
                        register int32_t limitX = image->W;
                        register int32_t limitY = image->H;	
                        
                        register int32_t limitS = limitY * image->W;
                        register int32_t imageW = image->W;
            
                        register Color *target = this->frame;
                        register const Color* source = image->Image;
                        Color pixel;
            
                        
                        int32_t imageX = 0;
                        for (int32_t x0 = x * this->h; imageX < limitX; imageX++) { 
                                for (int32_t d = x0 + scaleX * this->h; x0 < d; x0 += this->h) {
                                        for (int32_t imageY = limitS - limitX, y0 = y; imageY >= 0; imageY -= limitX) {
                                            pixel = source[imageX + imageY];
                                            if (pixel != white) {
                                                for (int32_t d = y0 + scaleY; y0 < d; y0++) {
                                                            target[y0 + x0] = pixel;
                                                }
                                            } else {
                                                y0 = y0 + scaleY; 
                                            }
                                        }
                                }
                        }			
            return 0;
        }


    
};


#endif


/*End of file*/

