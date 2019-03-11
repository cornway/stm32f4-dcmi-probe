#ifndef WAVE_SAMPLE_H
#define WAVE_SAMPLE_H

#include "wave.h"
#include "iterable.h"

class WaveSample {
    private :
        WAVE_DSC waveDsc;
        void *buffer;
        char *name;
        virtual void ll_play (WAVE_DSC dsc, void *buffer);
        virtual void ll_wait ();
    public :
        WaveSample (char *name, WAVE_DSC dsc, void *buffer);
        void init (char *name, WAVE_DSC dsc, void *buffer);
        ~WaveSample ();
    
        void play ();
        void toUnsigned ();
    
        WAVE_DSC getDsc ();
        void *getBuffer ();
    
        void wait ();
};

#endif /*WAVE_SAMPLE_H*/

