#include "Thread.h"
#include <string.h>

_STATIC THREAD_LIST thread_list[VM_THREAD_MAX_PRIORITY];
_STATIC THREAD_LIST thread_drop_list;
_STATIC THREAD_LIST thread_delay_list;
_STATIC THREAD_LIST thread_fault_list;
_STATIC THREAD_LIST thread_notify_list;
_STATIC THREAD_LIST thread_cond_list;
_STATIC THREAD_LIST thread_event_wait_list;
_STATIC THREAD_LIST thread_mail_wait_list;
_STATIC THREAD *thread_table[THREAD_MAX_COUNT];
_STATIC WORD_T thread_count;

_STATIC void thread_register (THREAD *thread_link); 
_STATIC void thread_unregister (THREAD *thread_link); 
_STATIC void thread_link (THREAD_LIST *list, THREAD *thread_link);     /*link first to the list*/
_STATIC void thread_unlink (THREAD_LIST *list, THREAD *thread_link);   /*unlink from list*/
_STATIC void thread_move_list (THREAD_LIST *dest, THREAD_LIST *src);
_STATIC int test_thread (THREAD *t);
_STATIC UINT_T threads_total;

_EXTERN void *vmalloc (UINT_T size);
_EXTERN void vmfree (void *p);   

_STATIC void thread_register (THREAD *t)
{
    for (INT_T i = 0; i < THREAD_MAX_COUNT; i++) {
        if ((WORD_T)thread_table[i] == (WORD_T)0) {
            thread_table[i] = t;
            thread_count++;
            break;
        }
    }
}

_STATIC void thread_unregister (THREAD *t)
{
    for (INT_T i = 0; i < THREAD_MAX_COUNT; i++) {
        if ((WORD_T)thread_table[i] == (WORD_T)t) {
            thread_table[i] = (THREAD *)NULL;
            thread_count--;
            break;
        }
    }
}

void t_init_core ()
{
    thread_count = 0;
    for (INT_T i = 0; i < THREAD_MAX_COUNT; i++) {
        thread_table[i] = (THREAD *)NULL;
    }
    
    thread_drop_list.firstLink = (THREAD *)NULL;
    thread_drop_list.lastLink = (THREAD *)NULL;
    thread_drop_list.elements = 0;
    
    thread_delay_list.firstLink = (THREAD *)NULL;
    thread_delay_list.lastLink = (THREAD *)NULL;
    thread_delay_list.elements = 0;
    
    thread_notify_list.firstLink = (THREAD *)NULL;
    thread_notify_list.lastLink = (THREAD *)NULL;
    thread_notify_list.elements = 0;
    
    thread_cond_list.firstLink = (THREAD *)NULL;
    thread_cond_list.lastLink = (THREAD *)NULL;
    thread_cond_list.elements = 0;
    
    thread_event_wait_list.firstLink = (THREAD *)NULL;
    thread_event_wait_list.lastLink = (THREAD *)NULL;
    thread_event_wait_list.elements = 0;
    
    thread_mail_wait_list.firstLink = (THREAD *)NULL;
    thread_mail_wait_list.lastLink = (THREAD *)NULL;
    thread_mail_wait_list.elements = 0;
    
    for (WORD_T i = 0; i < VM_THREAD_MAX_PRIORITY; i++) {
        thread_list[i].firstLink = (THREAD *)NULL;
        thread_list[i].lastLink = (THREAD *)NULL;
        thread_list[i].elements = 0;
    }
}

