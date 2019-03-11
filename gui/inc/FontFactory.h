#ifndef FONT_FACTORY_CLASS
#define FONT_FACTORY_CLASS
#include "iterable.h"
#include "gui_Defs.h"



template <typename F, typename Set>
class FontFactory {
    private :
        vector::Vector<FontCharSet<tFont, DefaultCharStorage_t> > fontsCollection;
    public :
        FontFactory ()
        {
            
        }
        ~FontFactory ()
        {
            auto fs = fontsCollection.getFirst();
            auto fsn = fs->next();
            while (fs != nullptr) {
                fontsCollection.remove(fs);
                delete fs;
                fs = fsn;
                fsn = fsn->next();
            }
        }
        FontCharSet<F, Set> *collectFont (const F *f)
        {
            Set *set = (Set *) new Set[256];
            if (set == nullptr) {
                return nullptr;
            }
            FontCharSet<tFont, DefaultCharStorage_t> *fontSet = \
                        (FontCharSet<tFont, DefaultCharStorage_t> *) new \
                                    FontCharSet<tFont, DefaultCharStorage_t>(set, f);
            if (fontSet == nullptr) {
                delete(set);
                return nullptr;
            }
            makeSet(set, f);
            fontsCollection.addFirst(fontSet);
            return fontSet;
        }
        
        
        Set *makeSet (Set *set, const F *f)
        {
                int l = f->elements;
                int i  = 0;
                while (l--) {
                    i = (int) f->imagesSet[l].Code;
                    set[i] = l;
                }
                return set;
        }
};




#endif

/*End of file*/


