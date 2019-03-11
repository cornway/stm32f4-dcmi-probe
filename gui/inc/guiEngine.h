#ifndef GUI_ENGINE
#define GUI_ENGINE

#include "ContentPane.h"
#include "FontFactory.h"
#include "ViewPort.h"
#include <string.h>



template <typename Color, typename Range, Color white>
class GuiEngine {
    private :
        vector::Vector<GContentPane<Color, Range, white> > contentPanes;
        FontFactory<tFont, DefaultCharStorage_t> fontFactory;
        ViewPort<Range> viewPort;
    
        GContentPane<Color, Range, white> *selectedContentPane;
        Color *frame;
    
        FontCharSet<tFont, DefaultCharStorage_t> *defaultFontSet;
        int32_t selectedComponentId;
    
        template <typename C>
        int32_t fireSensorComponent (ArrayListBase<C> &list, TouchPointTypeDef arg, uint32_t cause)
        {
            uint32_t id = 0;
            list.foreach([&](C *c) -> void {
                id = c->fireSensor(arg, cause);
                if (id > 0) {
                    return;
                }
            });
            if (id > 0) {
                return id;
            }
            return -1;
        }

        template <typename C>
        void
        destroy (ArrayListBase<C> &array)
        {
            C *e = array.getFirst();
            C *ne = e->next();
            while (e != nullptr) {
                array.remove(e);
                delete e;
                e = ne;
                ne = ne->next();
            }
        }


    public :
        template<typename F>
        GuiEngine (const F &font, Color *f, int x, int y, int w, int h) :   contentPanes(),
                                                                            fontFactory()
        {
            viewPort.setSize(x, y, w, h);
            defaultFontSet = fontFactory.collectFont(&font);
            selectedComponentId = SilentComponent;
            this->frame = f;
        }
        
        ~GuiEngine ()
        {
            this->destroy(contentPanes);
            fontFactory.~FontFactory();
        }
        
        GContentPane<Color, Range, white> *newContentPane (Dimension<Range> d)
        {
            auto g =  new Graphic<Color, Range, white>(this->frame, d);
            auto contentPane =  new GContentPane<Color, Range, white>(g); 
            contentPane->getGraphic()->setDrawCharSet( defaultFontSet );
			selectedContentPane = contentPane;
            contentPanes.addFirst(contentPane);
            return contentPane;
        }
        
        GContentPane<Color, Range, white> *newContentPane (Range x, Range y, Range w, Range h)
        {
            Dimension<Range> d(x, y, w, h);
            return this->newContentPane(d);
        }
        
        void destroyPane (GContentPane<Color, Range, white> *pane)
        {
            this->contentPanes.remove(pane);
            delete pane;
        }
		
        GContentPane<Color, Range, white> *
        getSelectedContentPane ()
        {
            return this->selectedContentPane;
        }
				
        GContentPane<Color, Range, white> *
        getContentPane (int id)
        {
            if (id < 0) {
                return this->contentPanes.getFirst();
            }
            if (id > contentPanes.size()) {
                return this->contentPanes.getFirst();
            }
            return this->contentPanes.get(id);
        }
        
        ViewPort<Range> *
        getViewPort ()
        {
            return &this->viewPort;
        }
        
        Color *
        getFrameBuf ()
        {
            return this->frame;
        }
		
        /**/
        
        int fireSensor (TouchPointTypeDef arg, uint32_t cause)
        {
            if (selectedComponentId < 0) {
                //viewPort.move(arg.move);
            }
            //viewPort.setTrueView(arg.touchPoint);
            //arg.touchPoint = viewPort.getTrueView();
            selectedComponentId = fireSensorComponent(contentPanes, arg, cause);
            return 0;
        }

        /**/
};



#endif


/*End of file*/


