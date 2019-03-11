#ifndef VMAPI_CALL_H
#define VMAPI_CALL_H

#ifdef __cplusplus
    extern "C" {
#endif
        
#define DEPREC_IDLE_CALL    (0x80U)

enum {
    VMAPI_RESTART       = 0x00U,
    VMAPI_SLEEP         = DEPREC_IDLE_CALL | 0x1U,
    VMAPI_YIELD         = 0x02U,
    VMAPI_CREATE        = 0x03U,
    VMAPI_CREATE_DRV    = 0x04U,
    VMAPI_LOCK          = DEPREC_IDLE_CALL | 0x05U,
    VMAPI_UNLOCK        = DEPREC_IDLE_CALL | 0x06U,
    VMAPI_NOTIFY        = 0x09U,
    VMAPI_WAIT_NOTIFY   = DEPREC_IDLE_CALL | 0x0AU,
    VMAPI_NOTIFY_WAIT   = DEPREC_IDLE_CALL | 0x0BU,
    VMAPI_SYNC          = DEPREC_IDLE_CALL | 0x0CU,
    VMAPI_WAIT          = DEPREC_IDLE_CALL | 0x0DU,
    VMAPI_WAIT_EVENT    = DEPREC_IDLE_CALL | 0x0EU,
    VMAPI_FIRE_EVENT    = 0x0FU,
    VMAPI_MAIL          = 0x10,
    VMAPI_WAIT_MAIL     = DEPREC_IDLE_CALL | 0x11,
    VMAPI_TIMER_CREATE  = 0x12,
    VMAPI_TIMER_REMOVE  = 0x13,
    VMAPI_CALL          = 0x14,
    VMAPI_CRITICAL      = 0x15,
    VMAPI_END_CRITICAL  = 0x16,
    
    VMAPI_EXIT          = 0x7E,
    VMAPI_FAULT         = 0x7FU,
    
    VMAPI_RESET         = 0x80000U,
};    
        
        
#ifdef __cplusplus
    }
#endif
        
#endif /*VMAPI_CALL_H*/
