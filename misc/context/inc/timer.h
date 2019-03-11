#ifndef VM_TIMER_H
#define VM_TIMER_H

#include "machM4.h"
#include "vm_conf.h"
#include "iterable.h"

#define TIMER_SECONDS_MASK  (0x80000000U) /*need to identify what timer is create - milisecond or second accuracy*/

class TIMER_FACTORY;

class TIMER_HANDLE : public Link<TIMER_HANDLE> {
    private :
        WORD_T *dest;
        WORD_T count;
        WORD_T id;
    
        TIMER_HANDLE ();
        ~TIMER_HANDLE ();
        friend class TIMER_FACTORY;
    public :
        
};

class TIMER_FACTORY {
    private :
        vector::Vector<TIMER_HANDLE> timers_ms, timers_s;
        WORD_T mills;
    
        void tick_s ();
    public :
        TIMER_FACTORY ();
        ~TIMER_FACTORY ();
    
        void tick_ms ();    
        INT_T create (WORD_T *dest, WORD_T id);
        INT_T remove (WORD_T id);
};

#endif /*VM_TIMER_H*/
