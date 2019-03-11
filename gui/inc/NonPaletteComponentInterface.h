#ifndef NON_PALETTE_INTERFACE
#define NON_PALETTE_INTERFACE
#include "graphic.h"
#include "gComponent.h"

template <typename Color, typename Range, Color white>
class NonPalette : public GComponent<Color, Range, white>,
                   public Link<NonPalette<Color, Range, white> >
{
private :
    void (*repaintHook) (NonPalette<Color, Range, white> *);
    Color selectColor;
public:
    NonPalette (Graphic<Color, Range, white> *graphic, void (*repaintHook) (NonPalette<Color, Range, white> *)) :
                                       GComponent<Color, Range, white> (graphic), 
                                       Link<NonPalette<Color, Range, white> > ()
    {
		this->repaintHook = repaintHook;
    }

    virtual ~NonPalette ()
    {
        //delete(this);
    }
    
 
    
    virtual void repaint ()
    {
        if (repaintHook == nullptr) {
            Dimension<Range> box = {this->x, this->y, this->w, this->h};
            this->graphic->fill(box, this->background | this->glow);
        } else {
            (*repaintHook)(this);
        }
    }
    
    void setSelectColor (Color color)
    {
        this->selectColor = color;
    }
    
}; /*class NonPaletteInterface*/





#endif

/*End of file*/

