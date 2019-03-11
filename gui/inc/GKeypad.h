#ifndef GCEYPAD_OBJECT
#define GCEYPAD_OBJECT
#include "graphic.h"
#include "printer.h"
#include "gComponent.h"
#include "string.h"

typedef struct {
    const char *name;
    int8_t func;
} keyPair;

enum {
    KEY_FUNC_CHAIR,
    KEY_FUNC_DELETE,
    KEY_FUNC_BCKSPC,
    KEY_FUNC_ENT,
    KEY_FUNC_TAB,
    KEY_FUNC_SPACE,
};
    
static const keyPair defaultKeyArray[] = 
{
    {"1", KEY_FUNC_CHAIR},
    {"2", KEY_FUNC_CHAIR},
    {"3", KEY_FUNC_CHAIR},
    {"4", KEY_FUNC_CHAIR},
    {"5", KEY_FUNC_CHAIR},
    {"6", KEY_FUNC_CHAIR},
    {"7", KEY_FUNC_CHAIR},
    {"8", KEY_FUNC_CHAIR},
    {"9", KEY_FUNC_CHAIR},
    {"0", KEY_FUNC_CHAIR},
    {"Q", KEY_FUNC_CHAIR},
    {"W", KEY_FUNC_CHAIR},
    {"E", KEY_FUNC_CHAIR},
    {"R", KEY_FUNC_CHAIR},
    {"T", KEY_FUNC_CHAIR},
    {"Y", KEY_FUNC_CHAIR},
    {"U", KEY_FUNC_CHAIR},
    {"I", KEY_FUNC_CHAIR},
    {"O", KEY_FUNC_CHAIR},
    {"P", KEY_FUNC_CHAIR},
    {"A", KEY_FUNC_CHAIR},
    {"S", KEY_FUNC_CHAIR},
    {"D", KEY_FUNC_CHAIR},
    {"F", KEY_FUNC_CHAIR},
    {"G", KEY_FUNC_CHAIR},
    {"H", KEY_FUNC_CHAIR},
    {"J", KEY_FUNC_CHAIR},
    {"K", KEY_FUNC_CHAIR},
    {"L", KEY_FUNC_CHAIR},
    {";", KEY_FUNC_CHAIR},
    {"'", KEY_FUNC_CHAIR},
    {"Z", KEY_FUNC_CHAIR},
    {"X", KEY_FUNC_CHAIR},
    {"C", KEY_FUNC_CHAIR},
    {"V", KEY_FUNC_CHAIR},
    {"B", KEY_FUNC_CHAIR},
    {"N", KEY_FUNC_CHAIR},
    {"M", KEY_FUNC_CHAIR},
    {".", KEY_FUNC_CHAIR},
    {"/", KEY_FUNC_CHAIR},
    {"\"", KEY_FUNC_CHAIR},
    {"\\", KEY_FUNC_CHAIR},
    {"!", KEY_FUNC_CHAIR},
    {"@", KEY_FUNC_CHAIR},
    {"#", KEY_FUNC_CHAIR},
    {"$", KEY_FUNC_CHAIR},
    {"%", KEY_FUNC_CHAIR},
    {"^", KEY_FUNC_CHAIR},
    {"&", KEY_FUNC_CHAIR},
    {"*", KEY_FUNC_CHAIR},
    {"(", KEY_FUNC_CHAIR},
    {")", KEY_FUNC_CHAIR},
    {"-", KEY_FUNC_CHAIR},
    {"+", KEY_FUNC_CHAIR},
    {"=", KEY_FUNC_CHAIR},
    {"|", KEY_FUNC_CHAIR},
    {"DEL", KEY_FUNC_DELETE},
    {"ENT", KEY_FUNC_ENT},
    {"BKSP", KEY_FUNC_BCKSPC},
    {"SPACE", KEY_FUNC_SPACE},
    {"", -1},
};

template <typename Color, typename Range, Color white>
    class GKey : public Dimension<Range>,
                 public Link<GKey<Color, Range, white> > {
        private :
            char name[6];
            int8_t func;
        public :
            GKey (keyPair key, Dimension<Range> d) : Dimension<Range> (d),
                                Link<GKey<Color, Range, white> > ()
            {
                int i = 0;
                while ((key.name[i] != '\0') && (i < 5)) {
                    this->name[i] = key.name[i];
                    i++;
                }
                this->name[i] = '\0';
                this->func = key.func;
            }
            ~GKey ()
            {
                
            }
            
            char *getName ()
            {
                return this->name;
            }
            
            int8_t getFunc ()
            {
                return this->func;
            }
    }; /*class GKey*/
 


