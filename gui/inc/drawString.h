#ifndef DRAW_STRING_INTERFACE
#define DRAW_STRING_INTERFACE

#include "Dimensions.h"
#include "frame.h"
#include "gui_defs.h"


namespace print  {
  
enum TextDirection {
	UPWARD = 1,
	DOWNWARD = -1,
};


template <typename Color, typename Range, Color white>
    class DrawString  : public virtual Dimension<Range>,
                        public virtual Frame<Color> {
    private :
            Box<Range> caret;
            FontCharSet<tFont, DefaultCharStorage_t> *drawCharSet;
            const tChar *rawSet;
            
			int8_t direction;
            
            Box<Range> &putChar (Box<Range> box, char c, Color color)
            {
                const tImage<Color> *img = (tImage<Color> *)this->rawSet[ this->drawCharSet->get_UTF8( (uint16_t)c ) ].Image;
                drawChar(box, img, color);
                this->caret.x = img->W + box.x;
                this->caret.w -= img->W;
                return this->caret;
            }

            int32_t drawChar (Box<Range> box, const tImage<Color> *image, Color color)
            {

                int32_t wm = GUI_min(image->W, box.w);
                int32_t hm = GUI_min(image->H, box.h);

                Color *dest = this->frame;
                const Color *src = image->Image;
                
               
                int32_t S = hm * wm;
                for (int xi = 0, xt = box.x * this->h; xi < wm; xt += this->h, xi++) {  
                        for (int32_t yt = box.y + xt, yi = S + xi - wm; yi >= xi; yt++, yi -= wm) {
                            if (src[yi] != white) {
                                dest[yt] = color;   
                            } else {}
                        }
                }
                return 0;
            }
            
    public :
        DrawString (Color *f, Dimension<Range> d)
        {
            this->frame = f;
            this->setSize(d);
            this->caret.x = 0;
            this->caret.y = 0;
            this->caret.w = 0;
            this->caret.h = 0;
			this->direction = DOWNWARD;
            
        }
        
        ~DrawString ()
        {
            
        }
	
        Box<Range> &drawString (Box<Range> rect, char *str, Color color, bool scaled = false)
        {
                    this->rawSet = drawCharSet->getFont()->imagesSet;
					char character = '0';
					uint32_t index = 0;
					int fontWidth = this->drawCharSet->getW();
                    int fontHeight = this->drawCharSet->getH();
					switch (this->direction) {
						case UPWARD: 
										this->caret = rect;
										
										while (str[index] != 0) {
											character = str[index++];
											if (this->caret.x +  fontWidth > (rect.w + rect.x) || (character == '\n')) {
													this->caret.x = rect.x;
													this->caret.w = rect.w;
													this->caret.y += fontHeight;
													this->caret.h -= fontHeight;
													
											} 
											if (this->caret.y > rect.y + rect.h) {
													return this->caret;
											} 
											if ((character != '\n')) {				
													this->putChar(this->caret, character, color);		
											}				
										}
							break;
						case DOWNWARD:
										this->caret.x = rect.x;
										this->caret.w = rect.w;
										this->caret.y = rect.y + rect.h - fontHeight;
										this->caret.h = rect.h;
										
										while (str[index] != 0) {
											character = str[index++];
											if (this->caret.x + fontWidth * 2 > (rect.w + rect.x) || (character == '\n')) {
													this->caret.x = rect.x;
													this->caret.w = rect.w;
													this->caret.y -= fontHeight;
													this->caret.h -= fontHeight;
													
											} 
											if (this->caret.y < rect.y) {
													return this->caret;
											} 
											if ((character != '\n')) {				
													this->putChar(this->caret, character, color);		
											}				
										}
                                        this->caret.y = this->caret.y - this->caret.h + fontHeight;
							break;
					}
                    return this->caret;
        }
        
        
        void 
		setDrawCharSet (FontCharSet<tFont, DefaultCharStorage_t> *set)
        {
            this->drawCharSet = set;
        }
        
        uint32_t 
		getFontWidth ()
        {
            return drawCharSet->getW();
        }
        
        uint32_t 
		getFontHeight ()
        {
            return drawCharSet->getH ();
        }
        
        void 
		setDirection (TextDirection dir)
		{
			this->direction = dir;
		}
        
};

};
#endif 
