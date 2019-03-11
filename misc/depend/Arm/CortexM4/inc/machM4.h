#ifndef CONTEXT_SWITCHING
#define CONTEXT_SWITCHING

#include <stdint.h>

#ifdef __cplusplus
    extern "C" {
#endif

#define WORD_T      uint32_t
#define HWORD_T     uint16_t
#define BYTE_T      uint8_t

#define UINT_T      uint32_t
#define INT_T       int32_t
        
#ifndef _PACKED
#define _PACKED  __packed
#endif   

#ifndef _VALUES_IN_REGS       
#define _VALUES_IN_REGS     __value_in_regs
#endif  

#ifndef _WEAK
#define _WEAK __weak
#endif        
        
#ifndef _STATIC
#define _STATIC static
#endif

#ifndef _EXTERN
#define _EXTERN extern
#endif


#ifndef _UNUSED
#define _UNUSED(a)
#endif


#pragma import VMTick   
#pragma import StackSwitchPSV   
#pragma import VMSvc
#pragma import VMInit
#pragma import VMStart

#define CPU_PRIV_ACCESS 0
#define CPU_UNPRIV_ACCESS 1

#define CPU_USE_MSP 0
#define CPU_USE_PSP 2

#define HANDLER_NOFPU_MSP   (0xF1U)
#define THREAD_NOFPU_MSP    (0xF9U)
#define THREAD_NOFPU_PSP    (0xFDU)
#define HANDLER_FPU_MSP     (0xE1U)
#define THREAD_FPU_MSP      (0xE9U)
#define THREAD_FPU_PSP      (0xEDU)

#define EXC_RETURN(exc)     (0xFFFFFF00U | exc)

#define EXC_RETURN_USE_FPU_BM   (0x10U)
#define EXC_RETURN_HANDLER_GM   (0xfU)
#define EXC_RETURN_HANDLER_VAL  (0x1)

#define FPU_STACK_SIZE      (33 * sizeof(WORD_T))
#define CPU_STACK_SIZE      (17 * sizeof(WORD_T))
    
#define STACK_ALLIGN        (8U)
    
#define CPU_XPSR_T_BM       (0x01000000U)

#define CPU_ACCESS_LEVEL_0 (CPU_USE_PSP | CPU_UNPRIV_ACCESS)
#define CPU_ACCESS_LEVEL_1 (CPU_USE_PSP | CPU_PRIV_ACCESS)
#define CPU_ACCESS_LEVEL_2 (CPU_USE_MSP | CPU_UNPRIV_ACCESS)
#define CPU_ACCESS_LEVEL_3 (CPU_USE_MSP | CPU_PRIV_ACCESS)


typedef INT_T (*_CALLBACK) (WORD_T, void *);

typedef _PACKED struct {
    WORD_T EXC_RET;
    WORD_T R11; /*user top*/
    WORD_T R10;
    WORD_T R9;
    WORD_T R8;
    WORD_T R7;
    WORD_T R6;
    WORD_T R5;
    WORD_T R4; /*irq top*/
    WORD_T R0; 
    WORD_T R1;
    WORD_T R2;
    WORD_T R3;
    WORD_T R12;
    WORD_T LR;
    WORD_T PC;
    WORD_T XPSR; /*pre irq top*/
    
} CPU_STACK; /*stack frame implementation for no fpu context store*/

typedef _PACKED struct {
    WORD_T S16[16];
    WORD_T EXC_RET;
    WORD_T R11;
    WORD_T R10;
    WORD_T R9;
    WORD_T R8;
    WORD_T R7;
    WORD_T R6;
    WORD_T R5;
    WORD_T R4;
    WORD_T R0;
    WORD_T R1;
    WORD_T R2;
    WORD_T R3;
    WORD_T R12;
    WORD_T LR;
    WORD_T PC;
    WORD_T XPSR;
    WORD_T S[16];
    WORD_T FPSCR;
} CPU_STACK_FPU; /*stack frame implementation for lazy fpu context store*/


typedef _PACKED struct {
    WORD_T EXC_RET;
    WORD_T RESERVED[8]; /*R10 - R4*/
    WORD_T POINTER;     /*R0*/
    WORD_T OPTION_A;    /*R1*/
    WORD_T OPTION_B;    /*R2*/
    WORD_T ERROR;       /*R3*/ 
    WORD_T PAD;         /* */
    WORD_T LINK;        /*Lr*/
    WORD_T PC;          /*PC*/
    WORD_T PSR;         /*XPSR*/
} CALL_CONTROL_CPU_STACK;

typedef _PACKED struct {
    WORD_T RESERVED0[16];       /*S16 - S31*/
    WORD_T EXC_RET;
    WORD_T RESERVED[8];         /*R10 - R4*/
    WORD_T POINTER;             /*R0*/
    WORD_T OPTION_A;            /*R1*/
    WORD_T OPTION_B;            /*R2*/
    WORD_T ERROR;               /*R3*/ 
    WORD_T PAD;                 /* */
    WORD_T LINK;                /*Lr*/
    WORD_T PC;                  /*PC*/
    WORD_T PSR;                 /*XPSR*/
} CALL_CONTROL_CPU_FPU_STACK;


#pragma anon_unions

typedef _PACKED struct {
  _PACKED union {
        CPU_STACK      cpuStack;
        CPU_STACK_FPU  cpuStackFpu;
      
        CALL_CONTROL_CPU_STACK callControl;
        CALL_CONTROL_CPU_FPU_STACK callControlFpu;
    };
} CPU_STACK_FRAME;

typedef _PACKED struct {
   _PACKED union {
        _PACKED struct {
           WORD_T R0;
           WORD_T R1;
           WORD_T R2;
           WORD_T R3; /*!*/
        };
        _PACKED struct {
           WORD_T POINTER;
           WORD_T CONTROL;
           WORD_T LINK;
           WORD_T ERROR; /*!*/
        };
        _PACKED struct {
           CPU_STACK_FRAME *FRAME;
        };
    };
} ARG_STRUCT_T;

#define THREAD_SET_REG(THREAD, REG, VAL) \
        do { \
            if (THREAD->USE_FPU == 0) { \
                THREAD->CPU_FRAME->callControl.REG = VAL; \
            } else { \
                THREAD->CPU_FRAME->callControlFpu.REG = VAL; \
            } \
        } while (0)
            
#define THREAD_GET_REG(THREAD, REG, VAR) \
        do { \
            if (THREAD->USE_FPU == 0) { \
                VAR = THREAD->CPU_FRAME->callControl.REG; \
            } else { \
                VAR = THREAD->CPU_FRAME->callControlFpu.REG; \
            } \
        while (0)


#define CPU_SET_REG(FRAME, TYPE, REG, VAL) \
        do { \
                        if ((TYPE & EXC_RETURN_USE_FPU_BM) == 0) \
                             FRAME->callControlFpu.REG = VAL; \
                        else \
                             FRAME->callControl.REG = VAL; \
                    } while (0)
            
#define CPU_GET_REG(FRAME, TYPE, REG, VAL) \
        do { \
                if ((TYPE & EXC_RETURN_USE_FPU_BM) == 0) \
                            VAL = FRAME->callControlFpu.REG; \
                        else \
                             VAL = FRAME->callControl.REG; \
                    } while (0); \
        
typedef struct {
    WORD_T ACTLR;   /*Auxiliary Control Register                            */
    WORD_T CPUID;   /*CPUID Base Register                                   */
    WORD_T ICSR;    /*Interrupt Control and State Register                  */
    WORD_T VTOR;    /*Vector Table Offset Register                          */
    WORD_T AIRCR;   /*Application Interrupt and Reset Control Register      */
    WORD_T SCR;     /*System Control Register                               */
    WORD_T CCR;     /*Configuration and Control Register                    */
    WORD_T SHPR1;   /*System Handler Priority Register 1                    */
    WORD_T SHPR2;   /*System Handler Priority Register 2                    */
    WORD_T SHPR3;   /*System Handler Priority Register 3                    */
    WORD_T SHCRS;   /*System Handler Control and State Register             */
    WORD_T CFSR;    /*Configurable Fault Status Register                    */
    WORD_T MMSRb;   /*MemManage Fault Status Register                       */
    WORD_T BFSRb;   /*BusFault Status Register                              */
    WORD_T UFSRb;   /*UsageFault Status Register                            */
    WORD_T HFSR;    /*HardFault Status Register                             */
    WORD_T MMAR;    /*MemManage Fault Address Register                      */
    WORD_T BFAR;    /*BusFault Address Register                             */
    WORD_T AFSR;    /*Auxiliary Fault Status Register                       */
} SCB_M4_TypeDef;   /*system control block for cortex m4 core               */


void machInitCore ();

#ifdef __cplusplus
    }
#endif

   
    
    
#endif /*CONTEXT_SWITCHING*/


/*End of file*/

