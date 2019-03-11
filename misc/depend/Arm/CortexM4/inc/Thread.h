#ifndef VM_THREAD
#define VM_THREAD

#ifdef __cplusplus
    extern "C" {
#endif

#include "stdint.h"
#include "vm_conf.h"
#include "machM4.h"
        
#ifndef     THREAD_MAX_COUNT    
#define     THREAD_MAX_COUNT (24U)
#endif

#ifndef     VM_THREAD_MAX_PRIORITY  
#define     VM_THREAD_MAX_PRIORITY  (8U)
#endif
        
#ifndef     VM_DEF_THREAD_HEAP_SIZE
#define     VM_DEF_THREAD_HEAP_SIZE (6144U)
#endif
        
#ifndef     VM_MIN_THREAD_HEAP_SIZE
#define     VM_MIN_THREAD_HEAP_SIZE (FPU_STACK_SIZE + CPU_STACK_SIZE + 128)
#endif
        
#ifndef     VM_DEF_THREAD_NAME_LEN
#define     VM_DEF_THREAD_NAME_LEN (24U)
#endif
        
#ifndef     NULL
#define     NULL    ((void *)0)
#endif

#define     IDLE_THREAD_ID  (0U)    

typedef _PACKED struct {
    _CALLBACK Callback;
    WORD_T StackSize;
    BYTE_T Priority;
    const char *Name;
    WORD_T argSize;
    void *Arg;
} THREAD_HANDLE;
       
typedef INT_T (*THREAD_COND_T) (WORD_T type, void *link);
        
enum {
    THREAD_STOP = 0x0U,
    THREAD_RUN  = 0x1U,
    THREAD_PEND = 0x2U,
};

enum {
    THREAD_NO_FPU = 0x0U,
    THREAD_FPU = 0x1U,
};

enum {
    T_OK,
    T_LONG_NAME,
    T_SMALL_HEAP,
    T_SMALL_CORE,
    T_NULL_CALLBACK,
    T_PRIV_UNDEF,
    T_PRIOR_UNDEF,
};


        
typedef _PACKED struct {
    void *firstLink;
    void *lastLink;
    BYTE_T elements;
} THREAD_LIST;

typedef _PACKED struct {
    CPU_STACK_FRAME *CPU_FRAME;
    WORD_T DELAY;
    
    BYTE_T PRIVILEGE;
    WORD_T STACK;
    WORD_T STACK_SIZE;
    INT_T  ID;
    BYTE_T PRIORITY;
    BYTE_T V_PRIORITY;
    char name[VM_DEF_THREAD_NAME_LEN];
    void *nextLink;
    void *prevLink;
    
    unsigned STATUS : 3;
    unsigned USE_FPU : 1;
    unsigned mutex : 1;
    unsigned monitor : 1;
    
    unsigned fault : 1;
    const char *faultMessage;
    
    unsigned waitNotify : 1;
    _PACKED union {
        void *link;
        THREAD_COND_T cond;
        const char *event;
    };
    
    void *caller;
    
    
    THREAD_LIST chain;
} THREAD;



_VALUES_IN_REGS ARG_STRUCT_T VMBREAK (UINT_T ret);

void t_init_core (void);
void t_destroy (THREAD *t);
int t_init (THREAD **t, _CALLBACK callback, BYTE_T priority, WORD_T id, WORD_T heap_size, BYTE_T privilege, const char *name, WORD_T arg_size, void *arg);
THREAD *t_ready (UINT_T priority);
THREAD *t_search (const char *name);
void t_unlink_ready (THREAD *t);
void t_link_chain (THREAD *t, THREAD *l);
void t_unchain (void (*linker) (THREAD *), THREAD *t);
void t_link_ready (THREAD *t);
void t_link_drop (THREAD *t);
void t_link_delay (THREAD *t);
void t_link_fault (THREAD *t);
void t_link_notify (THREAD *t);
void t_unlink_notify (THREAD *t);
void t_link_mail (THREAD *t);
void t_unlink_mail(THREAD *t);
THREAD *t_mail (const char *name);
void t_link_cond (THREAD *t);
WORD_T t_check_cond (void (*linker) (THREAD *));
void t_link_wait_event (THREAD *t);
WORD_T t_fire_event (void (*linker) (THREAD *), const char *event_name);
THREAD *t_notify (const char *name);
int t_check_list (void);
void t_refresh (void (*linker) (THREAD *));
void t_tick (void (*linker) (THREAD *));
        
#ifdef __cplusplus
    }
#endif

#endif /*VM_THREAD*/


/*End of file*/

