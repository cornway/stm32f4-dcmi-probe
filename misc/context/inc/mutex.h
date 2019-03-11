#ifndef MUTEX_H
#define MUTEX_H

#include "vm_conf.h"
#include "machM4.h"
#include "thread.h"
#include "iterable.h"

#ifndef MAX_OWNERS_COUNT
#define MAX_OWNERS_COUNT    (8U)
#endif

#ifndef MAX_MUTEX_COUNT
#define MAX_MUTEX_COUNT     (8U)
#endif


enum {
    MUTEX_GRANT_LOCK,
    MUTEX_GRANT_WAIT,
    MUTEX_GRANT_UNLOCK,
    MUTEX_SMALL_CORE,
};



class MUTEX_FACTORY;


class MUTEX;

class OWNER : public Link<OWNER> {
    private :
        THREAD *owner;
        bool busy;
        
        friend class MUTEX_FACTORY;
    public :
        OWNER ();
        OWNER (THREAD *owner);
        OWNER (OWNER &owner);
    
        void init (THREAD *owner);
        
        THREAD *getOwner (); 
        void operator = (OWNER &owner);
        void operator = (OWNER *owner);
    
        ~OWNER ();
};

class MUTEX : public Link<MUTEX> {
    private :
        vector::Vector<OWNER> owners;
        THREAD *owner;
        WORD_T Id;
        bool busy;
    
        friend class MUTEX_FACTORY;
    public :
        MUTEX ();
        ~MUTEX ();
    
        void init (WORD_T id);
        WORD_T lock (THREAD *t);
        WORD_T lock (OWNER *owner);
        OWNER *unlock (THREAD *t);
    
        WORD_T getId ();
        bool hasLock ();
};


class MUTEX_FACTORY {
    private :
        MUTEX mutexMemory[MAX_MUTEX_COUNT];
        OWNER ownerMemory[MAX_OWNERS_COUNT];
        
        uint16_t freeOwnersCount;
        uint16_t freeMutexCount;
    
        vector::Vector<MUTEX> mutexList;
    
        OWNER *allocOwner();
        void freeOwner (OWNER *owner);
    
        MUTEX *allocMutex ();
        void freeMutex (MUTEX *mutex);
    public :
        MUTEX_FACTORY ();
        ~MUTEX_FACTORY();
    
        void init ();
        WORD_T lock (THREAD *t, WORD_T id);
        THREAD *unlock (THREAD *t, WORD_T id);
        WORD_T monitor_start (WORD_T mon_id, WORD_T *mutexList);
        WORD_T monitor_stop (WORD_T mon_id, WORD_T *mutexList);
    
        WORD_T getFreeOwners ();
        WORD_T getFreeMutexs ();
        
};



#define     MON_ID  (0xFFFFFFFE)

/*
class MONITOR_FACTORY;
class MONITOR;

class KEY : public Link<KEY> {
    private :
        MUTEX *mutex;
        WORD_T id;
        KEY ();
        ~KEY();
    public :
        
};

class MONITOR : public Link<MONITOR> {
    private :
        vector::Vector<KEY> keys;
        WORD_T id;
        MONITOR ();
        ~MONITOR ();
    public :
        
        
};


MONITOR_FACTORY {
    
};
*/
#endif /*MUTEX_H*/


