#ifndef TOUCH_H
#define TOUCH_H

#include "stdint.h"
#include "abstract.h"
#include "dimensions.h"

enum {
    SENSOR_ANY          = -1,
    SENSOR_CLICK        = 1,
    SENSOR_TOUCH        = 2,
    SENSOR_RELEASE      = 3,
    SENSOR_HOLD         = 4,
    SENSOR_SLIDE_LEFT   = 5,
    SENSOR_SLIDE_RIGHT  = 6,
    SENSOR_SLIDE_UP     = 7,
    SENSOR_SLIDE_DOWN   = 8,
} ;

#pragma anon_unions

typedef struct {
    union {
      struct {
        int16_t x; 
        int16_t y;
      }; 
      struct {
          Point<int16_t> point;
      };
    };
    
    uint16_t hits;
    uint16_t onPressTime;
} TouchPointTypeDef;

class TouchSensor {
    private :
        int32_t event;
        TouchPointTypeDef point;
        TouchPointTypeDef tpoint;
        bool hasInvoke;
        abstract::EventBurner eventBurner;
    public :
        TouchSensor ();
        ~TouchSensor ();
        
        void setEvent (TouchPointTypeDef point, uint32_t Event);
        void invokeEvent ();
        void clearEvent ();
    
        template <typename L>
        void
        addListener (L l)
        {
            abstract::EventListener *li = new abstract::EventListener(l);
            if (li == nullptr) {
                return;
            }
            this->eventBurner.addListener( li );
        }
        
        void removeAllListeners ()
        {
            this->eventBurner.removeAll();
        }

        
}; /*class TouchSensor*/

#endif /*TOUCH_H*/

