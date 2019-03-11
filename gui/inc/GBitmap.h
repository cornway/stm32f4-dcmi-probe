#ifndef BITMAP_CLASS_H
#define BITMAP_CLASS_H

#ifdef SOFTWARE_GL

#include "Dimensions.h"
#include "frame.h"

#define PI_const (3.14159265359F)
#define PI2_const (3.14159265359F / 2.0F)
#define PI4_const (3.14159265359F / 4.0F)
#define PI32_const (3.14159265359F * 1.5F)


template <typename Color, typename Range, Color white> 
	class GBitmap : public virtual Dimension<Range>,
                    public virtual Frame<Color> {
    private :    
        
	public:
		GBitmap (Color *f, Dimension<Range> d)
		{
			this->frame = f;
            this->setSize(d);
		}
        
        ~GBitmap ()
        {
            
        }
	
        void setScale (Range sx, Range sy)
        {
            this->scale_x = sx;
            this->scale_y = sy;
        }
	
		template <typename Src>
	    int32_t 
		bitmapDraw (Dimension<Range> rect, Src *image, Color color = 0x0)
		{
            return this->DrawScaled (rect, image, color);
        }
		
		template <typename Src>
		int32_t 
		bitmapDraw (Range x0, Range y0, Src *img_src, Color color = 0x0)
        {
            Dimension<Range> rect;
            rect(x0, y0, 0, 0);
            return this->DrawScaled (rect, img_src, color);
        }
		
		template <typename Dest, typename Src>
	    int32_t bitmapCopy (Dimension<Range> rect, Dest *img_dest, Src *img_src, Color color = 0x0)
		{ 
			
			register int32_t x = rect.x * img_dest->H, xi = 0;
			register int32_t w = img_src->W;
			register int32_t h = img_src->H;
			
			register int32_t wd = img_dest->W;
				
			register const Color *src = img_src->Image;
			register Color *dest = img_dest->Image;
			
			register int32_t S = h * w;
			
			Color target = color;
			
			if (color != 0x0) {
				for (; xi < w; x++, xi++) {  
					for (int32_t y = rect.y, yi = 0; yi < S; y += wd, yi += w) {
						target = src[xi + yi];
						if (target != color) {
							dest[y + x] = target;   
						} else {}
					}
				}
			} else {	
				for (; xi < w; x++, xi++) {  
					for (int32_t y = rect.y, yi = 0; yi < S; y += wd, yi += w) {
							dest[y + x] = src[xi + yi];   
					}
				}
			}
			return 0;
		}
		
		
		template <typename Dest, typename Src>
	    int32_t 
		bitmapCopy (int32_t x0, int32_t y0, Dest *img_dest, Src *img_src, Color color = 0x0)
        {
            Dimension<Range> rect;
            rect(x0, y0, 0, 0);
            return this->Copy (rect, img_dest, img_src, color);
        }
		
		template <typename Dest, typename Src>
	    int32_t 
		bitmapCopy (Dest *img_dest, Src *img_src, Color color = 0x0)
		{
            int32_t x = (img_dest->W - img_src->W) / 2;
            int32_t y = (img_dest->H - img_src->H) / 2;
            Dimension<Range> rect;
            rect(x, y, 0, 0);
            return this->Copy (rect, img_dest, img_src, color);
        }
		
		template <typename Dest, typename Src>
	    Dest *
		bitmapRotate (Dest *img_dest, Src *img_src, float A, Color color = 0x0)
		{
            float D = ((float) (img_src->H > img_src->W ? img_src->H : img_src->W) * 2.0F );
            int32_t w = (int32_t)D;
            img_dest->W = w;
            img_dest->H = w;
            
            tImage<Color> *texture = new uint8_t[sizeof(tImage<Color>) + uint32_t(D * D)];
            texture->Image = (Color *)((uint32_t)texture + sizeof(tImage<Color>));
            texture->W = D;
            texture->H = D;
            this->bitmapFill(img_dest, white);
            this->bitmapCopy(img_dest, img_src, color);
            
            float t;
            t = A / (PI_const / 2);
            t = (abs(floor(t)));
            bool sign = false;
            if (A > 0) {
                A -= t * (PI_const / 2);
            } else {
                sign = true;
                A += t * (PI_const / 2);
            }
            this->Fill(&img_dest[1]);
            int32_t j = (int32_t)t % 4;
                if (j == 1) {
                    if (sign) {
                        this->Rotate270(texture, img_dest);
                    } else {
                      this->Rotate90(texture, img_dest);
                    }
                } else if (j == 2) {
                    this->Rotate180(texture, img_dest);
                } else if (j == 3){
                    if (sign) {
                        this->Rotate90(texture, img_dest);
                    } else {
                      this->Rotate270(texture, img_dest);
                    }
                } else {
                    this->Rotate0(texture, img_dest);
                }
            A = ((PI_const / 4) - A);
          
          Color *Output = img_dest->Image, *Input = texture->Image;		
                
          this->Fill(img_dest, white);
            this->ShiftRight(Output, Input, w, A / 2);
            this->Fill(texture, white);
          this->ShiftDown(Input, Output, w, A);
            this->Fill(img_dest, white);
          this->ShiftRight(Output, Input, w, A / 2);    
          this->Screen->KillTexture(texture);
            return img_dest;
        }
		
		template <typename Dest, typename Src>
	    Dest *
		bitmapRotate90 (Dest *img_dest, Src *img_src, Color color = 0x0)
		{
			register int32_t w = img_src->W;
			register int32_t S = w * w;
			register int32_t xi = 0;
			
			Color *target = img_dest->Image;
			Color *src = img_src->Image;
			
			for (; xi < S; xi += w) {  
				for (int32_t y = xi, yi = w + xi; yi > xi; y++, yi--) {
						target[y] = src[yi];   
				}
			}
			return img_dest;
		}
		
		
		template <typename Dest, typename Src>
	    Dest *
		bitmapRotate180 (Dest *img_dest, Src *img_src, Color color = 0x0)
		{
            int32_t w = img_src->W;
            int32_t S = w * w;
            int32_t x = 0, xi = w;
            
            Color *target = img_dest->Image;
            Color *src = img_src->Image;

            for (; xi > 0; x += w, xi--) {  
                for (int32_t y = x, yi = S + xi; yi > xi; y++, yi -= w) {
                        target[y] = src[yi];   
                    }
            }
            return img_src;
        }
		
		template <typename Dest, typename Src>
	    Dest *
        bitmapRotate270 (Dest *img_dest, Src *img_src, Color color = 0x0)
		{
            int32_t w = img_src->W;
            int32_t S = w * w;
            int32_t x = 0, xi = S;
            
            Color *target = img_dest->Image;
            Color *src = img_src->Image;
            
            for (; xi > 0; x += w, xi -= w) {  
                for (int32_t y = x, yi = xi, t = w + xi; yi < t; y++, yi++) {
                        target[y] = src[yi];   
                }
            }
            return img_dest;
        }
		
		template <typename Dest, typename Src>
	    Dest *
		bitmapRotate0 (Dest *img_dest, Src *img_src, Color color = 0x0)
        {
            int32_t w = img_src->W;
            int32_t S = w * w;
            int32_t x = 0, xi = 0;
        
            Color *target = img_dest->Image;
            Color *src = img_src->Image;
            
            for (; xi < w; x += w, xi++) {  
                for (uint32_t y = x, yi = xi, t = S + xi; yi < t; y++, yi += w) {
                        target[y] = src[yi];   
                }
            }
            return img_src;
		}
		template <typename Dest, typename Src>
	    Dest *
		bitmapFlipVertical (Dest *img_dest, Src *img_src, Color color = 0x0)
		{
			register int32_t w = img_src->W;
			register int32_t h = img_src->H;
			register int32_t hd = img_dest->H;
			register int32_t S = h * img_src->W;
			register int32_t x = 0, xi = w;
			
			Color *target = img_dest->Image;
			Color *src = img_src->Image;
			
			
			for (; xi > 0; x += hd, xi--) {  
				for (uint32_t y = 0, yi = 0; yi < S; y++, yi += w) {
						target[y + x] = src[xi + yi];   
				}
			}
			return img_dest;
		}
		template <typename Dest, typename Src>
	    Dest *
		bitmapFlipHorizontal (Dest *img_dest, Src *img_src, Color color = 0x0)
		{	
			register int32_t w = img_src->W;
			register int32_t h = img_src->H;
			register int32_t hd = img_dest->H;
			register int32_t S = h * img_src->W;
			register int32_t x = 0, xi = w;
			
			Color *target = img_dest->Image;
			Color *src = img_src->Image;
			
			for (; xi > 0; x += hd, xi--) {  
				for (uint32_t y = 0, yi = 0; yi < S; y++, yi += w) {
						target[y + x] = src[xi + yi];   
				}
			}
			return img_src;
		}
		
		template <typename Dest, typename Src>
	    Dest *
		bitmapShrinkX (Dest *img_dest, Src *img_src, float A, Color color = 0x0)
		{	
			register int32_t w = img_src->W;
			register int32_t h = img_src->H;
			register int32_t wd = img_dest->W;
			register int32_t S = h * w;
			register int32_t x = 0, xi = 0;
			float Ax = 0, An = 0;
			
			Color *target = img_dest->Image;
			const Color *src = img_src->Image;

			/*
					for (; xi < w; x++, xi++) {  
					for (uint32_t y = rect.y, yi = 0; yi < S; y += wd, yi += w) {
							dest[y + x] = src[xi + yi];   
					}
				}
			*/
			for (x = 0, xi = 0; xi < S; xi += w) {  
				Ax += A;
				if ((Ax - An) > 1.0F) {
					An += 1.0F;
					for (int32_t y = x, yi = xi, t = h + xi; yi < t; y += wd, yi++) {
							target[y] = src[yi];   
					}
					x++;
				} else {}
			}
			return img_dest;
		}
		template <typename Dest, typename Src>
	    Dest *
		bitmapShrinkY (Dest *img_dest, Src *img_src, float A, Color color = 0x0)
		{
			register int32_t w = img_src->W;
			register int32_t h = img_src->H;
			register int32_t wd = img_dest->W;
			register int32_t S = h * img_src->W;
			register int32_t x = 0, xi = 0;
			float Ay = 0, An = 0;
			
			
			Color *target = img_dest->Image;
			const Color *src = img_src->Image;

			for (int32_t y = 0, yi = 0; yi < S; yi += w) {  
				Ay += A;
				if ((Ay - An) > 1.0F) {
					An += 1;
					for (x = y, xi = yi; xi < w + yi; x++, xi++) {
							target[x] = src[xi];   
					}
					y += wd;
				} else {}
			}
			return img_dest;
		}
		template <typename Dest, typename Src>
	    Dest *
		bitmapExpandX (Dest *img_dest, Src *img_src, float A, Color color = 0x0)
		{
			register int32_t w = img_src->W;
			register int32_t h = img_src->H;
			register int32_t wd = img_dest->W;
			register int32_t S = h * w;
			register int32_t x = 0, xi = 0;
			
			if (A != 0) {
			  A = (1.0F - 1 / A);
			}
			float Ax = 0, An = 0;
			
			Color *target = img_dest->Image;
			const Color *src = img_src->Image;
			
			for (; xi < w; x++) {  
			  Ax += A;
			  if ((Ax - An) > 1.0F) {
					An += 1.0F;
			  } else {
					xi++;
			  }
			  for (uint32_t y = x, yi = xi, t = S + xi; yi < t; y += wd, yi += w) {
							target[y] = src[yi];   
			  }
			}
			return img_dest;
		}
		template <typename Dest, typename Src>
	    Dest *
		bitmapExpandY (Dest *img_dest, Src *img_src, float A, Color color = 0x0)
		{
			register int32_t w = img_src->W;
			register int32_t h = img_src->H;
			register int32_t wd = img_dest->W;
			register int32_t S = h * w;
			register int32_t x = 0, xi = 0;
			
			if (A != 0) {
			  A = (1.0F - 1 / A);
			}
			float Ax = 0, An = 0;
			
			Color *target = img_dest->Image;
			const Color *src = img_src->Image;

			for (uint32_t y = 0, yi = 0; yi < S; y += wd){  
			  Ax += A;
			  if ((Ax - An) > 1.0F) {
					An += 1.0F;
			  } else {
					yi += w;
			  }
			  for (x = y, xi = yi; xi < w + yi; x++, xi++) {
			  	  target[x] = src[xi];   
			  }
			}
			return img_dest;
		}
		
		template <typename Dest, typename Src>
	    Dest *
		bitmapResizeX (Dest *img_dest, Src *img_src, float A, Color color = 0x0)
		{
			if (A > 1.0F) {
				return this->bitmapExpandX(img_dest, img_src, A, color);
			} else {
				return this->bitmapShrinkX(img_dest, img_src, A, color);
			}
		}
		
		template <typename Dest, typename Src>
	    Dest *
		bitmapResizeY (Dest *img_dest, Src *img_src, float A, Color color = 0x0)
		{
			if (A > 1.0F) {
				return this->bitmapExpandY(img_dest, img_src, A, color);
			} else {
				return this->bitmapShrinkY(img_dest, img_src, A, color);
			}
		}
		
		template <typename Dest, typename Src>
	    Dest *
		bitmapResize (Dest *img_dest, Src *img_src, float A, float B, Color color = 0x0)
		{
			int32_t w = (int32_t)abs(floor((float)img_src->W * A));
			int32_t h = (int32_t)abs(floor((float)img_src->H * B));
			Dimension<Range> rect; 
			rect(0, 0, w, h);
			return this->bitmapResize(rect, img_dest, img_src, color);
		}
		
		template <typename Dest, typename Src>
	    Dest *
		bitmapResize (Dimension<Range> rect, Dest *img_dest, Src *img_src, Color color = 0x0)
		{
			tImage<Color> *texture = new uint8_t[sizeof(tImage<Color>) + uint32_t(img_src->H * rect.w)];
            texture->Image = (Color *)((uint32_t)texture + sizeof(tImage<Color>));
				
			texture->H = img_src->H;
			texture->W = rect.w;
			img_dest->W = rect.w;
			img_dest->H = rect.h;
			float A = (float)rect.w / (float)img_src->W;
			float B = (float)rect.h / (float)img_src->H;
			
			this->bitmapFill(texture);	
			this->bitmapResizeX(texture, img_src, A);	
			this->bitmapFill(img_dest);
			this->bitmapResizeY(img_dest, texture, B);	
			
			delete(texture);
			
			return img_dest;
		}
		
		template <typename Src>
	    int32_t 
		bitmapTile (Dimension<Range> rect, Src *img_src, Color color = 0x0)
		{
            int32_t x0 = rect.x;
            int32_t y0 = rect.y;
            int32_t w  = rect.w + x0;
            int32_t h  = rect.h + y0;
            int32_t img_w = img_src->W;
            int32_t img_h = img_src->H;
            int32_t cut_x = 0;
            int32_t cut_y = 0;
            int32_t h0 = this->H;
            int32_t t_y;
            
            Color *target = this->frame;
            Color *src = img_src->Image;
            if (color == 0x0) {
                for (; x0 < w; x0 += img_w) {
                        t_y = y0;
                    for (; y0 < h; y0 += img_h) {
                            uint32_t __x = x0 * h0;
                            uint32_t xi = 0;	
                            uint32_t i = w - x0;
                            if (i < img_w) {
                                    cut_x = i;
                            } else {
                                    cut_x = img_w;
                            }
                            i = h - y0;
                            if (i < img_h) {
                                 cut_y = i;
                            } else {
                            cut_y = img_h;
                            }
                            uint32_t S = cut_y * img_w;
                            for (; xi < cut_x; __x += h0, xi++) {  
                                    for (uint32_t __y = y0, yi = 0; yi < S; __y++, yi += img_w) {
                                            target[__y + __x] = src[xi + yi];   
                                    }
                            }
                    }
                    y0 = t_y;
                }
          } else {
                for (Color tcol = 0; x0 < w; x0 += img_w) {
                        t_y = y0;
                    for (; y0 < h; y0 += img_h) {
                            uint32_t __x = x0 * h0;
                            uint32_t xi = 0;	
                            uint32_t i = w - x0;
                            if (i < img_w) {
                                    cut_x = i;
                            } else {
                                    cut_x = img_w;
                            }
                            i = h - y0;
                            if (i < img_h) {
                                 cut_y = i;
                            } else {
                            cut_y = img_h;
                            }
                            uint32_t S = cut_y * img_w;
                            for (; xi < cut_x; __x += h0, xi++) {  
                                    for (uint32_t __y = y0, yi = 0; yi < S; __y++, yi += img_w) {
                                          tcol = src[xi + yi];
                                          if (tcol != color) {
                                                target[__y + __x] = tcol;
                                            } else {}									
                                    }
                            }
                    }
                    y0 = t_y;
                }		
            }
            return 0;
        }
        

		
		template <typename Dest>
		int32_t 
		bitmapFill (Dest *img_dest, Color color = 0x0)
		{
            Color *target = img_dest->Image;
            for (int32_t D = img_dest->W * img_dest->H; D > 0; D--) {
                target[D] = color;
            }
            return 0;
        }
		
            void SetScale (Dimension<Range>);
            void SetScale (float, float);
		
	private:
	  	
	
		template <typename img_t>
	    int32_t 
		bitmapDrawScaled (Dimension<Range> rect, img_t *image, Color color = 0x0)
		{
			if (!this->scale_x || !this->scale_y) {
				  return this->DrawNoScale(rect, image, color);
			}
			
			rect.w = image->W * this->scale_x;
			rect.h = image->H * this->scale_y;
			
			register int32_t h  = this->H;
			register int32_t ys = this->scale_y;
			register int32_t xs = this->scale_x;
			register int32_t xt = rect.w / xs;
			register int32_t yt = rect.h / ys;	
			
			register int32_t S = yt * image->W;
			register int32_t ws = image->W;
			
			register Color *target = this->Frame;
			register const Color *source = image->Image;
			
			Color targ = color;
			
			if (color == 0x0) {
				for (int32_t xi = 0, x = rect.x * h; xi < xt; xi++) { 
						for (int32_t d = x + xs * h; x < d; x += h) {
								for (int32_t y = rect.y + 1, yi = 0; yi < S; yi += ws) {
										for (int32_t d = y + ys; y < d; y++) {
											 target[y + x] = source[xi + yi]; 
										}
								}
						}
				}	
			}	else {
				for (int32_t xi = 0, x = rect.x * h; xi < xt; xi++) { 
						for (int32_t d = x + xs * h; x < d; x += h) {
								for (int32_t y = rect.y + 1, yi = 0; yi < S; yi += ws) {
										for (int32_t d = y + ys; y < d; y++) {
											  targ = source[xi + yi];
											  if (targ != color) {
													target[y + x] = targ;
												} else {}	
										}
								}
						}
				}			
			}
			return 0;
		} /*bitmapDrawScaled */
		
		template <typename img_t>
	    int32_t 
		bitmapDrawNoScale (Dimension<Range> rect, const img_t *image, Color color = 0x0)
		{
			
			register int32_t gh = this->h;
			register int32_t x = rect.x * gh, xi = 0;
			register int32_t w = GUI_min(rect.w, image->W);
			register int32_t h = GUI_min(rect.h, image->H);
			register int32_t y0 = rect.y;
			register int32_t iw = image->W;
			register int32_t S = h * iw;
			
			Color *dest = this->frame;
			const Color *src = image->Image;
			
			
			Color target = color;
			if (color != 0x0) {
				for (; xi < w; x += gh, xi++) {  
					for (int32_t y = y0 + x, yi = xi, t = S + xi; yi < t; y++, yi += iw) {
						target = src[yi];
						if (target != color) {
							dest[y] = target;   
						} else {}
					}
				}
			} else {
				for (; xi < w; x += gh, xi++) {  
					for (int32_t y = y0 + x, yi = xi, t = S + xi; yi < t; y++, yi += iw) {
							dest[y] = src[yi];   
					}
				}
			}
			
			return 0;
		}
		
		template <typename Dest, typename Src>
	    Dest *ShiftRight (Dest *dest, Src *src, int32_t w, float A, Color color = 0x0)
		{
			register int32_t S = w * w;
			register int32_t C = (int32_t)((float)(w / 2) * A) * w;
			float j;
			
			register int32_t xi = 0;
			register int32_t yi = 0;
			register int32_t t = 0;
			for (yi = 0, j = 0; yi < w; yi++, j += A) {  
					for (xi = yi, t = S + yi; xi < t; xi += w) {
							dest[xi - (int32_t)(j) * w + C] = src[xi];   
					}
			}
			return dest;
		}
		template <typename Dest, typename Src>
	    Dest *bitmapShiftLeft (Dest *img_dest, Src *img_src, uint32_t w, float A, Color color = 0x0)
		{
			return this->ShiftRight(img_dest, img_src, w, -A, color);
		}
		template <typename  Dest, typename Src>
	    Dest *
		ShiftUp (Dest *img_dest, Src *img_src, uint32_t w, float A, Color color = 0x0)
		{
			return this->ShiftDown(img_dest, img_src, w, -A, color);
		}
		
		template <typename  Dest, typename Src>
	    Dest *
		bitmapShiftDown (Dest *dest, Src *src, uint32_t w, float A, Color color = 0x0)
		{
			register int32_t S = w * w;
			register int32_t C = -(int32_t)((float)(w / 2) * A);
			
			register int32_t xi = S;
			for (float j = 0; xi > 0; xi -= w, j -= A) {  
				for (int32_t c = ((int32_t)(j) - C), t = w + xi, yi = xi; yi < t; yi++) {
						dest[yi + c] = src[yi];   
				}
			}
			return dest;
		}
	  
		uint8_t scale_x, scale_y;		
};




#else /*!SOFTWARE_GL*/

#endif /*SOFTWARE_GL*/

#endif /*BITMAP_CLASS_H*/

