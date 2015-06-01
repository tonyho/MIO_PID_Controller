;********************************************************************************************************
;                                               uC/OS-II
;                                         The Real-Time Kernel
;
;                               (c) Copyright 1992-2006, Micrium, Weston, FL
;                                          All Rights Reserved
;
;                                           Generic ARM Port
;
; File      : OS_CPU_A.ASM
; Version   : V2.86
; By        : Jean J. Labrosse
;
; For       : ARMv7M Cortex-M3
; Mode      : Thumb2
; Toolchain : RealView Development Suite
;             RealView Microcontroller Development Kit (MDK)
;             ARM Developer Suite (ADS)
;             Keil uVision
;********************************************************************************************************

;********************************************************************************************************
;                                           PUBLIC FUNCTIONS
;********************************************************************************************************

    EXTERN  OSRunning                                           ; External references
    EXTERN  OSPrioCur
    EXTERN  OSPrioHighRdy
    EXTERN  OSTCBCur
    EXTERN  OSTCBHighRdy
    EXTERN  OSIntNesting
    EXTERN  OSIntExit
    EXTERN  OSTaskSwHook


    EXPORT  OS_CPU_SR_Save                                      ; Functions declared in this file
    EXPORT  OS_CPU_SR_Restore
    EXPORT  OSStartHighRdy
    EXPORT  OSCtxSw
    EXPORT  OSIntCtxSw
    EXPORT  OSPendSV

;********************************************************************************************************
;                                                EQUATES
;********************************************************************************************************

NVIC_INT_CTRL   EQU     0xE000ED04                              ; Interrupt control state register
;NVIC_SYSPRI2    EQU     0xE000ED20                              ; System priority register (2)
NVIC_SYSPRI2    EQU     0xE000ED22                              ; System priority register (yan).
;NVIC_PENDSV_PRI EQU     0x00   ; 0xFF00                         ; PendSV priority value (highest)
NVIC_PENDSV_PRI EQU           0xFF                              ; PendSV priority value (LOWEST yan).
NVIC_PENDSVSET  EQU     0x10000000                              ; Value to trigger PendSV exception

;********************************************************************************************************
;                                      CODE GENERATION DIRECTIVES
;********************************************************************************************************

    AREA |.text|, CODE, READONLY, ALIGN=2
    THUMB
    REQUIRE8
    PRESERVE8

;*********************************************************************************************************
;                                   CRITICAL SECTION METHOD 3 FUNCTIONS
;
; Description: Disable/Enable interrupts by preserving the state of interrupts.  Generally speaking you
;              would store the state of the interrupt disable flag in the local variable 'cpu_sr' and then
;              disable interrupts.  'cpu_sr' is allocated in all of uC/OS-II's functions that need to
;              disable interrupts.  You would restore the interrupt disable state by copying back 'cpu_sr'
;              into the CPU's status register.
;
; Prototypes :     OS_CPU_SR  OS_CPU_SR_Save(void);
;                  void       OS_CPU_SR_Restore(OS_CPU_SR cpu_sr);
;
;
; Note(s)    : 1) These functions are used in general like this:
;
;                 void Task (void *p_arg)
;                 {
;                 #if OS_CRITICAL_METHOD == 3          /* Allocate storage for CPU status register */
;                     OS_CPU_SR  cpu_sr;
;                 #endif
;
;                          :
;                          :
;                     OS_ENTER_CRITICAL();             /* cpu_sr = OS_CPU_SaveSR();                */
;                          :
;                          :
;                     OS_EXIT_CRITICAL();              /* OS_CPU_RestoreSR(cpu_sr);                */
;                          :
;                          :
;                 }
;
;              2) OS_CPU_SaveSR() is implemented as recommended by Atmel's application note:
;
;            (N/A for Cortex-M3)    "Disabling Interrupts at Processor Level"
;*********************************************************************************************************
;函数返回值存储在R0中
OS_CPU_SR_Save
    MRS     R0, PRIMASK     ;保存全局中断标志  ; Set prio int mask to mask all (except faults)
    CPSID   I	            ;关中断
    BX      LR
