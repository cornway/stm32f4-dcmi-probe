#ifndef G_CONTENT_PANE
#define G_CONTENT_PANE

#include <string.h>

#include "gui_defs.h"
#include "iterable.h"
#include "graphic.h"
#include "GLabel.h"
#include "GSlide.h"
#include "GKeypad.h"
#include "textField.h"
#include "NonPaletteComponentInterface.h"
#include "GProgressBar.h"
#include "GBrowser.h"
#include "GDialog.h"
#include "GAlert.h"
#include "touch.h"

template <typename Color, typename Range, Color white>
class GPad : public Link<GPad<Color, Range, white> > , 
             public Dimension<Range> {
    private :
        Color color;
        Graphic<Color, Range, white> *graphic;
    public :
        GPad (Graphic<Color, Range, white> *g, Color color, Range x, Range y, Range w, Range h)
        {
            this->color = color;
            this->setSize(x, y, w, h);
            this->graphic = g;
        }
        
        void repaintComponent ()
        {
            this->graphic->fill(this->getBox(), this->color);
        }
        
        bool isVisible ()
        {
            return true;
        }
};

enum ComponentTypeId {
    SilentComponent = -1,
    ComponentLabelTypeId = 0,
    ComponentSliderTypeId = 1,
    ComponentTextFieldTypeId = 2,
    ComponentProgressBarTypeId = 3,
    ComponentBrowserTypeId = 4,
    ComponentDialogTypeId = 5,
    ComponentAlertTypeId = 6,


    NonPaletteComponentTypeId = 33, 
    KeyPadTypeId = 33,    
};



typedef struct {
	ComponentTypeId id;
	void *component;
} ComponentDsc;

#define ACCESS_COMPONENT(ret, dsc, action, args ...) \
do { \
    switch (dsc.id) { \
        case SilentComponent : \
            break; \
        case ComponentLabelTypeId : ret = ((GLabel<Color, Range, white> *)dsc.component)->action(args); \
            break; \
        case ComponentSliderTypeId : ret = ((GSlide<Color, Range, white> *)dsc.component)->action(args); \
            break; \
        case ComponentTextFieldTypeId : ret = ((GTextField<Color, Range, white> *)dsc.component)->action(args); \
            break; \
        case ComponentProgressBarTypeId : ret = ((GProgressBar<Color, Range, white> *)dsc.component)->action(args); \
            break; \
        case ComponentBrowserTypeId : ret = ((GBrowser<Color, Range, white> *)dsc.component)->action(args); \
            break; \
        case NonPaletteComponentTypeId : ret = ((NonPalette<Color, Range, white> *)dsc.component)->action(args); \
            break; \
    }; \
    } \
    while (0)
        
#define ACCESS_VCOMPONENT(dsc, action, args ...) \
do { \
    switch (dsc.id) { \
        case SilentComponent : \
            break; \
        case ComponentLabelTypeId : ((GLabel<Color, Range, white> *)dsc.component)->action(args); \
            break; \
        case ComponentSliderTypeId : ((GSlide<Color, Range, white> *)dsc.component)->action(args); \
            break; \
        case ComponentTextFieldTypeId : ((GTextField<Color, Range, white> *)dsc.component)->action(args); \
            break; \
        case ComponentProgressBarTypeId : ((GProgressBar<Color, Range, white> *)dsc.component)->action(args); \
            break; \
        case ComponentBrowserTypeId : ((GBrowser<Color, Range, white> *)dsc.component)->action(args); \
            break; \
        case NonPaletteComponentTypeId : ((NonPalette<Color, Range, white> *)dsc.component)->action(args); \
            break; \
    }; \
    } \
    while (0)
		