int t_init (
                THREAD **t, 
                _CALLBACK callback, 
                BYTE_T priority, 
                WORD_T id, 
                WORD_T heap_size, 
                BYTE_T privilege, 
                const char *name,
                WORD_T arg_size,
                void *arg
           )
{
    if (callback == (_CALLBACK)NULL) {
        return T_NULL_CALLBACK;
    }
    if (heap_size < VM_MIN_THREAD_HEAP_SIZE) {
        return T_SMALL_HEAP;
    }
    if (
        (privilege != CPU_ACCESS_LEVEL_0) &&
        (privilege != CPU_ACCESS_LEVEL_1) &&
        (privilege != CPU_ACCESS_LEVEL_2) &&
        (privilege != CPU_ACCESS_LEVEL_3) 
    ) {
        return T_PRIV_UNDEF;
    }
    if (priority >= VM_THREAD_MAX_PRIORITY) {
        return T_PRIOR_UNDEF;
    }
    WORD_T l = strlen(name);
    if (l >= VM_DEF_THREAD_NAME_LEN) {
        return T_LONG_NAME;
    }
    *t = (THREAD *)vmalloc(heap_size + sizeof(THREAD));
    if (*t == (THREAD *)NULL) {
        return T_SMALL_CORE;
    }
    
    /*
    if (priority <= 2) {
         privilege |= CPU_PRIV_ACCESS;
    } else {
         privilege &= ~CPU_PRIV_ACCESS;
    }
    */
    
    thread_register(*t);
    
    memcpy((*t)->name, name, l);
    
    (*t)->ID = id;
    (*t)->STATUS = THREAD_STOP;
    (*t)->DELAY = 0;
    (*t)->CPU_FRAME = (CPU_STACK_FRAME *)NULL;
    (*t)->USE_FPU = THREAD_NO_FPU;
    (*t)->STACK_SIZE = heap_size;
    (*t)->PRIORITY = priority;
    (*t)->V_PRIORITY = priority;
    (*t)->PRIVILEGE = privilege;
    
    (*t)->chain.elements = 0;
    (*t)->chain.firstLink = NULL;
    (*t)->chain.lastLink = NULL;
    
    (*t)->link = NULL;
    (*t)->mutex = 0;
    (*t)->monitor = 0;
    
    (*t)->fault = 0;
    (*t)->faultMessage = "OK";
    
    (*t)->waitNotify = 0;
    
    (*t)->caller = NULL;
    
    (*t)->STACK = ((WORD_T)(*t) + sizeof(THREAD) + heap_size - STACK_ALLIGN) & (~(STACK_ALLIGN - 1));
    
    (*t)->CPU_FRAME = (CPU_STACK_FRAME *)((*t)->STACK - sizeof(CPU_STACK));
    (*t)->CPU_FRAME->callControl.EXC_RET = (WORD_T)callback;
    (*t)->CPU_FRAME->callControl.PC = (WORD_T)callback;
    (*t)->CPU_FRAME->callControl.PSR = CPU_XPSR_T_BM;
    (*t)->CPU_FRAME->cpuStack.R0 = arg_size;
    (*t)->CPU_FRAME->cpuStack.R1 = (WORD_T)arg;
    (*t)->CPU_FRAME->cpuStack.LR = (WORD_T)VMBREAK;
    
    return T_OK;
}

void t_destroy (THREAD *t)
{
    thread_unregister(t);
    vmfree(t);
}


THREAD *t_ready (UINT_T priority)
{
    THREAD *t;
    t = (THREAD *)thread_list[priority].firstLink;
    if (t == (THREAD *)NULL) {
        return (THREAD *)NULL;
    }
    while (t != (THREAD *)NULL) {
        if (test_thread(t) == 0) {
            break;
        }
        t = (THREAD *)t->nextLink;
    }
    return t;
}

void t_link_chain (THREAD *t, THREAD *l)
{
    thread_link(&t->chain, l);
}

void t_unlink_chain (THREAD *t, THREAD *l)
{
    thread_unlink(&t->chain, l);
}

void t_link_ready (THREAD *t)
{
    threads_total++;
    thread_link(&thread_list[t->V_PRIORITY], t);
}

void t_unlink_ready (THREAD *t)
{
    threads_total--;
    thread_unlink(&thread_list[t->V_PRIORITY], t);
}

void t_link_drop (THREAD *t)
{
    thread_link(&thread_drop_list, t);
}

void t_link_delay (THREAD *t)
{
    thread_link(&thread_drop_list, t);
}

void t_link_fault (THREAD *t)
{
    thread_link(&thread_fault_list, t);
}

void t_link_notify (THREAD *t)
{
    thread_link(&thread_notify_list, t);
}

void t_unlink_notify (THREAD *t)
{
    thread_unlink(&thread_notify_list, t);
}

THREAD *t_notify (const char *name)
{
    if (thread_notify_list.elements <= 0) {
        
    }
    THREAD *t = (THREAD *)thread_notify_list.firstLink;
    while (t != (THREAD *)NULL) {
        if (strcmp(name, t->name) == 0) {
            return t;
        }
        t = (THREAD *)t->nextLink;
    }
    return (THREAD *)NULL;
}

void t_link_cond (THREAD *t)
{
    thread_link(&thread_cond_list, t);
}

WORD_T t_check_cond (void (*linker) (THREAD *))
{
    if (thread_cond_list.elements <= 0) {
        return T_OK;
    }
    THREAD *t = (THREAD *)thread_cond_list.firstLink;
    THREAD *tn = (THREAD *)t->nextLink;
    while (t != (THREAD *)NULL) {
        if (t->link == NULL) {
            return T_NULL_CALLBACK;
        } else {
            if ((t->cond)(0, NULL) == 0) {
                thread_unlink(&thread_cond_list, t);
                (linker)(t);
            }
        }
        t = tn;
        tn = (THREAD *)tn->nextLink;
    }
    return T_OK;
}

