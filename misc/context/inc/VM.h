#ifndef VM_CLASS
#define VM_CLASS

#ifndef __cplusplus
    extern "C" {
#endif

#include "machM4.h"
#include "vm_conf.h"
    
INT_T VM_SYS_THREAD (WORD_T, void *);
        
enum {
    VM_OK,
    VM_SMALL_CORE,
    VM_CREATE_ERR,
    VM_DEPRECATED_CALL,
    VM_UNKNOWN_CALL,
    VM_NOT_FOUND,
    VM_DUP_CALL,
};
        
_VALUES_IN_REGS ARG_STRUCT_T VMINIT ();

        
#ifndef __cplusplus
    }
#endif        
        
        
#endif


/*End of file*/

