#ifndef VMAPI_INTERFACE
#define VMAPI_INTERFACE

#include "machM4.h"
#include "thread.h"
#include "mail.h"
#include "drv.h"
#include "vmapi_call.h"


_EXTERN "C" _VALUES_IN_REGS ARG_STRUCT_T upcall (ARG_STRUCT_T a);
_EXTERN "C" _VALUES_IN_REGS ARG_STRUCT_T VMBOOT();
_EXTERN _VALUES_IN_REGS ARG_STRUCT_T VMINIT();

_WEAK INT_T VMAPI_ErrorHandler (WORD_T from, _VALUES_IN_REGS ARG_STRUCT_T arg);


namespace vm {

_VALUES_IN_REGS ARG_STRUCT_T init ();
_VALUES_IN_REGS ARG_STRUCT_T start (); 
_VALUES_IN_REGS ARG_STRUCT_T restart (); 
    
_VALUES_IN_REGS ARG_STRUCT_T sleep (UINT_T delay);
_VALUES_IN_REGS ARG_STRUCT_T yield (void);
_VALUES_IN_REGS ARG_STRUCT_T create (THREAD_HANDLE *th);
_VALUES_IN_REGS ARG_STRUCT_T create (_CALLBACK callback, const char *name, WORD_T stack, WORD_T prio, WORD_T size, void *arg);
_VALUES_IN_REGS ARG_STRUCT_T call (_CALLBACK callback, const char *name, WORD_T stack, WORD_T prio, WORD_T size, void *arg);
_VALUES_IN_REGS ARG_STRUCT_T create_drv (DRIVER_HANDLER *dh);
_VALUES_IN_REGS ARG_STRUCT_T lock (UINT_T id);   
_VALUES_IN_REGS ARG_STRUCT_T unlock (UINT_T id);      
_VALUES_IN_REGS ARG_STRUCT_T notify (const char *name);   
_VALUES_IN_REGS ARG_STRUCT_T wait_notify ();  
_VALUES_IN_REGS ARG_STRUCT_T notify_wait (const char *name);   
_VALUES_IN_REGS ARG_STRUCT_T sync (const char *name);       
_VALUES_IN_REGS ARG_STRUCT_T wait (THREAD_COND_T cond);
_VALUES_IN_REGS ARG_STRUCT_T wait_event (const char *event_name);
_VALUES_IN_REGS ARG_STRUCT_T fire_event (const char *event_name);   
_VALUES_IN_REGS ARG_STRUCT_T mail (char *name, MAIL_HANDLE *mail);
_VALUES_IN_REGS ARG_STRUCT_T wait_mail ();
_VALUES_IN_REGS ARG_STRUCT_T timer (WORD_T *dest, WORD_T id);
_VALUES_IN_REGS ARG_STRUCT_T timer_remove (WORD_T id);

_VALUES_IN_REGS ARG_STRUCT_T critical ();
_VALUES_IN_REGS ARG_STRUCT_T end_critical ();
_VALUES_IN_REGS ARG_STRUCT_T exit (UINT_T ret);   
_VALUES_IN_REGS ARG_STRUCT_T fault (const char *cause); 
void reset (); 
  
};


#define _XCALL(ret, callback, size, arg) \
WORD_T ret; \
do { \
    ARG_STRUCT_T callback##_ret = vm::call(callback, #callback, VM_DEF_THREAD_HEAP_SIZE, VM_THREAD_DEF_PRIORITY, size, arg); \
    if (callback##_ret.ERROR != VM_OK) { \
        VMAPI_ErrorHandler(VMAPI_CALL, callback##_ret); \
    } else { \
        \
    } \
    ret = callback##_ret.POINTER; \
    _UNUSED(ret); \
} while (0)

#define __XCALL(ret, stack, callback, size, arg) \
WORD_T ret; \
do { \
    ARG_STRUCT_T callback##_ret = vm::call(callback, #callback, stack, VM_THREAD_DEF_PRIORITY, size, arg); \
    if (callback##_ret.ERROR != VM_OK) { \
        VMAPI_ErrorHandler(VMAPI_CALL, callback##_ret); \
    } else { \
        \
    } \
    ret = callback##_ret.POINTER; \
    _UNUSED(ret); \
} while (0)

  

#endif


/*End of file*/