#define GET_FROM_COMPONENT(dsc, action, value) \
do { \
    switch (dsc.id) { \
        case SilentComponent : \
            break; \
        case ComponentLabelTypeId : value = ((GLabel<Color, Range, white> *)dsc.component)->action(); \
            break; \
        case ComponentSliderTypeId : value = ((GSlide<Color, Range, white> *)dsc.component)->action(); \
            break; \
        case ComponentTextFieldTypeId : value = ((GTextField<Color, Range, white> *)dsc.component)->action(); \
            break; \
        case ComponentProgressBarTypeId : value = ((GProgressBar<Color, Range, white> *)dsc.component)->action(); \
            break; \
        case ComponentBrowserTypeId : value = ((GBrowser<Color, Range, white> *)dsc.component)->action(); \
            break; \
        case NonPaletteComponentTypeId : value = ((NonPalette<Color, Range, white> *)dsc.component)->action(); \
            break; \
    }; \
    } \
    while (0)

typedef struct {
    int num;
    void *component;
} ComponentNum;

template <typename Color, typename Range, Color white>
	class GuiEngine;

class Component : public Link<Component> {
	private :
		ComponentDsc dsc;
	public :
		Component (void *component, ComponentTypeId id) : Link<Component> () 
		{
			this->dsc.component = component;
			this->dsc.id = id;
		}
		
		ComponentDsc
		get ()
		{
			return this->dsc;
		}
};

