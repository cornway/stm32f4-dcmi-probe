#include "VM.h"
#include "thread.h"
#include "mail.h"
#include "timer.h"
#include "drv.h"
#include "mutex.h"
#include "vmapi_call.h"
#include "string.h"

_EXTERN "C" void *vmalloc (UINT_T size);
_EXTERN "C" void vmfree (void *p);

_EXTERN "C" _VALUES_IN_REGS ARG_STRUCT_T upcall (ARG_STRUCT_T arg);
_EXTERN "C" void SystemSoftReset (void);

_STATIC _VALUES_IN_REGS ARG_STRUCT_T DISPATCH (ARG_STRUCT_T ARG);
_STATIC _VALUES_IN_REGS ARG_STRUCT_T DISPATCH_SVC (ARG_STRUCT_T arg);
_STATIC _VALUES_IN_REGS ARG_STRUCT_T VMRUN ();
_STATIC WORD_T SET_LINK (THREAD *t);
_STATIC THREAD *GET_READY ();
_STATIC void SET_STACK (THREAD *t, ARG_STRUCT_T arg);
_STATIC THREAD *PICK_READY ();
_STATIC _VALUES_IN_REGS ARG_STRUCT_T UPDATE (THREAD *t);



_STATIC THREAD *CUR_THREAD = (THREAD *)NULL;
_STATIC THREAD *IDLE_THREAD = (THREAD *)NULL;

unsigned long uWTick = 0;
BYTE_T preemtSwitchEnabled = 1;

_STATIC MUTEX_FACTORY mutexFactory;
_STATIC TIMER_FACTORY timerFactory;

_VALUES_IN_REGS ARG_STRUCT_T VMINIT ()
{
    ARG_STRUCT_T ret = {0, 0, 0, 0};
    static THREAD *t = (THREAD *)NULL;
    
    machInitCore();
    t_init_core();
    
    int res = t_init (          &t, 
                                VM_SYS_THREAD,
                                VM_THREAD_MAX_PRIORITY - 1,
                                IDLE_THREAD_ID,
                                VM_IDLE_THREAD_HEAP_SIZE,
                                CPU_ACCESS_LEVEL_1,
                                "SYS_IDLE",
                                0,
                                NULL
                            );
    if (res != T_OK) {
        ret.R0 = VM_CREATE_ERR;
        return ret;
    }
    t_link_ready(t);
    IDLE_THREAD = t;
    
    t->USE_FPU = THREAD_NO_FPU;
    
    ret.R0 = VM_OK;
    return ret;
}

_VALUES_IN_REGS ARG_STRUCT_T VMBREAK (UINT_T ret)
{
    if (CUR_THREAD->ID == IDLE_THREAD_ID) {
        for (;;) {}
    }
    ARG_STRUCT_T arg = {VMAPI_EXIT, ret, 0, 0};
    return upcall(arg);
}

_STATIC WORD_T SET_LINK (THREAD *t)
{
    if (t->USE_FPU == THREAD_FPU) {
        if ((t->PRIVILEGE & CPU_USE_PSP) == CPU_USE_PSP) {
            return EXC_RETURN(THREAD_FPU_PSP);
        } else {
            return EXC_RETURN(THREAD_FPU_MSP);
        }
    } else {
        if ((t->PRIVILEGE & CPU_USE_PSP) == CPU_USE_PSP) {
            return EXC_RETURN(THREAD_NOFPU_PSP);
        } else {
            return EXC_RETURN(THREAD_NOFPU_MSP);
        }
    }
}

_STATIC void SET_STACK (THREAD *t, ARG_STRUCT_T arg)
{
    if ((arg.LINK & EXC_RETURN_USE_FPU_BM) == 0) { /**/
        /*FPU pending*/
        t->USE_FPU = THREAD_FPU;
    } else {
        t->USE_FPU = THREAD_NO_FPU;
    }
    t->CPU_FRAME = arg.FRAME;
    
}

_STATIC _VALUES_IN_REGS ARG_STRUCT_T UPDATE (THREAD *t)
{
    ARG_STRUCT_T ret = {0, 0, 0, 0};
    ret.LINK = SET_LINK(t); /*set link register value*/
    ret.CONTROL = t->PRIVILEGE;
    ret.FRAME = t->CPU_FRAME;
    return ret;
}

