#ifndef TEXT_FIELD_OBJECT
#define TEXT_FIELD_OBJECT

#include "graphic.h"
#include "printer.h"
#include "gComponent.h"
#include "gComponent.h"



template <typename Color, typename Range, Color white>
class GTextField : public GComponent<Color, Range, white>,
                   public Link<GTextField<Color, Range, white> >,
                   public std_printer::Printer<char, '\0', '\n', '0', '-'>       {
private :
    int cursor;
    Color cursorColor;
    Color selectColor;
    bool cursorVisible;
public:
    GTextField (Graphic<Color, Range, white> *graphic, int textSize) : 
                                                         GComponent<Color, Range, white> (graphic), 
                                                         Link<GTextField<Color, Range, white> > (),
                                                         std_printer::Printer<char, '\0', '\n', '0', '-'> (textSize)
    {
        this->background = white;
        this->cursorColor = white;
        this->cursorVisible = true;
    }

    ~GTextField ()
    {
        
    }

    virtual void repaint () final
    {
        Box<Range> box = {this->x, this->y, this->w, this->h};
        this->graphic->fill(box, this->background);
        
        box = this->graphic->drawString(box, this->getText(), this->foreground);
        if (this->cursorVisible == true) {
            this->graphic->drawString(box, "<-", this->cursorColor);
        }
    }
    
    void setCursorColor (Color color)
    {
        this->cursorColor = color;
    } 

    void setSelectColor (Color color)
    {
        this->selectColor = color;
    }
    void setCursorVisible (bool visible)
    {
        this->cursorVisible = visible;
    }
    
};



#endif

/*End of file*/