;通过R0传递参数
OS_CPU_SR_Restore
    MSR     PRIMASK, R0	    ;恢复全局中断标志
    BX      LR


;*********************************************************************************************************
;                                          START MULTITASKING
;                                       void OSStartHighRdy(void)
;
; Note(s) : 1) This function triggers a PendSV exception (essentially, causes a context switch) to cause
;              the first task to start.
;
;           2) OSStartHighRdy() MUST:
;              a) Setup PendSV exception priority to lowest;
;              b) Set initial PSP to 0, to tell context switcher this is first run;
;              c) Set OSRunning to TRUE;
;              d) Trigger PendSV exception;
;              e) Enable interrupts (tasks will run with interrupts enabled).
;*********************************************************************************************************

OSStartHighRdy
    LDR     R0, =NVIC_SYSPRI2                                   ; Set the PendSV exception priority
    LDR     R1, =NVIC_PENDSV_PRI
    STRB    R1, [R0]
	

    MOVS    R0, #0                                              ; Set the PSP to 0 for initial context switch call
    MSR     PSP, R0

    LDR     R0, __OS_Running                                    ; OSRunning = TRUE
    MOVS    R1, #1
    STRB    R1, [R0]

    LDR     R0, =NVIC_INT_CTRL                                  ; Trigger the PendSV exception (causes context switch)
    LDR     R1, =NVIC_PENDSVSET
    STR     R1, [R0]

    CPSIE   I   ;开总中断                                       ; Enable interrupts at processor level  

OSStartHang
    B       OSStartHang                                         ; Should never get here


;*********************************************************************************************************
;                               PERFORM A CONTEXT SWITCH (From task level)
;                                           void OSCtxSw(void)
;
; Note(s) : 1) OSCtxSw() is called when OS wants to perform a task context switch.  This function
;              triggers the PendSV exception which is where the real work is done.
;*********************************************************************************************************

OSCtxSw		   ;悬起PSV异常
   ;PUSH    {R0, R1} ;自己添加的代码
    LDR     R0, =NVIC_INT_CTRL                                  ; Trigger the PendSV exception (causes context switch)
    LDR     R1, =NVIC_PENDSVSET
    STR     R1, [R0]
	;POP     {R0, R1} ;自己添加的代码
    BX      LR

;*********************************************************************************************************
;                             PERFORM A CONTEXT SWITCH (From interrupt level)
;                                         void OSIntCtxSw(void)
;
; Notes:    1) OSIntCtxSw() is called by OSIntExit() when it determines a context switch is needed as
;              the result of an interrupt.  This function simply triggers a PendSV exception which will
;              be handled when there are no more interrupts active and interrupts are enabled.
;*********************************************************************************************************

OSIntCtxSw	   ;悬起PSV异常
    ;PUSH    {R0, R1} ;自己添加的代码
    LDR     R0, =NVIC_INT_CTRL                                  ; Trigger the PendSV exception (causes context switch)
    LDR     R1, =NVIC_PENDSVSET
    STR     R1, [R0]
	;POP     {R0, R1} ;自己添加的代码
    BX      LR