template <typename Color, typename Range, Color white>
class GKeypad : public GComponent<Color, Range, white>  {
    private :
        vector::Vector<GKey<Color, Range, white> > keyArray;
        GKey<Color, Range, white> *selectedKey;
        int selectedKeyFunc;
        int8_t keySelectTime;
        Color selectColor;
        void destroy ()
        {
            GKey<Color, Range, white> *key = keyArray.getFirst();
            GKey<Color, Range, white> *keyn = key->next();
            while (key != nullptr) {
                keyArray.remove(key);
                delete key;
                key = keyn;
                keyn = keyn->next();
            }
        }
    public:
        GKeypad (Graphic<Color, Range, white> *graphic, Box<Range> box) : GComponent<Color, Range, white> (graphic)
        {
            
            this->glow = 0;
            this->setSize(box);
            this->selectedKeyFunc = 0;
            this->selectedKey = nullptr;
            this->keySelectTime = 0;
            this->setKeys((keyPair *)defaultKeyArray, box); 
            this->selectColor = 0;
        }
            
        ~GKeypad ()
        {
            
            this->destroy();
        }    
        
        void setKeys (keyPair *array, Box<Range> box)
        {
            this->destroy();
            GKey<Color, Range, white> *gkey;
            Range fw = this->graphic->getFontWidth();
            Range fh = this->graphic->getFontHeight();
            Dimension<Range> keyc(box.x, box.y, fw, fh);
            Box<Range> keyb;
            int keyIndex = 0;
            while (array[keyIndex].func != -1) {
                keyb = this->getTextBox((char *)array[keyIndex].name, fw, fh);
                keyc.w = keyb.w;
                keyc.h = keyb.h;
                gkey = new GKey<Color, Range, white>(array[keyIndex], keyc);
                if (gkey == nullptr) {
                    break;
                }
                keyArray.addFirst(gkey);
                keyc.moveRight();
                if ((keyc.x + keyc.w) > (box.x + box.w)) {
                    keyc.x = box.x;
                    keyc.y += fh;
                    if ((keyc.y + keyc.h) > (box.y + box.h)) {
                        break;
                    }
                }
                keyIndex++;
            }
        }
        
        virtual
        int fireSensorListeners (TouchPointTypeDef arg, uint32_t cause) final
        {
            if (this->silent == true) {
                return -1;
            }
            if (this->testPoint(arg.point) == false) {
                return -1;
            }
            static TouchPointTypeDef arg__;
            arg__ = arg;
            if (cause == SENSOR_RELEASE) { 
                GKey<Color, Range, white> *key = keyArray.getFirst();
                while (key != nullptr) {
                    if (key->testPoint(arg.point) == true) {
                        this->selectedKey = key;
                        this->keySelectTime = 50;
                        arg.point = this->normalize(arg.point);
                        #if (EVENT_SIMPLE_SCHEME == 1U)
                            if (this->eventListener != nullptr)
                                (*this->eventListener) (abstract::Event(&arg__, cause));
                        #else
                            userActionEventBurner.fireEvents(abstract::Event(&arg__, cause));
                        #endif
                        return 1;
                    }
                    key = key->next();
                }
            } 
            this->selectedKey = nullptr;
            this->keySelectTime = 0;
            return 1;
        }

        virtual void repaint () final
        {
            GKey<Color, Range, white> *key = keyArray.getFirst();
            this->graphic->fill(this->getBox(), this->background);
            Box<Range> tbox;
            while (key != nullptr) {
                if ((key == this->selectedKey) && (this->keySelectTime > 0)) {
                    this->graphic->fill(key->getBox(), this->selectColor);
                    this->keySelectTime--;
                }
                tbox = key->getBox();
                tbox.x = tbox.x + 10;
                this->graphic->drawString(tbox, key->getName(), this->foreground);
                key = key->next();
            }
        }
        
        void setSelectColor (Color color)
        {
            this->selectColor = color;
        }
        
        GKey <Color, Range, white> *
        getSelectedKey ()
        {
            return this->selectedKey;
        }
        
};        

#endif

