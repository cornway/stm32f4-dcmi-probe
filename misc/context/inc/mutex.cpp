#include "mutex.h"



OWNER::OWNER ()
{
    this->busy = false;
}

OWNER::~OWNER ()
{
    
}

OWNER::OWNER (THREAD *owner)
{
    this->owner = owner;
}

OWNER::OWNER (OWNER &owner)
{
    this->owner = owner.owner;
}

void OWNER::init (THREAD *t)
{
    this->owner = t;
}

void OWNER::operator = (OWNER &owner)
{
    this->owner = owner.owner;
}

void OWNER::operator = (OWNER *owner)
{
    this->owner = owner->owner;
}

THREAD *OWNER::getOwner ()
{
    return this->owner;
}


MUTEX::MUTEX ()
{
    this->busy = false;
}


MUTEX::~MUTEX ()
{
    
}


void MUTEX::init (WORD_T id)
{
    this->Id = id;
}




WORD_T MUTEX::lock (OWNER *owner)
{
    this->owners.addLast(owner);
    return MUTEX_GRANT_WAIT;
}

WORD_T MUTEX::lock (THREAD *t)
{
    this->owner = t;
    return MUTEX_GRANT_LOCK;
}


OWNER *MUTEX::unlock (THREAD *t)
{
    if (this->owner != t) {
        return nullptr;
    } 
    OWNER *o = this->owners.removeFirst();
    if (o != nullptr) {
        this->owner = o->getOwner();
    }
    return o;
}

bool MUTEX::hasLock ()
{
    return !this->owners.isEmpty();
}




MUTEX_FACTORY::MUTEX_FACTORY ()
{
    this->freeMutexCount = MAX_MUTEX_COUNT;
    this->freeOwnersCount = MAX_OWNERS_COUNT;
}

MUTEX_FACTORY::~MUTEX_FACTORY ()
{
    
}

void MUTEX_FACTORY::init ()
{
    
}

WORD_T MUTEX_FACTORY::lock (THREAD *t, WORD_T id)
{
    OWNER *o = nullptr;
    MUTEX *m = this->mutexList.getFirst();
    while (m != nullptr) {
        if (m->Id == id) {
            o = this->allocOwner();
            if (o == nullptr) {
                return MUTEX_SMALL_CORE;
            }
            o->init(t);
            return m->lock(o);
        }
        m = m->next();
    }
    m = this->allocMutex ();
    if (m == nullptr) {
        return MUTEX_SMALL_CORE;
    }
    m->init(id);
    this->mutexList.addLast(m);
    return m->lock(t);
}



THREAD *MUTEX_FACTORY::unlock (THREAD *t, WORD_T id)
{
    OWNER *o;
    MUTEX *m = this->mutexList.getFirst();
    while (m != nullptr) {
        if (m->Id == id) {
            o = m->unlock(t);
            if (o != nullptr) {
                t = o->getOwner();
                this->freeOwner(o);
                return t;
            } else {
                this->mutexList.remove(m);
                this->freeMutex(m);
                return nullptr;
            }
        }
        m = m->next();
    }
    return nullptr;
}

WORD_T MUTEX_FACTORY::monitor_start (WORD_T mon_id, WORD_T *mutexList)
{
    return MUTEX_GRANT_UNLOCK;
}

WORD_T MUTEX_FACTORY::monitor_stop (WORD_T mon_id, WORD_T *mutexList)
{
    return MUTEX_GRANT_UNLOCK;
}

OWNER *MUTEX_FACTORY::allocOwner ()
{
    if (this->freeOwnersCount == 0) {
        return nullptr;
    }
    this->freeOwnersCount--;
    for (WORD_T i = 0; i < MAX_OWNERS_COUNT; i++) {
        if (this->ownerMemory[i].busy == false) {
            this->ownerMemory[i].busy = true;
            return &this->ownerMemory[i];
        }
    }
    return nullptr;
}

void MUTEX_FACTORY::freeOwner (OWNER *o)
{
    this->freeOwnersCount++;
    o->busy = false;
    o->owner = nullptr;
}

MUTEX *MUTEX_FACTORY::allocMutex ()
{
    if (this->freeMutexCount == 0) {
        return nullptr;
    }
    this->freeMutexCount--;
    for (WORD_T i = 0; i < MAX_MUTEX_COUNT; i++) {
        if (this->mutexMemory[i].busy == false) {
            this->mutexMemory[i].busy = true;
            return &this->mutexMemory[i];
        }
    }
    return nullptr;
}

void MUTEX_FACTORY::freeMutex (MUTEX *m)
{
    this->freeMutexCount++;
    m->busy = false;
    m->owner = nullptr;
    m->Id = 0;
    m->owners.removeAll();
}

WORD_T MUTEX_FACTORY::getFreeOwners ()
{
    return this->freeOwnersCount;
}

WORD_T MUTEX_FACTORY::getFreeMutexs ()
{
    return this->freeMutexCount;
}


_EXTERN void *vmalloc (UINT_T size);
_EXTERN void vmfree (void *p);

