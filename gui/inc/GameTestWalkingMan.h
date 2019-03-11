#ifndef GameTestWalkingMan
#define GameTestWalkingMan

#include "NonPaletteComponentInterface.h"
#include "TGraphicComponent.h"
#include "ContentPane.h"
#include "ganimation.h"


int WalkingManRepaintHook (void *);
void WalkingManThrowingWeaponRepaintHook (void *);


int DefaultWalkingManClickListener (TouchStruct arg);
int DefaultWalkingManTouchListener (TouchStruct arg);
int DefaultWalkingManReleaseListener (TouchStruct arg);

int DefaultWalkAnimationHook (void *a, void *p);
int DefaultThrowAnimationHook (void *a, void *p);


enum WalkingManActionEnum {
    WalkingManWalk = 0, /*or iddle*/
    WalkingManThrow = 1,
    
};

#define WalkingManObjectId 0x0000A00D

template <typename Color>
    class WalkingManObject : public Dimension<int32_t>
{
    private :
        Graphic<Color> *graphic;  
        NonPaletteInterface<Color> *paletteInterface;     
        friend int WalkingManRepaintHook (void *);
        friend int DefaultWalkingManClickListener (TouchStruct);
        friend int DefaultWalkingManTouchListener (TouchStruct);
        friend int DefaultWalkingManReleaseListener (TouchStruct);
        friend int DefaultThrowAnimationHook (void *a, void *p);
        friend int DefaultWalkAnimationHook (void *a, void *p);
    
        TGraphicComponent<ColorDepth> throwingWeapon;//()
    
        GAnimation<Color> walkingSkin;
        GAnimation<Color> throwingSkin;
    
        int32_t  tickCount;
        int16_t  neededStepCount;
        int16_t  stepX;
        int16_t  stepY;
        int32_t  xTarget;
        int32_t  yTarget;
        int8_t   action;
        int8_t   direction;
    public :  
        
    WalkingManObject (NonPaletteInterface<Color> *interface, tAnimation<Color> walking, tAnimation<Color> throwing)
                        : 
                        Dimension<int32_t>(0, 0, 50, 50), 
                        throwingWeapon(this->graphic, -1)
        {
            this->graphic = interface->getGraphic();
            this->paletteInterface = interface;
            this->paletteInterface->setUserObject(this, WalkingManObjectId);
            this->paletteInterface->setRepaintHook(WalkingManRepaintHook);
            
            setWalkSkin(walking);
            setThrowSkin(throwing);

            tickCount = 0;
            xTarget = 0;
            xTarget = 0;
            stepX = 0;
            stepY = 0;
            action = 0;
            direction = 0;
            neededStepCount = 0;
            this->paletteInterface->setForceRepaint(true);
            this->paletteInterface->addClickListener(DefaultWalkingManClickListener);
            this->paletteInterface->addTouchListener(DefaultWalkingManTouchListener);
            this->paletteInterface->addReleaseListener(DefaultWalkingManReleaseListener);
            
            
        }
        
        int32_t tick ()
        {
            tickCount++;
            return tickCount;
        }
        
        void throwWeapon (int power, int direction)
        {
            if (throwingWeapon.isTimeElapse() == false) {
                return;
            }
            action = WalkingManThrow;
            throwingWeapon(this->graphic, 15);
            throwingWeapon.setSize(this->x + 10, this->y + 16, 50, 50);
            throwingWeapon.dx = 10 * (direction == 0 ? 1 : -1);
            throwingWeapon.setRepaintHook(WalkingManThrowingWeaponRepaintHook);
            ((GContentPane<ColorDepth> *)(paletteInterface->getContentPane()))->addTempGraphic(&throwingWeapon);
        }
        
        
        
        void setWalkSkin (tAnimation<Color> a)
        {
            walkingSkin(a);
            walkingSkin.setReachEndHook(DefaultWalkAnimationHook, this);
        }
        
        void setThrowSkin (tAnimation<Color> a)
        {
            throwingSkin(a);
            throwingSkin.setReachEndHook(DefaultThrowAnimationHook, this);
        }
        
        
                   
};






#endif

/*End of file*/