;*********************************************************************************************************
;                                         HANDLE PendSV EXCEPTION
;                                            void OSPendSV(void)
;
; Note(s) : 1) OSPendSV is used to cause a context switch.  This is a recommended method for performing
;              context switches with Cortex-M3.  This is because the Cortex-M3 auto-saves half of the
;              processor context on any exception, and restores same on return from exception.  So only
;              saving of R4-R11 is required and fixing up the stack pointers.  Using the PendSV exception
;              this way means that context saving and restoring is identical whether it is initiated from
;              a thread or occurs due to an interrupt or exception.
;
;           2) Pseudo-code is:
;              a) Get the process SP, if 0 then skip (goto d) the saving part (first context switch);
;              b) Save remaining regs r4-r11 on process stack;
;              c) Save the process SP in its TCB, OSTCBCur->OSTCBStkPtr = SP;
;              d) Call OSTaskSwHook();
;              e) Get current high priority, OSPrioCur = OSPrioHighRdy;
;              f) Get current ready thread TCB, OSTCBCur = OSTCBHighRdy;
;              g) Get new process SP from TCB, SP = OSTCBHighRdy->OSTCBStkPtr;
;              h) Restore R4-R11 from new process stack;
;              i) Perform exception return which will restore remaining context.
;
;           3) On entry into OSPendSV handler:
;              a) The following have been saved on the process stack (by processor):
;                 xPSR, PC, LR, R12, R0-R3
;              b) Processor mode is switched to Handler mode (from Thread mode)
;              c) Stack is Main stack (switched from Process stack)
;              d) OSTCBCur      points to the OS_TCB of the task to suspend
;                 OSTCBHighRdy  points to the OS_TCB of the task to resume
;
;           4) Since OSPendSV is set to lowest priority in the system (by OSStartHighRdy() above), we
;              know that it will only be run when no other exception or interrupt is active, and
;              therefore safe to assume that context being switched out was using the process stack (PSP).
;*********************************************************************************************************
;Cortex-M3进入异常服务例程时,自动压栈了R0-R3,R12,LR(R14,连接寄存器),PSR(程序状态寄存器)和PC(R15).并且在返回时自动弹出
OSPendSV
    ;MRS     R3, PRIMASK  ;             
    ;CPSID   I	  ;
    MRS     R0, PSP                                             ; PSP is process stack pointer
    CBZ     R0, OSPendSV_nosave                                 ; skip register save the first time

    SUBS    R0, R0, #0x20                                       ; save remaining regs r4-11 on process stack
    STM     R0, {R4-R11}

    LDR     R1, __OS_TCBCur                                     ; OSTCBCur->OSTCBStkPtr = SP;
    LDR     R1, [R1]
    STR     R0, [R1]                                            ; R0 is SP of process being switched out

                                                                ; at this point, entire context of process has been saved
OSPendSV_nosave
    PUSH    {R14}                                               ; need to save LR exc_return value
    LDR     R0, __OS_TaskSwHook                                 ; OSTaskSwHook();
    BLX     R0
    POP     {R14}

    LDR     R0, __OS_PrioCur                                    ; OSPrioCur = OSPrioHighRdy;
    LDR     R1, __OS_PrioHighRdy
    LDRB    R2, [R1]
    STRB    R2, [R0]

    LDR     R0, __OS_TCBCur                                     ; OSTCBCur  = OSTCBHighRdy;
    LDR     R1, __OS_TCBHighRdy
    LDR     R2, [R1]
    STR     R2, [R0]

    LDR     R0, [R2]                                            ; R0 is new process SP; SP = OSTCBHighRdy->OSTCBStkPtr;
    LDM     R0, {R4-R11}                                        ; restore r4-11 from new process stack
    ADDS    R0, R0, #0x20
    MSR     PSP, R0                                             ; load PSP with new process SP
    ORR     LR, LR, #0x04                                       ; ensure exception return uses process stack
	;MSR     PRIMASK, R3  ;
    BX      LR                                                  ; exception return will restore remaining context


;*********************************************************************************************************
;                                     POINTERS TO VARIABLES
;*********************************************************************************************************

__OS_TaskSwHook
        DCD     OSTaskSwHook

__OS_IntExit
        DCD     OSIntExit

__OS_IntNesting
        DCD     OSIntNesting

__OS_PrioCur
        DCD     OSPrioCur

__OS_PrioHighRdy
        DCD     OSPrioHighRdy

__OS_Running
        DCD     OSRunning

__OS_TCBCur
        DCD     OSTCBCur

__OS_TCBHighRdy
        DCD     OSTCBHighRdy

        END