_STATIC THREAD *GET_READY ()
{
    THREAD *t;
    for (UINT_T i = 0; i < VM_THREAD_MAX_PRIORITY; i++) {
        t = t_ready(i);
        if (t != (THREAD *)NULL) {
            break;
        }
    }
    return t;
}
_STATIC THREAD *PICK_READY ()
{
    THREAD *t;
    
    if (t_check_list() == 0) {
        t_refresh( t_link_ready );
    }
    t = GET_READY();
    if (t == (THREAD *)NULL) {
        t = IDLE_THREAD; /*Fatal error*/
    }  
    if (t->STATUS != THREAD_STOP) {
        t_unchain(t_link_ready, t);
    } 
    t->STATUS = THREAD_RUN;
    return t;
}

_STATIC _VALUES_IN_REGS ARG_STRUCT_T VMRUN ()
{
    ARG_STRUCT_T ret = {0, 0, 0, 0};
    
    CUR_THREAD = IDLE_THREAD;
    
    ret.CONTROL = IDLE_THREAD->PRIVILEGE;
    ret.FRAME = IDLE_THREAD->CPU_FRAME;
    return ret;
}

_STATIC _VALUES_IN_REGS ARG_STRUCT_T DISPATCH (ARG_STRUCT_T arg)
{
    uWTick++;
    
    THREAD *t = CUR_THREAD;
    ARG_STRUCT_T ret = {0, 0, 0, 0};
    
    timerFactory.tick_ms();
    
    if (((arg.LINK & EXC_RETURN_HANDLER_GM) == EXC_RETURN_HANDLER_VAL) || ((uWTick & 0x9U) == 0) || (preemtSwitchEnabled == 0)) { /*return, if another IRQ is pending*/
        ret.POINTER = arg.POINTER;
        ret.LINK    = arg.LINK;
        ret.CONTROL = t->PRIVILEGE;
        return ret;
    }
    
    SET_STACK(t, arg);
    t_unlink_ready(t);
    
    t->STATUS = THREAD_PEND;
    t->V_PRIORITY++;
    if (t->V_PRIORITY < VM_THREAD_MAX_PRIORITY) {
        t_link_ready(t);
    } else {
        t_link_drop(t);
    } 
    
    t_check_cond( t_link_ready );
    t_tick( t_link_drop );
    
    CUR_THREAD = PICK_READY();
    
    return UPDATE(CUR_THREAD);
}