void t_link_wait_event (THREAD *t)
{
    thread_link(&thread_event_wait_list, t);
}

WORD_T t_fire_event (void (*linker) (THREAD *) ,const char *event_name)
{
    THREAD *t = (THREAD *)thread_event_wait_list.firstLink;
    THREAD *tn = (THREAD *)t->nextLink;
    while (t != (THREAD *)NULL) {
        if (strcmp(t->event, event_name) == 0) {
            thread_unlink(&thread_event_wait_list, t);
            (linker)(t);
        }
        t = tn;
        tn = (THREAD *)tn->nextLink;
    }
    return T_OK;
}

int t_check_list ()
{
    return threads_total;
}

void t_refresh (void (*linker) (THREAD *))
{
    THREAD *t, *tn;
    t = (THREAD *)thread_drop_list.firstLink;
    tn = (THREAD *)t->nextLink;
    while (t != (THREAD *)NULL) {
        t->V_PRIORITY = t->PRIORITY;
        thread_unlink(&thread_drop_list, t);
        linker(t);
        t = tn;
        tn = (THREAD *)tn->nextLink;
    }
}

void t_tick (void (*linker) (THREAD *))
{
    THREAD *t = (THREAD *)thread_delay_list.firstLink;
    while (t != (THREAD *)NULL) {
        if (t->DELAY) {
            t->DELAY--;
        } else {
            thread_unlink(&thread_delay_list, t);
            linker(t);
        }
    }
}

THREAD *t_search (const char *name)
{
    for (WORD_T i = 0; i < THREAD_MAX_COUNT; i++) {
        if (strcmp(name, thread_table[i]->name) == 0) {
            return thread_table[i];
        }
    }
    return (THREAD *)NULL;
}

void t_unchain (void (*linker) (THREAD *), THREAD *t)
{
    if (t->chain.elements <= 0) {
        return;
    }
    THREAD * t_i = (THREAD *)t->chain.firstLink;
    THREAD *t_next_i = (THREAD *)t_i->nextLink;
    while (t_i != (THREAD *)NULL) {
        thread_unlink(&t->chain, t_i);
        linker(t_i);
        t_i = t_next_i;
        t_next_i = (THREAD *)t_next_i->nextLink;
    }
}

void t_link_mail (THREAD *t)
{
    thread_link(&thread_mail_wait_list, t);
}

void t_unlink_mail(THREAD *t)
{
    thread_unlink(&thread_mail_wait_list, t);
}

THREAD *t_mail (const char *name)
{
    if (thread_mail_wait_list.elements <= 0) {
        
    }
    THREAD *t = (THREAD *)thread_mail_wait_list.firstLink;
    while (t != (THREAD *)NULL) {
        if (strcmp(name, t->name) == 0) {
            return t;
        }
        t = (THREAD *)t->nextLink;
    }
    return (THREAD *)NULL;
}















_STATIC int test_thread (THREAD *t)
{
    return (t->fault || t->mutex || t->waitNotify);
}



_STATIC void thread_link (THREAD_LIST *list, THREAD *t)
{
    list->elements++;
    THREAD *i;
    if (list->lastLink == (THREAD *)NULL) {
        list->lastLink = t;
        list->firstLink = t;
        t->nextLink = NULL;
        t->prevLink = NULL;
        return;
    }
    i = (THREAD *)list->lastLink;
    t->prevLink = i;
    t->nextLink = NULL;
    i->nextLink = t;
    list->lastLink = t;			
    return;
}

_STATIC void thread_unlink (THREAD_LIST *list, THREAD *t)
{
    if (list->elements == 0){
        return;
    }
    list->elements--;
    THREAD *l = (THREAD *)t->prevLink, *r = (THREAD *)t->nextLink;
    if (!l && !r) {
        list->firstLink = (THREAD *)NULL;
        list->lastLink = (THREAD *)NULL;
        return;
    }
    if (!l) {
        list->firstLink = r;
        r->prevLink = NULL;
    }   else    {
        if (r)
        l->nextLink = r;
    }
    if (!r) {
        l->nextLink = NULL;
        list->lastLink = l;
    }   else    {
            r->prevLink = l;
    }	
}

_STATIC void thread_move_list (THREAD_LIST *dest, THREAD_LIST *src)
{
    THREAD *src_i = (THREAD *)src->firstLink;
    THREAD *src_next_i = (THREAD *)src_i->nextLink;
    while (src_i != (THREAD *)NULL) {
        thread_unlink(src, src_i);
        thread_link(dest, src_i);
        src_i = src_next_i;
        src_next_i = (THREAD *)src_next_i->nextLink;
    }
}

