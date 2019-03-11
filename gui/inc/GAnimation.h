#ifndef GANIMATION_OBJECT
#define GANIMATION_OBJECT
#include "graphic.h"

template <typename Color>
class GAnimation {
private :
    tAnimation<Color> animation;
    int frameCursor;
    int (*reachesEnd) (void *, void *);
    void *parent;
public :   
    GAnimation () {}
    GAnimation (tAnimation<Color> animation)
    {
        frameCursor = 0;
        this->animation = animation;
        reachesEnd = nullptr;
    }
    void operator () (tAnimation<Color> animation)
    {
        frameCursor = 0;
        this->animation = animation;
        reachesEnd = nullptr;
    }
    ~GAnimation()
    {
        
    }
    
    
    const tImage<Color> *getDiscont ()
    {
        frameCursor++;
        if (frameCursor >= animation.imagesCount) {
            frameCursor = 0;
            if (reachesEnd != nullptr) {
                (*reachesEnd) (this, parent);
            }   
        }
        return animation.imagesSet[frameCursor];
    }
    
    const tImage<Color> *getDiscont (int cursor)
    {
        if (cursor >= animation.imagesCount) {
            cursor = 0;
        }
        return animation.imagesSet[cursor];
    }
    
    void setReachEndHook (int (*hook) (void *, void *), void *parent)
    {
        reachesEnd = hook;
        this->parent = parent;
    }
    
    
    
};





#endif

/*End of file*/