template <typename Color, typename Range, Color white>
  class GContentPane : public GComponent<Color, Range, white>,
                       public Link<GContentPane<Color, Range, white> > {
    private :
        Color *frame;
		
		const tImage<Color> *cursorIcon;
        Point<Range> cursorPos;
		
        vector::Vector<GLabel<Color, Range, white> > gLabels;
        vector::Vector<GSlide<Color, Range, white> > gSliders;
        vector::Vector<GTextField<Color, Range, white> > gTextFields;
        vector::Vector<GProgressBar<Color, Range, white> > gProgressBars;
        vector::Vector<GBrowser<Color, Range, white> > gBrowsers;
        vector::Vector<NonPalette<Color, Range, white> > gComponents;
        vector::Vector<GPad<Color, Range, white> > gPads;
        
        GKeypad<Color, Range, white> *keyPad;
        GDialog<Color, Range, white> *dialog;
        GAlert<Color, Range, white> *alert;
    
        array::Array<ComponentDsc> *packedArray;
    
    
    
        ComponentTypeId selectedComponentId;
        int32_t selectedComponentType;
        void *selectedComponent;
        int elementsCount;
		bool packed;
        bool wievPortDisabled;
    
        template <typename Element>
        void
        destroy (ArrayListBase<Element> *array)
        {
            Element *e = array->getFirst();
            Element *ne = e->next();
            while (e != nullptr) {
                array->remove(e);
                delete e;
                e = ne;
                ne = ne->next();
            }
        }
		
		template <typename Component>
		void *
        checkComponentName (ArrayListBase<Component> *array, const char *name)
		{
			Component *c = array->getFirst();
			while (c != nullptr) {
				if ( strcmp(name, c->getName()) == 0) {
					return (void *)c;
				}
				c = c->next();
			}
			return nullptr;
		}
        
        template <typename Component>
        ComponentNum
        checkComponentNum (ArrayListBase<Component> *array, int num)
        {
            ComponentNum cNum = {num, nullptr};
            cNum.component = array->get(num);
            if (cNum.component == nullptr) {
                cNum.num = num - array->size();
                return cNum;
            }
            cNum.num = 0;
            return cNum;
        }
		
        bool test (ViewPort<Range> &w,  Box<Range> p)
        {
            if (w.testTrueViewPoint( p.x, p.y ) == true) {
                    return true;
            }
            if (w.testTrueViewPoint( p.x + p.w, p.y ) == true) {
                    return true;
            }
            if (w.testTrueViewPoint( p.x, p.y + p.h ) == true) {
                    return true;
            }
            if (w.testTrueViewPoint( p.x + p.w, p.y + p.h ) == true) {
                    return true;
            }
            return false;
        }
        
        template <typename C, typename S>
        bool testInvisible (C *c, S *s)
        {
            if (s->testPoint(c->x, c->y) == false) {
                return false;
            }
            if (s->testPoint(c->x + c->w, c->y) == false) {
                return false;
            }
            if (s->testPoint(c->x, c->y + c->h) == false) {
                return false;
            }
            if (s->testPoint(c->x + c->w, c->y + c->h) == false) {
                return false;
            }
            return true;
        }
    
        template <typename Element>
        void repaint (ArrayListBase<Element> *list, ViewPort<Range> &w)
        {
            Element *it = list->getFirst();
            bool repaintFlag = true;
            while (it != nullptr) {
                repaintFlag = true;
                if (it->isVisible() == true) {
                    if ((this->test(w, it->getBox()) == true) || (this->wievPortDisabled == true)) {
                        if (this->dialog != nullptr) {
                            if (this->dialog->isOpened() == true) {
                                if (this->testInvisible(it, this->dialog) == false) {
                                    repaintFlag = true;
                                } else {
                                    repaintFlag = false;
                                }
                            }
                        }
                        if (repaintFlag == true) {
                            if (this->alert != nullptr) {
                                if (this->alert->isOpened() == true) {
                                    if (this->testInvisible(it, this->alert) == false) {
                                        repaintFlag = true;
                                    } else {
                                        repaintFlag = false;
                                    }
                                }
                            }
                        }
                        if (repaintFlag == true) {
                            it->repaintComponent();
                        }
                    }
                }
                it = it->next();
            }
        }
        
        template <typename Element>
        void wakeUp (ArrayListBase<Element> *components)
        {
            Element *it = components->getFirst();
            while (it != nullptr) {
                it->wakeUp();
                it = it->next();
            }
        }
        
        template <typename Element>
        int fireSensorPane(ArrayListBase<Element> *components, TouchPointTypeDef arg, uint32_t cause)
        {
            if (this->testPoint(arg.point) == false) {
                return -1;
            }
            selectedComponentId = (ComponentTypeId)-1;
            Element *e = components->getFirst();
            while (e != nullptr) {
                selectedComponentId = (ComponentTypeId)e->fireSensorComponent(arg, cause);
                if (selectedComponentId > 0) {
                    selectedComponent = e;
                    e->setSelectColor(GET_GREY_16BPP(100));
                    return selectedComponentId;
                }
                e = e->next();
            }
            return selectedComponentId;
        }

        GContentPane (Graphic<Color, Range, white> *g) :    GComponent<Color, Range, white> (g),
                                                            Link<GContentPane<Color, Range, white> >(),
                                                            gLabels(),
                                                            gSliders(),
                                                            gTextFields(),
                                                            gProgressBars(),
                                                            gBrowsers(),
                                                            gComponents()
        {
            frame = g->getFrameBuf();
            this->setSize(g->getBox());     
            cursorPos.x = 0;
            cursorPos.y = 0;
            this->selectedComponentId = SilentComponent;
            this->selectedComponentType = SilentComponent;
            this->selectedComponent = nullptr;
            this->elementsCount = 0;
			this->packed = false;
            this->cursorIcon = nullptr;
            this->keyPad = nullptr;
            this->alert = nullptr;
            this->dialog = nullptr;
            this->wievPortDisabled = true;
        }
        
        ~GContentPane ()
        {
            this->unPack();
            destroy(&gTextFields);
            destroy(&gProgressBars);
            destroy(&gBrowsers);
            destroy(&gSliders);
            destroy(&gLabels);
            destroy(&gComponents);
            destroy(&gPads);
            if (this->keyPad != nullptr) {
                delete this->keyPad;
            }
            if (this->alert != nullptr) {
                delete this->alert;
            }
            if (this->dialog != nullptr) {
                delete this->dialog;
            }
        }
        friend class GuiEngine<Color, Range, white>;
    public :
        
		void
		pack ()
		{	
            if (this->packed == false) {
                this->packedArray = new array::Array<ComponentDsc>(this->elementsCount);
                gLabels.foreach([&](GLabel<Color, Range, white> *component) ->void {
                    ComponentDsc dsc = {ComponentLabelTypeId, component};
                    this->packedArray->push(dsc);
                });
                gSliders.foreach([&](GSlide<Color, Range, white> *component) ->void {
                    ComponentDsc dsc = {ComponentSliderTypeId, component};
                    this->packedArray->push(dsc);
                });
                gTextFields.foreach([&](GTextField<Color, Range, white> *component) ->void {
                    ComponentDsc dsc = {ComponentTextFieldTypeId, component};
                    this->packedArray->push(dsc);
                });
                gProgressBars.foreach([&](GProgressBar<Color, Range, white> *component) ->void {
                    ComponentDsc dsc = {ComponentProgressBarTypeId, component};
                    this->packedArray->push(dsc);
                });
                gBrowsers.foreach([&](GBrowser<Color, Range, white> *component) ->void {
                    ComponentDsc dsc = {ComponentBrowserTypeId, component};
                    this->packedArray->push(dsc);
                });
                gComponents.foreach([&](NonPalette<Color, Range, white> *component) ->void {
                    ComponentDsc dsc = {NonPaletteComponentTypeId, component};
                    this->packedArray->push(dsc);
                });
                this->packed = true;
            } else {
                this->packed = false;
                delete this->packedArray;
                this->pack();
            }
		}
		
		void
		unPack ()
		{
            if (this->packed == false) {
                return;
            }
			this->packed = false;
            delete this->packedArray;
		}
        
        
        void 
		fill (Color color)
        {
            this->graphic->fill(color);
        }
        
        void 
		fill ()
        {
            this->graphic->fill(this->background);
        }
        
        
        GDialog<Color, Range, white> *
        newDialog (char *name, int size, Range w, Range h)
        {
            if (this->dialog != nullptr) {
                return this->dialog;
            }
            Dimension<Range> d(0, 0, w, h);
            d.setOrigins(this->getOrigins());
            GDialog<Color, Range, white> *dialog = new GDialog<Color, Range, white>(name, size, this->graphic, d.getBox());
            this->dialog = dialog;
            return this->dialog;
        }
        
        void openDialog (   
                            char *text,
                            void (*acceptListener) (abstract::Event e), 
                            void (*declineListener) (abstract::Event e)
                         )
        {
            if (this->dialog == nullptr) {
                return;
            }
            this->dialog->open (text, acceptListener, declineListener);
        }
        
                
        GAlert<Color, Range, white> *
        newAlert (char *name, int size, Range w, Range h)
        {
            if (this->alert != nullptr) {
                return this->alert;
            }
            Dimension<Range> d(0, 0, w, h);
            d.setOrigins(this->getOrigins());
            GAlert<Color, Range, white> *alert = new GAlert<Color, Range, white>(name, size, this->graphic, d.getBox());
            this->alert = alert;
            return this->alert;
        }
        
        void openAlert (char *text, void (*acceptListener) (abstract::Event e))
        {
            if (this->alert == nullptr) {
                return;
            }
            this->alert->alert(text, acceptListener);
        }
        
        template <typename C, int textSize> 
        C *
		createComponent (char *name, char *text, Range x, Range y, Range w, Range h)
        {
            auto c = (C *) new C(this->graphic, textSize);
            c->setSize(x, y, w, h);
            c->setName(name);
            c->setText(text);
            c->setBackground(white);
            c->setForeground(0x00);
            return c;
        }
        
        template <typename C> 
        C *
		createComponent (int textSize, char *name, char *text, Range x, Range y, Range w, Range h)
        {
            auto c = (C *) new C(this->graphic, textSize);
            c->setSize(x, y, w, h);
            c->setName(name);
            c->setText(text);
            c->setBackground(white);
            c->setForeground(0x00);
            return c;
        }
				
		template <typename C, int textSize> 
        C *
		createComponent (Dimension<Range> &d)
        {
           return this->createComponent<C, textSize>("", "", d.x, d.y, d.w, d.h);
        }
			
        template <typename C, int textSize> 
        C *
		createComponent (char *name, char *text, Dimension<Range> &d)
        {
           return this->createComponent<C, textSize>(name, text, d.x, d.y, d.w, d.h);
        }
        
        template <typename C, int textSize> 
        C *
		createComponent (char *name, char *text)
        {
           return this->createComponent<C, textSize>(name, text, 0, 0, 0, 0);
        }
        
        template <typename C, int textSize> 
        C *
		createComponent ()
        {
            auto c = (C *) new C(this->graphic, textSize);
            c->setSize(this->x, this->y, this->w, this->h);
            return c;
        }
				
        NonPalette<Color, Range, white> *
		createNonPaletteComponent ( void (*repaintHook) (NonPalette<Color, Range, white> *))
        {
            auto c = new NonPalette<Color, Range, white>(this->graphic, repaintHook);
            return c;
        }
				
        NonPalette<Color, Range, white> *
		createNonPaletteComponent (int x, int y, int w, int h, void (*repaintHook) (NonPalette<Color, Range, white> *))
        {
            auto c = new NonPalette<Color, Range, white>(this->graphic, repaintHook);
            c->setSize((Range)x, (Range)y, (Range)w, (Range)h);
            return c;
        }
        
        GKeypad<Color, Range, white> *
        getKeyPad (Box<Range> box)
        {
            if (this->keyPad != nullptr) {
                return this->keyPad;
            }
            this->keyPad = new GKeypad<Color, Range, white>(this->graphic, box);
            this->keyPad->setName("keypad");
            this->keyPad->setBackground(white);
            this->keyPad->setForeground(0x00);
            return this->keyPad;
        }
        
        void addPad (Color color, Range x, Range y, Range w, Range h)
        {
            auto pad = new GPad<Color, Range, white>(this->graphic, color, x, y, w, h);
            this->gPads.addFirst(pad);
        }
        
        
        void 
		addLabel (GLabel<Color, Range, white> *l)
        {
            /*if (list.contain(c) == false)*/
            elementsCount++;
            gLabels.addFirst(l);
        }
        void 
		addSlider (GSlide<Color, Range, white> *S)
        {
            /*if (list.contain(c) == false)*/
            elementsCount++;
            gSliders.addFirst(S);
        }
        void 
		addTextField (GTextField<Color, Range, white> *c)
        {
            /*if (list.contain(c) == false)*/
            elementsCount++;
            gTextFields.addFirst(c);
        }
        void 
		addProgressBar (GProgressBar<Color, Range, white> *c)
        {
            /*if (list.contain(c) == false)*/
            elementsCount++;
            gProgressBars.addFirst(c);
        }
        void 
		addBrowser (GBrowser<Color, Range, white> *c)
        {
            /*if (list.contain(c) == false)*/
            elementsCount++;
            gBrowsers.addFirst(c);
        }
        //
        void 
		addNonPalette (NonPalette<Color, Range, white> *I)
        {
            /*if (list.contain(c) == false)*/
            elementsCount++;
            gComponents.addFirst(I);
        }
        
        
        
        Color *
		getFrame ()
        {
            return frame;
        }
        
        virtual void setVisible (bool value) final
        {
            if (value == true) {
                this->wakeUp();
            }
            this->visible = value;
        } 
        
        void
        wakeUp ()
        {
            if (this->visible == false) {
                return;
            }
            wakeUp(&gTextFields);
            wakeUp(&gSliders);
            wakeUp(&gLabels);
            wakeUp(&gComponents);
            wakeUp(&gProgressBars);
            wakeUp(&gBrowsers);
        }
         
		void repaint (ViewPort<Range> &w)
        {
            if (this->visible == false) {
                return;
            }
            
            repaint(&gTextFields, w);
            repaint(&gSliders, w);
            repaint(&gLabels, w);
            repaint(&gComponents, w);
            repaint(&gProgressBars, w);
            repaint(&gBrowsers, w);
            repaint(&gPads, w);
            
            
            if (this->keyPad != nullptr) {
                if (test(w, this->keyPad->getBox()) == true) {
                    this->keyPad->repaint();
                }
            }
            if (this->dialog != nullptr) {
                if (this->dialog->isOpened() == true) {
                    this->dialog->repaint();
                }
            }
            if (this->alert != nullptr) {
                if (this->alert->isOpened() == true) {
                    this->alert->repaint();
                }
            }
            
			if (this->cursorIcon != nullptr) {
				this->getGraphic()->fill(this->cursorPos.x, this->cursorPos.y, this->cursorIcon);
			}
        }
				
        
        int32_t 
		fireSensor (TouchPointTypeDef arg, uint32_t cause)
        {
            if (this->silent == true) {
                return -1;
            }
            this->cursorPos.x = arg.point.x;
            this->cursorPos.y = arg.point.y;
            int32_t id = -1;
            if (this->alert != nullptr) {
                if (this->alert->isOpened() == true) {
                    if (this->alert->testPoint(arg.point) == false) {
                        this->alert->setGlow(150U);
                        return SilentComponent;
                    } else {
                        if (this->alert->fireSensorListeners(arg, cause) >= 0) {
                            this->wakeUp();
                        }
                        return ComponentAlertTypeId;
                    }
                }
            }
            if (this->dialog != nullptr) {
                if (this->dialog->isOpened() == true) {
                    if (this->dialog->testPoint(arg.point) == false) {
                        this->dialog->setGlow(150U);
                        return SilentComponent;
                    } else {
                        if (this->dialog->fireSensorListeners(arg, cause) >= 0) {
                            this->wakeUp(); 
                        }
                        return ComponentDialogTypeId;
                    } 
                }
            }
            if (selectedComponent != nullptr) {
                ComponentDsc dsc;
                dsc.id = (ComponentTypeId)selectedComponentType;
                dsc.component = selectedComponent;
                ACCESS_COMPONENT(id, dsc, fireSensorComponent, arg, cause);
                if (cause == SENSOR_RELEASE) {
                    int glow = 0;
                    ACCESS_VCOMPONENT(dsc, setGlow, glow);
                    selectedComponentType = SilentComponent;
                    selectedComponent = nullptr;  
                    return id;
                } else {
                    return -1;
                }
            } 
            if (this->keyPad != nullptr) {
                if (this->keyPad->fireSensorComponent(arg, cause) > 0) {
                    selectedComponentType = KeyPadTypeId;
                    selectedComponent = this->keyPad;
                    return KeyPadTypeId;
                }
            }
            if (this->packed == true) {
                ComponentDsc dsc;
                uint32_t size = this->packedArray->getSize();
                for (uint32_t i = 0; i < size; i++) {
                    dsc = this->packedArray->get(i);
                    ACCESS_COMPONENT(id, dsc, fireSensorComponent, arg, cause);
                    if((id > 0) && (cause != SENSOR_RELEASE)) {
                        selectedComponentType = dsc.id;
                        selectedComponent = dsc.component;
                        break;
                    } else {
                        selectedComponentType = SilentComponent;
                        selectedComponent = nullptr;
                        break;
                    }
                }
                return id;
            } else {
                selectedComponent = nullptr;
                id = fireSensorPane(&gProgressBars, arg, cause);
                if (id >= 0) {
                    selectedComponentType = ComponentProgressBarTypeId;
                    return id;
                }
                id = fireSensorPane(&gBrowsers, arg, cause);
                if (id >= 0) {
                    selectedComponentType = ComponentBrowserTypeId;
                    return id;
                }
                id = fireSensorPane(&gLabels, arg, cause);
                if (id >= 0) {
                    selectedComponentType = ComponentLabelTypeId;
                    return id;
                }
                id = fireSensorPane(&gSliders, arg, cause);
                if (id >= 0) {
                    selectedComponentType = ComponentSliderTypeId;
                    return id;
                }
                id = fireSensorPane(&gTextFields, arg, cause);
                if (id >= 0) {
                    selectedComponentType = ComponentTextFieldTypeId;
                    return id;
                }
                id = fireSensorPane(&gComponents, arg, cause);
                if (id >= 0) {
                    selectedComponentType = NonPaletteComponentTypeId;
                    return id;
                }
            }
            return -1;
        }
        
        int32_t 
		fireSensor (void *arg, uint32_t cause, uint32_t type = 0)
        {
            static TouchPointTypeDef __arg;
            __arg = *(TouchPointTypeDef *)arg;
            return this->fireSensor(__arg, cause);
        }

		ComponentDsc
		getComponentByName (const char *name)
		{
			ComponentDsc dsc = {SilentComponent, nullptr};
			char *component_name;
			if (this->packed == true) {
                this->packedArray->foreach([&](ComponentDsc &component_dsc) ->void{
                    GET_FROM_COMPONENT(component_dsc, getName, component_name);
                    if (strcmp(name, component_name) == 0) {
						dsc = component_dsc;
					}
                });
				return dsc;
			}
			dsc.component = this->checkComponentName(&gLabels, name);
			if (dsc.component != nullptr) {
				dsc.id = ComponentLabelTypeId;
				return dsc;
			}
			
			dsc.component = this->checkComponentName(&gSliders, name);
			if (dsc.component != nullptr) {
				dsc.id = ComponentSliderTypeId;
				return dsc;
			}
			
			dsc.component = this->checkComponentName(&gTextFields, name);
			if (dsc.component != nullptr) {
				dsc.id = ComponentTextFieldTypeId;
				return dsc;
			}
            
            dsc.component = this->checkComponentName(&gProgressBars, name);
			if (dsc.component != nullptr) {
				dsc.id = ComponentProgressBarTypeId;
				return dsc;
			}
            dsc.component = this->checkComponentName(&gBrowsers, name);
			if (dsc.component != nullptr) {
				dsc.id = ComponentBrowserTypeId;
				return dsc;
			}
			dsc.component = this->checkComponentName(&gComponents, name);
			if (dsc.component != nullptr) {
				dsc.id = NonPaletteComponentTypeId;
				return dsc;
			}
			return dsc;
			
		}
        
        ComponentDsc 
        accessElement (int element)
        {
            ComponentDsc dsc = {SilentComponent, nullptr};
            ComponentNum cNum = {element, nullptr};
            if (element == 0) {
                return dsc;
            }
            if (this->packed == true) {
                element--;
                if (this->packedArray->getSize() < element) {
                    return dsc;
                } else {
                    return this->packedArray->get(element);
                }
			}
            cNum = checkComponentNum(&gLabels, cNum.num);
            if (cNum.component != nullptr) {
                dsc.id = ComponentLabelTypeId;
                dsc.component = cNum.component;
                return dsc;
            }
            cNum = checkComponentNum(&gSliders, cNum.num);
            if (cNum.component != nullptr) {
                dsc.id = ComponentSliderTypeId;
                dsc.component = cNum.component;
                return dsc;
            }
            cNum = checkComponentNum(&gTextFields, cNum.num);
            if (cNum.component != nullptr) {
                dsc.id = ComponentTextFieldTypeId;
                dsc.component = cNum.component;
                return dsc;
            }
            cNum = checkComponentNum(&gProgressBars, cNum.num);
            if (cNum.component != nullptr) {
                dsc.id = ComponentProgressBarTypeId;
                dsc.component = cNum.component;
                return dsc;
            }
            cNum = checkComponentNum(&gBrowsers, cNum.num);
            if (cNum.component != nullptr) {
                dsc.id = ComponentBrowserTypeId;
                dsc.component = cNum.component;
                return dsc;
            }
            cNum = checkComponentNum(&gComponents, cNum.num);
            if (cNum.component != nullptr) {
                dsc.id = NonPaletteComponentTypeId;
                dsc.component = cNum.component;
                return dsc;
            }
            return dsc;
        }
        
        void setViewPortDisabled (bool disabled)
        {
            this->wievPortDisabled = disabled;
        }
        
        
            
        
        
        /*
            this methods are not provide access level check - 
            accessible component can be from another content pane
        */
        /*Access*/
        void
        setSelectComponent (ComponentDsc dsc, bool select)
        {  
            int res;
            ACCESS_COMPONENT(res, dsc, setSelect, select);
        }
        
        void
        setComponentText (ComponentDsc dsc, const char *text)
        {
            int res;
            ACCESS_COMPONENT(res, dsc, setText, (char *)text);
        }
        
		template <typename Value>
        void
        setComponentText (ComponentDsc dsc, Value value)
        {
            int res;
            ACCESS_COMPONENT(res, dsc, setText, value);
        }
        
        void
        apendComponentText (ComponentDsc dsc, const char *text)
        {
            int res;
            ACCESS_COMPONENT(res, dsc, apendText, (char *)text);
        }
        
		template <typename Value>
        void
        apendComponentText (ComponentDsc dsc, Value value)
        {
            int res;
            ACCESS_COMPONENT(res, dsc, apendText, value);
        }
        
        template <typename Listener>
        void
        setUserActionListener (ComponentDsc dsc, Listener listener)
        {
            int res;
            ACCESS_COMPONENT(res, dsc, addUserEvent, listener);
        }
        
        template <typename Event>
        void
        performUserActionEvent (ComponentDsc dsc, Event event)
        {
            int res;
            ACCESS_COMPONENT(res, dsc, performUserActionEvent, event);
        }
        
        void
        removeAllUserActionListeners (ComponentDsc dsc)
        {
            int res;
            ACCESS_COMPONENT(res, dsc, removeAllUserEventLiteners, false);
        }
        
        
        
        void
        setSelectComponent (const char *name, bool select)
        {  
            ACCESS_VCOMPONENT(getComponentByName(name) , setSelect, select);
        }
        
        void
        setComponentText (const char *name, char *text)
        {
            ACCESS_VCOMPONENT(getComponentByName(name) , setText, text);
        }
        
        template <typename Value>
        void
        setComponentText (const char *name, Value value)
        {
            ACCESS_VCOMPONENT(getComponentByName(name) , setText, value);
        }
		
        
        void
        appendComponentText (const char *name, char *text)
        {
            ACCESS_VCOMPONENT(getComponentByName(name) , appendText, (char *)text);
        }
		
		void
        appendComponentText (ComponentDsc dsc, char *text)
        {
            ACCESS_VCOMPONENT(dsc, appendText, (char *)text);
        }
		
		void 
		setComponentBackgroundColor (ComponentDsc dsc, Color color)
		{
			ACCESS_VCOMPONENT(dsc , setBackground, color);
		}
        
		void 
		setComponentBackgroundColor (const char *name, Color color)
		{
			ACCESS_VCOMPONENT(getComponentByName(name) , setBackground, color);
		}
		
		void 
		setComponentForegroundColor (ComponentDsc dsc, Color color)
		{
			ACCESS_VCOMPONENT(dsc , setForeground, color);
		}
        
		void 
		setComponentForegroundColor (const char *name, Color color)
		{ 
			ACCESS_VCOMPONENT(getComponentByName(name) , setForeground, color);
		}
		
        template <typename Listener>
        void
        setUserActionListener (const char *name, Listener listener)
        {
            int res;
            ACCESS_COMPONENT( res, getComponentByName(name) , addUserEvent, listener);
        }
        
        template <typename Event>
        void
        performUserActionEvent (const char *name, Event event)
        {
            int res;
            ACCESS_COMPONENT( res, getComponentByName(name) , performUserActionEvent, event);
        }
        
        void
        removeAllUserActionListeners (const char *name)
        {
            int res;
            ACCESS_COMPONENT( res, getComponentByName(name) , removeAllUserEventLiteners, false);
        }
        /*Access*/
        
        
        int getTotalElementsCount ()
        {
            return this->elementsCount;
        }
				
		template <typename Img>
		void setCursorIcon (Img *img)
		{
			this->cursorIcon = img;
		}
				
};





#endif 


/*End of file*/