_VALUES_IN_REGS ARG_STRUCT_T DISPATCH_SVC (ARG_STRUCT_T arg)
{

    SET_STACK(CUR_THREAD, arg);
    /*
    call_struct.R0[7 : 0] - call reason, if == 0 -> restart (not reset !);
    call_struct.R1 - pointer to handle struct;
    call_struct.R2 - attribute 0;
    call_struct.R2 - attribute 1;
    */
    ARG_STRUCT_T call_struct = {0, 0, 0, 0};
    CPU_STACK_FRAME *frame = CUR_THREAD->CPU_FRAME;
    
    
    CPU_GET_REG(frame, arg.LINK, POINTER, call_struct.R0);
    CPU_GET_REG(frame, arg.LINK, OPTION_A, call_struct.R1);
    CPU_GET_REG(frame, arg.LINK, OPTION_B, call_struct.R2);
    CPU_GET_REG(frame, arg.LINK, ERROR, call_struct.R3);
    
    /*frame->cpuStack.R0 - pointer to resource*/
    /*frame->cpuStack.R1 - attribute 1*/
    /*frame->cpuStack.R2 - attribute 2*/
    /*frame->cpuStack.R3 - error code*/
    
    ARG_STRUCT_T ret;
    ret.POINTER = arg.POINTER;
    ret.LINK    = arg.LINK;
    ret.ERROR   = arg.ERROR;
    ret.CONTROL = CUR_THREAD->PRIVILEGE;
    if (call_struct.R0 == VMAPI_RESET) {
        
    }
    if (call_struct.R0 == VMAPI_RESET) {
        SystemSoftReset();
        for (;;) {}
    } else {
        BYTE_T reason = call_struct.R0 & 0xFFU;
        if (CUR_THREAD->ID == IDLE_THREAD_ID) {
            if ((reason & DEPREC_IDLE_CALL) == DEPREC_IDLE_CALL) {
                CPU_SET_REG(frame, arg.R2, ERROR, VM_DEPRECATED_CALL);
                return ret;
            }
        }
        
        BYTE_T force = 0;
        static THREAD *tn = (THREAD *)NULL;
        THREAD_HANDLE *th = NULL;
        WORD_T res = VM_OK;
        switch (reason) {
            case VMAPI_SLEEP :  CUR_THREAD->DELAY = call_struct.R1;
                                t_unlink_ready(CUR_THREAD);
                                t_link_delay(CUR_THREAD);
                                force = 0x1U;
                break;
            case VMAPI_YIELD :  t_unlink_ready(CUR_THREAD);
                                t_link_drop(CUR_THREAD);
                                force = 0x1U;
                break;
            case VMAPI_CREATE : th = (THREAD_HANDLE *)call_struct.R1;
                                res = t_init (
                                                        &tn, 
                                                        th->Callback,
                                                        th->Priority,
                                                        (WORD_T)th->Callback,
                                                        th->StackSize,
                                                        CPU_ACCESS_LEVEL_0,
                                                        th->Name,
                                                        th->argSize,
                                                        th->Arg
                                                        );
                                if (res != VM_OK) {
                                    CPU_SET_REG(frame, arg.LINK, ERROR, res);
                                    break;
                                }
                                t_link_ready(tn);
                                CPU_SET_REG(frame, arg.LINK, ERROR, VM_OK);
                                break;
                //break;
            case VMAPI_CALL :   th = (THREAD_HANDLE *)call_struct.R1;
                                res = t_init (
                                                        &tn, 
                                                        th->Callback,
                                                        th->Priority,
                                                        (WORD_T)th->Callback,
                                                        th->StackSize,
                                                        CPU_ACCESS_LEVEL_0,
                                                        th->Name,
                                                        th->argSize,
                                                        th->Arg
                                                        );
                                if (res != VM_OK) {
                                    CPU_SET_REG(frame, arg.LINK, ERROR, res);
                                    break;
                                }
                                t_link_ready(tn);
                                t_unlink_ready(CUR_THREAD);
                                tn->caller = CUR_THREAD;
                                
                                CUR_THREAD = tn;
                                ret.LINK = SET_LINK(CUR_THREAD);
                                ret.FRAME = CUR_THREAD->CPU_FRAME;
                                ret.CONTROL = CUR_THREAD->PRIVILEGE;
                                break;
                //break;
            case VMAPI_CREATE_DRV : CPU_SET_REG(frame, arg.LINK, ERROR, VM_OK);
                                    break;
                //break;
            case VMAPI_LOCK :   WORD_T mutex_res = mutexFactory.lock(CUR_THREAD, call_struct.R1);
                                if (mutex_res == MUTEX_GRANT_LOCK) { /*succeed locked*/
                                    CPU_SET_REG(frame, arg.LINK, ERROR, VM_OK);
                                } else if (mutex_res == MUTEX_GRANT_WAIT) { /*thread will be unlinked and queued*/
                                    t_unlink_ready(CUR_THREAD);
                                    CUR_THREAD->mutex = 1;
                                    force = 1;
                                } else { /*error*/
                                    t_unlink_ready(CUR_THREAD);
                                    t_link_fault(CUR_THREAD);
                                    CUR_THREAD->fault = 1;
                                    CUR_THREAD->faultMessage = "Cannot allocate mutex resource";
                                    force = 1;
                                }
                                
                break;
            case VMAPI_UNLOCK : THREAD *owner = mutexFactory.unlock(CUR_THREAD, call_struct.R1);
                                if (owner != nullptr) { /*set next owner and unlock mutex for it*/
                                    t_link_ready(owner);
                                    owner->mutex = 0;
                                    CUR_THREAD = owner;
                                    ret = UPDATE(CUR_THREAD);
                                    break;
                                }
                                CPU_SET_REG(frame, arg.LINK, ERROR, VM_OK);
                                break;
                                
            case VMAPI_NOTIFY :   THREAD *recv = t_notify((const char *)call_struct.R1);
                                  if ((recv != nullptr) && (recv->waitNotify == 1)) {
                                      recv->waitNotify = 0;
                                      t_unlink_notify(recv);
                                      t_link_ready(recv);
                                      CPU_SET_REG(frame, arg.LINK, ERROR, VM_OK);
                                  } else {
                                    CPU_SET_REG(frame, arg.LINK, ERROR, VM_NOT_FOUND);
                                  }
                                  break;
                //break;
            case VMAPI_SYNC :   THREAD *syn = t_search((const char *)call_struct.R1);
                                if (syn == nullptr) {
                                    break;
                                }
                                if (syn->fault) {
                                    break;
                                }
                                t_unlink_ready(CUR_THREAD);
                                t_link_chain(syn, CUR_THREAD);
                                force = 1;
                                break;
                //break;
            case VMAPI_NOTIFY_WAIT :  THREAD *recv__ = t_notify((const char *)call_struct.R1);
                                      if ((recv__ != nullptr) && (recv__->waitNotify == 1)) {
                                          recv__->waitNotify = 0;
                                          t_unlink_notify(recv__);
                                          t_link_ready(recv__);
                                      }
                                      if (CUR_THREAD->waitNotify == 0) {
                                          CUR_THREAD->waitNotify = 1;
                                          t_unlink_ready(CUR_THREAD);
                                          t_link_notify(CUR_THREAD);
                                      } else {
                                          CUR_THREAD->fault = 1;
                                          CUR_THREAD->faultMessage = "thread cannot wait for notify twice";
                                          t_unlink_ready(CUR_THREAD);
                                          t_link_fault(CUR_THREAD);
                                          CPU_SET_REG(frame, arg.LINK, ERROR, VM_DUP_CALL);
                                      }
                                      CPU_SET_REG(frame, arg.LINK, ERROR, VM_OK);
                break;
            case VMAPI_WAIT_NOTIFY :    if (CUR_THREAD->waitNotify == 1) { /*error*/
                                            CUR_THREAD->fault = 1;
                                            CUR_THREAD->faultMessage = "thread cannot wait for notify twice";
                                            t_unlink_ready(CUR_THREAD);
                                            t_link_fault(CUR_THREAD);
                                            CPU_SET_REG(frame, arg.LINK, ERROR, VM_DUP_CALL);
                                            break;
                                        } else {
                                            CUR_THREAD->waitNotify = 1;
                                            t_unlink_ready(CUR_THREAD);
                                            t_link_notify(CUR_THREAD);
                                            CPU_SET_REG(frame, arg.LINK, ERROR, VM_OK);
                                        }
                                        force = 1;
                break;
            case VMAPI_WAIT : CUR_THREAD->cond = (THREAD_COND_T)call_struct.R1;
                              t_unlink_ready(CUR_THREAD);
                              t_link_cond(CUR_THREAD);
                              force = 1;
                break;
            case VMAPI_WAIT_EVENT : CUR_THREAD->event = (const char *)call_struct.R1;
                                    t_unlink_ready(CUR_THREAD);
                                    t_link_wait_event(CUR_THREAD);
                                    CPU_SET_REG(frame, arg.LINK, ERROR, VM_OK);
                                    force = 1;
                                    break;
                //break;
            case VMAPI_FIRE_EVENT : t_fire_event(t_link_ready, (const char *)call_struct.R1);
                                    CPU_SET_REG(frame, arg.LINK, ERROR, VM_OK);
                                    break;
                //break;
            case VMAPI_WAIT_MAIL :      if (CUR_THREAD->waitNotify == 1) { /*error*/
                                            CUR_THREAD->fault = 1;
                                            CUR_THREAD->faultMessage = "thread cannot wait for mail twice";
                                            t_unlink_ready(CUR_THREAD);
                                            t_link_fault(CUR_THREAD);
                                            break;
                                        } else {
                                            CUR_THREAD->waitNotify = 1;
                                            t_unlink_ready(CUR_THREAD);
                                            t_link_mail(CUR_THREAD);
                                        }
                                        force = 1;
                break;
            case VMAPI_MAIL :     THREAD *recvm = t_notify((const char *)call_struct.R1);
                                  if ((recvm != nullptr) && (recvm->waitNotify == 1)) {
                                      recv->waitNotify = 0;
                                      t_unlink_mail(recvm);
                                      t_link_ready(recvm);
                                      THREAD_SET_REG(recvm, POINTER, call_struct.R2);
                                  } else {
                                    CPU_SET_REG(frame, arg.LINK, ERROR, VM_NOT_FOUND);
                                  }
                                  break;
                //break;
            case VMAPI_TIMER_CREATE :   if (timerFactory.create((WORD_T *)call_struct.R1, call_struct.R2) < 0) {
                                            CPU_SET_REG(frame, arg.LINK, ERROR, VM_CREATE_ERR);
                                        } else {
                                            CPU_SET_REG(frame, arg.LINK, ERROR, VM_OK);
                                        }
                                        
                break;
            case VMAPI_TIMER_REMOVE :   if (timerFactory.remove(call_struct.R1) < 0) {
                                            CPU_SET_REG(frame, arg.LINK, ERROR, VM_NOT_FOUND);
                                        } else {
                                            CPU_SET_REG(frame, arg.LINK, ERROR, VM_OK);
                                        }
                break;
            case VMAPI_CRITICAL : preemtSwitchEnabled = 0;
                break;
            case VMAPI_END_CRITICAL : preemtSwitchEnabled = 1;
                break;
            case VMAPI_FAULT :  CUR_THREAD->fault = 1;
                                CUR_THREAD->faultMessage = (char *)call_struct.R1;
                                t_unlink_ready(CUR_THREAD);
                                force = 0x1U;                           
                break;
            case VMAPI_EXIT :   t_unlink_ready(CUR_THREAD);
                                tn = (THREAD *)CUR_THREAD->caller;
                                if (tn != (THREAD *)NULL) {
                                    t_link_ready(tn);
                                    THREAD_SET_REG(tn, POINTER, call_struct.R1);
                                    CUR_THREAD = tn;
                                    ret.LINK = SET_LINK(CUR_THREAD);
                                    ret.FRAME = CUR_THREAD->CPU_FRAME;
                                    ret.CONTROL = CUR_THREAD->PRIVILEGE;
                                    break;
                                }
                                t_destroy(CUR_THREAD);
                                force = 0x1U;                                  
                break;
            default :   
                        CPU_SET_REG(frame, arg.LINK, ERROR, VM_UNKNOWN_CALL);
                        break;
                //break;
        }
        if (force) {
            CUR_THREAD = PICK_READY();
            
            CPU_SET_REG(frame, arg.LINK, ERROR, VM_OK);
            ret.LINK = SET_LINK(CUR_THREAD);
            ret.FRAME = CUR_THREAD->CPU_FRAME;
            ret.CONTROL = CUR_THREAD->PRIVILEGE;
            
            return ret;
        }
        
        return ret;
    }
    
    //return ret;
}


extern "C"
_VALUES_IN_REGS ARG_STRUCT_T VMTick (ARG_STRUCT_T arg)
{
    return DISPATCH(arg);
}

extern "C"
_VALUES_IN_REGS ARG_STRUCT_T VMInit (ARG_STRUCT_T arg)
{
    ARG_STRUCT_T ret = {arg.R0, arg.R1, arg.R2, arg.R3};
    return ret;
}

extern "C"
_VALUES_IN_REGS ARG_STRUCT_T VMStart ()
{
    return VMRUN();
}

extern "C"
_VALUES_IN_REGS ARG_STRUCT_T VMSvc (ARG_STRUCT_T arg)
{
    return DISPATCH_SVC(arg);
}

extern "C"
_VALUES_IN_REGS ARG_STRUCT_T VMHardFault (ARG_STRUCT_T arg)
{
    for (;;) {
        
    }
}

extern "C"
void *StackSwitchPSV (void *frame, int32_t link)
{
    return frame;
}


