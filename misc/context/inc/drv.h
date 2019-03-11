#ifndef VM_DRV_H
#define VM_DRV_H



#ifdef __cplusplus
    extern "C" {
#endif
        
#include "machM4.h"       
        
typedef _PACKED struct {
    _CALLBACK Callback;
    INT_T (*StreamIn) (BYTE_T *, UINT_T);
    INT_T (*StreamOut) (BYTE_T *, UINT_T);
    WORD_T INT_VECTOR;
    const char *Name;
} DRIVER_HANDLER;    
        
        
#ifdef __cplusplus
    }
#endif


#endif  /*VM_DRV_H*/
