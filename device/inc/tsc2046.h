#ifndef TSC2046_H
#define TSC2046_H

#include "tsc2046_conf.h"
#include <stdint.h>
#include "abstract.h"
#include "touch.h"

#define TSC2046_CAL_OK              (0U)
#define TSC2046_CAL_FILE_NOT_FOUND  (1U)
#define TSC2046_CAL_SMALL_CORE      (-1)
#define TSC2046_CAL_WRITE_DENIED    (-2)

void tsc_wait_ns (int32_t c);

class tsc2046Drv {
    private :
        abstract::EventBurner eventBurner;
    
        bool invoke_flag;
        bool it_enabled;
        
        uint16_t read (uint8_t command);
    
        int16_t get_raw_x ();
        int16_t get_raw_y ();
        int16_t get_raw_z1 ();
        int16_t get_raw_z2 ();
        int16_t read_filter (uint8_t command, uint8_t maxSample);
    
        
        int saveCalData (char *path);
        
    
        uint8_t ll_rw (uint8_t data);
        void ll_sel (bool sel);
        bool ll_busy ();
        void ll_init (bool it_ena);
    
        
    public :
        tsc2046Drv ();
        void init (bool it_ena);
        int loadCalData (char *path);
        int32_t invoke (TouchPointTypeDef *point, uint32_t w, uint32_t h);
        bool isTouching ();
    
        void IT_Handle ();
        void tim_it_handle ();
        int calibration (char *path);
        int16_t getPositionX ();
        int16_t getPositionY ();
    
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
        
}; /*class tsc2046Drv*/

#endif /*TSC2046_H*/

