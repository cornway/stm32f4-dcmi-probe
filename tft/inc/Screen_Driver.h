#ifndef __SCREENDRIVER
#define __SCREENDRIVER
#include "gui_defs.h"

#ifdef tftili9486
#include "TFTili9486.h"
#else 
#error "please choose startup graphic controller"
#endif

#include "syncClass.h"
#include "viewPort.h"


template <typename GraphicController, typename FrameBuffer>
    class ScreenDriver : public Synchronizer<ScreenDriver <GraphicController, FrameBuffer> > {
        private :
        GraphicController *controller;
        char *name;
        public:
            ScreenDriver (GraphicController &c)
            {
                this->controller = &c;
            }
            ScreenDriver ()
            {
                
            }
            void operator () (GraphicController &c)
            {
                this->controller = &c;
            }
            
            void init (char *name)
            {
                this->name = name;
                this->controller->init(); 
            }
            void window (int x, int y, int w, int h)
            {
                this->controller->window(x, y, w, h);
            }
            
            template <typename Color>
            void fill (Color color)
            {
                //CriticalObject critical;
                Synchronize<ScreenDriver <GraphicController, FrameBuffer> > sync(this);
                if (sync.test() == false) {
                    return;
                }
                this->controller->driverFill(color);
            }
            void fill (FrameBuffer *buffer)
            {
                Synchronize<ScreenDriver <GraphicController, FrameBuffer> > sync(this);
                if (sync.test() == false) {
                    return;
                }
                controller->driverFill(buffer->getBuffer(), buffer->getBox());
            }
            template <typename Color>
            void fill (FrameBuffer *buffer, ViewPort &viewPort, Color color)
            {
                Synchronize<ScreenDriver <GraphicController, FrameBuffer> > sync(this);
                if (sync.test() == false) {
                    return;
                }
                ViewPort wp = viewPort;
                controller->driverFill(buffer->getBuffer(), buffer->getBox(), wp.getView(), color);
            }
            
            
            
            
            template <typename Image>
            void fill (Image &image)
            {
                Synchronize<ScreenDriver <GraphicController, FrameBuffer> > sync(this);
                if (sync.test() == false) {
                    return;
                }
            }
            template <typename Image>
            void fill (Image &image, int scaleX, int scaleY)
            {
                Synchronize<ScreenDriver <GraphicController, FrameBuffer> > sync(this);
                if (sync.test() == false) {
                    return;
                }
            }
        
    };

#endif /*__SCREENDRIVER*/



