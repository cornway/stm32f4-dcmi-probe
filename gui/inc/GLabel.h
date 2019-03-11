#ifndef GLABEL_OBJECT
#define GLABEL_OBJECT
#include "graphic.h"
#include "printer.h"
#include "gComponent.h"



template <typename Color, typename Range, Color white>
class GLabel : public GComponent<Color, Range, white>,
               public Link<GLabel<Color, Range, white> >,
               public std_printer::Printer<char, '\0', '\n', '0', '-'>      {
private :
    uint8_t image_or;
    Color selectColor;
public:
    GLabel (Graphic<Color, Range, white> *graphic, int textSize) :  GComponent<Color, Range, white> (graphic), 
                                                                    Link<GLabel<Color, Range, white> > (),
                                                                    std_printer::Printer<char, '\0', '\n', '0', '-'> (textSize)
    {
		this->opaqueLevel = 100;
        this->glow = 0;
        this->image_or = 1;
        this->textAllign = GTEXT_ALLIGN_NONE;
    }
		
	uint8_t opaqueLevel;
    uint8_t textAllign;
		

    ~GLabel ()
    {
        
    }
    
    virtual void repaint () final
    {
        Box<Range> box = {this->x, this->y, this->w, this->h};
		if (this->opaqueLevel > 0) {
			this->graphic->fill(box, this->background | this->glow);
		}
        if (this->icon.Image != nullptr) {
            Dimension<Range> id(0, 0, this->icon.W, this->icon.H);
            id.setOrigins(this->getOrigins());
            this->graphic->fill(id.getBox(), &this->icon, this->maskColor, this->glow, this->image_or);
        }
        Dimension<Range> dbox;
        Box<Range> tbox;
        if (this->getTextSize() > 0) {
            switch (this->textAllign) {
                case GTEXT_ALLIGN_NONE :    this->graphic->drawString(box, this->getText(), this->foreground);
                    break;
                case GTEXT_ALLIGN_TOP :     this->graphic->drawString(box, this->getText(), this->foreground);
                    break;
                case GTEXT_ALLIGN_BOTTOM :  this->graphic->drawString(box, this->getText(), this->foreground);
                    break;
                case GTEXT_ALLIGN_LEFT :    this->graphic->drawString(box, this->getText(), this->foreground);
                    break;
                case GTEXT_ALLIGN_RIGHT :   this->graphic->drawString(box, this->getText(), this->foreground);
                    break;
                case GTEXT_ALLIGN_CENTER :
                                            tbox = this->getTextBox(this->getText(), this->graphic->getFontWidth(), this->graphic->getFontHeight());
                                            dbox = tbox;
                                            dbox.setOrigins(this->getOrigins());
                                            tbox.x = dbox.x;
                                            tbox.y = dbox.y;
                                            this->graphic->drawString(tbox, this->getText(), this->foreground);
                    break;
                default :
                    break;
            }
            
        }
    }
		
    void setOpaque (int level)
    {
        this->opaqueLevel = level;
    }
    
		
    void setImageOr (uint8_t orintation)
    {
        this->image_or = orintation;
    }
    
    void setSelectColor (Color color)
    {
        this->selectColor = color;
    }
		
    void setTextAllign (uint32_t allign)
    {
        this->textAllign = allign;
    }
    
};
               
#endif

/*End of file*/

/**/


