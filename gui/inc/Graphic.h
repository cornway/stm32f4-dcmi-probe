#ifndef GRAPHIC_CLASS
#define GRAPHIC_CLASS
#include "Gui_Defs.h"
#include "ViewPort.h"
#include "ArrayListStatic.h"
#include "drawString.h"
#include "gline.h"
#include "GFill.h"
#include "GCircle.h"
#include "GRectangle.h"
#include "GBitmap.h"



template <typename Color, typename Range, Color white>
class Graphic : public print::DrawString<Color, Range, white>,
                public GFill<Color, Range, white>,
                public GCircle<Color, Range>,
                public Line_Class<Color, Range>,
                public GRectangle<Color, Range>,
				public GBitmap<Color, Range, white> {
    private :
            
    
    
    
    public :
        Graphic (Color *f, Dimension<Range> d) :  print::DrawString <Color, Range, white>    (f, d),
                                                  GFill             <Color, Range, white>    (f, d),
                                                  GCircle           <Color, Range>           (f, d),
                                                  Line_Class        <Color, Range>           (f, d),
                                                  GRectangle        <Color, Range>           (f, d),
                                                  GBitmap           <Color, Range, white>    (f, d)
        {

        }
    
        virtual ~Graphic ()
        {
           // delete(this);
        }
        
        Color *getFrameBuf ()
        {
            return this->frame;
        }
    
};

#endif

/*End of file*/

