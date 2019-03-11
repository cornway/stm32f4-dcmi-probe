#ifndef TEST_GAME_ENGINE
#define TEST_GAME_ENGINE

#include "ContentPane.h"
#include "NonPaletteComponentInterface.h"
#include "GameTestWalkingMan.h"
#include "backGround0.h"

extern tAnimation<uint16_t> mainHeroWalkSkin;
extern tAnimation<uint16_t> mainHeroThrowSkin;
extern tAnimation<uint16_t> throwingSkin;

extern const tImage<uint16_t> backGround0;

class  TestGameEngine {
    private :
        GContentPane<ColorDepth> *contentPane;
        WalkingManObject<ColorDepth> *mainHero;   
        int32_t tick;
    public :
        TestGameEngine (GContentPane<ColorDepth> *contentPane)
        {
            this->contentPane = contentPane;
            NonPaletteInterface<ColorDepth>  *c = contentPane->createComponent<NonPaletteInterface<ColorDepth> >();
            contentPane->addNonPalette( c );
            mainHero = (WalkingManObject<ColorDepth> *) new WalkingManObject<ColorDepth>( c, mainHeroWalkSkin, mainHeroThrowSkin ) ;
            tick = 0;
        }
        
        
        WalkingManObject<ColorDepth> *getMainHero()
        {
            return mainHero;
        }
        
        
        
        void repaint (ViewPort &w)
        {
            contentPane->getGraphic()->fillScaled(0, 0, &backGround0, 4, 4);
            contentPane->repaint(w);
            tick++;
            mainHero->tick();
        }
        
    
    
    
};



#endif