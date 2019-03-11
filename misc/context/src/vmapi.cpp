
#include "vmapi.h"




_VALUES_IN_REGS ARG_STRUCT_T vm::init ()
{
    return VMINIT();
}
_VALUES_IN_REGS ARG_STRUCT_T vm::start ()
{
    return VMBOOT();
}

_VALUES_IN_REGS ARG_STRUCT_T vm::restart ()
{
    ARG_STRUCT_T arg = {0, 0, 0, 0};
    return upcall(arg);
}

_VALUES_IN_REGS ARG_STRUCT_T vm::sleep (UINT_T delay)
{
    ARG_STRUCT_T arg = {VMAPI_SLEEP, delay, 0, 0};
    return upcall(arg);
}

_VALUES_IN_REGS ARG_STRUCT_T vm::yield (void)
{
    ARG_STRUCT_T arg = {VMAPI_YIELD, 0, 0, 0};
    return upcall(arg);
}

_VALUES_IN_REGS ARG_STRUCT_T vm::create (THREAD_HANDLE *th)
{
    ARG_STRUCT_T arg = {VMAPI_CREATE, (WORD_T)th, 0, 0};
    return upcall(arg);
}

_VALUES_IN_REGS ARG_STRUCT_T vm::create (_CALLBACK callback, const char *name, WORD_T stack, WORD_T prio, WORD_T size, void *arg)
{
    static THREAD_HANDLE th;
    th.Callback = callback;
    th.Name = name;
    th.Priority = prio;
    th.StackSize = stack;
    th.argSize = size;
    th.Arg = arg;
    return vm::create(&th);
}

_VALUES_IN_REGS ARG_STRUCT_T vm::call (_CALLBACK callback, const char *name, WORD_T stack, WORD_T prio, WORD_T size, void *arg)
{
    static THREAD_HANDLE th;
    th.Callback = callback;
    th.Name = name;
    th.Priority = prio;
    th.StackSize = stack;
    th.argSize = size;
    th.Arg = arg;
    ARG_STRUCT_T __arg = {VMAPI_CALL, (WORD_T)&th, 0, 0};
    return upcall(__arg);
}

_VALUES_IN_REGS ARG_STRUCT_T vm::create_drv (DRIVER_HANDLER *dh)
{
    ARG_STRUCT_T arg = {VMAPI_CREATE_DRV, (WORD_T)dh, 0, 0};
    return upcall(arg);
}

_VALUES_IN_REGS ARG_STRUCT_T vm::lock (UINT_T id)
{
    ARG_STRUCT_T arg = {VMAPI_LOCK, id, 0, 0};
    return upcall(arg);
}    
_VALUES_IN_REGS ARG_STRUCT_T vm::unlock (UINT_T id)
{
    ARG_STRUCT_T arg = {VMAPI_UNLOCK, id, 0, 0};
    return upcall(arg);
}  

_VALUES_IN_REGS ARG_STRUCT_T vm::notify (const char *name)
{
    ARG_STRUCT_T arg = {VMAPI_NOTIFY, (WORD_T)name, 0, 0};
    return upcall(arg);
}

_VALUES_IN_REGS ARG_STRUCT_T vm::wait_notify ()
{
    ARG_STRUCT_T arg = {VMAPI_WAIT_NOTIFY, 0, 0, 0};
    return upcall(arg);
}

_VALUES_IN_REGS ARG_STRUCT_T vm::notify_wait (const char *name)
{
    ARG_STRUCT_T arg = {VMAPI_NOTIFY_WAIT, (WORD_T)name, 0, 0};
    return upcall(arg);
}

_VALUES_IN_REGS ARG_STRUCT_T vm::sync (const char *name)
{
    ARG_STRUCT_T arg = {VMAPI_SYNC, (WORD_T)name, 0, 0};
    return upcall(arg);
}

_VALUES_IN_REGS ARG_STRUCT_T vm::wait (THREAD_COND_T cond)
{
    ARG_STRUCT_T arg = {VMAPI_WAIT, (WORD_T)cond, 0, 0};
    return upcall(arg);
}

_VALUES_IN_REGS ARG_STRUCT_T vm::wait_event (const char *event_name)
{
    ARG_STRUCT_T arg = {VMAPI_WAIT_EVENT, (WORD_T)event_name, 0, 0};
    return upcall(arg);
}

_VALUES_IN_REGS ARG_STRUCT_T vm::fire_event (const char *event_name)
{
    ARG_STRUCT_T arg = {VMAPI_FIRE_EVENT, (WORD_T)event_name, 0, 0};
    return upcall(arg);
}

_VALUES_IN_REGS ARG_STRUCT_T vm::mail (char *name, MAIL_HANDLE *mail)
{
    ARG_STRUCT_T arg = {VMAPI_MAIL, (WORD_T)name, (WORD_T)mail, 0};
    return upcall(arg);
}

_VALUES_IN_REGS ARG_STRUCT_T vm::wait_mail ()
{
    ARG_STRUCT_T arg = {VMAPI_WAIT_MAIL, 0, 0, 0};
    return upcall(arg);
}

_VALUES_IN_REGS ARG_STRUCT_T vm::timer (WORD_T *dest, WORD_T id)
{
    ARG_STRUCT_T arg = {VMAPI_TIMER_CREATE, WORD_T(dest), id, 0};
    return upcall(arg);
}

_VALUES_IN_REGS ARG_STRUCT_T vm::timer_remove (WORD_T id)
{
    ARG_STRUCT_T arg = {VMAPI_TIMER_REMOVE, id, 0, 0};
    return upcall(arg);
}

_VALUES_IN_REGS ARG_STRUCT_T vm::critical ()
{
    ARG_STRUCT_T arg = {VMAPI_CRITICAL, 0, 0, 0};
    return upcall(arg);
}

_VALUES_IN_REGS ARG_STRUCT_T vm::end_critical ()
{
    ARG_STRUCT_T arg = {VMAPI_END_CRITICAL, 0, 0, 0};
    return upcall(arg);
}

_VALUES_IN_REGS ARG_STRUCT_T vm::exit (UINT_T ret)
{
    ARG_STRUCT_T arg = {VMAPI_EXIT, ret, 0, 0};
    return upcall(arg);
}

_VALUES_IN_REGS ARG_STRUCT_T vm::fault (const char *cause)
{
    ARG_STRUCT_T arg = {VMAPI_FAULT, (WORD_T)cause, 0, 0};
    return upcall(arg);
}

void vm::reset ()
{
    ARG_STRUCT_T arg = {VMAPI_RESET, 0, 0, 0};
    upcall(arg);
}


INT_T VMAPI_ErrorHandler (WORD_T from, _VALUES_IN_REGS ARG_STRUCT_T arg)
{
    _UNUSED(from);
    _UNUSED(arg);
    for (;;) {}
    //return 0;
}

/*End of file*/

