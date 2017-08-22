
@********************************************************************************************************
@                                          PUBLIC FUNCTIONS
@********************************************************************************************************

.extern  OS_Priority_Current            @ External references.
.extern  OS_Priority_Ready
.extern  OS_TDB_Current
.extern  OS_TDB_Ready

.extern  OS_Exception_StackBase

@.extern  OS_task_switch_hook

.global  OS_StartHighReady        @ Functions declared in this file.
.global  OS_PendSV_handler

@********************************************************************************************************
@                                               EQUATES
@********************************************************************************************************

.equ NVIC_INT_CTRL,   0xE000ED04        @ Interrupt control state register.
.equ NVIC_SYSPRI14,   0xE000ED22        @ System priority register (priority 14).
.equ NVIC_PENDSV_PRI, 0xFF              @ PendSV priority value (lowest).
.equ NVIC_PENDSVSET,  0x10000000        @ Value to trigger PendSV exception.

@********************************************************************************************************
@                                     CODE GENERATION DIRECTIVES
@********************************************************************************************************

.text
.align 2
.thumb
.syntax unified

@********************************************************************************************************
@                                         START MULTITASKING
@                                      void OS_StartHighReady(void)
@
@ Note(s) : 1) This function triggers a PendSV exception (essentially, causes a context switch) to cause
@              the first task to start.
@
@           2) OS_StartHighReady() MUST:
@              a) Setup PendSV exception priority to lowest;
@              b) Set initial PSP to 0, to tell context switcher this is first run;
@              c) Set the main stack to OS_Exception_StackBase
@              d) Trigger PendSV exception;
@              e) Enable interrupts (tasks will run with interrupts enabled).
@********************************************************************************************************
.thumb_func
OS_StartHighReady:
    LDR     R0, =NVIC_SYSPRI14              @ Set the PendSV exception priority
    LDR     R1, =NVIC_PENDSV_PRI
    STRB    R1, [R0]

    MOVS    R0, #0                          @ Set the PSP to 0 for initial context switch call
    MSR     PSP, R0

    LDR     R0, =OS_Exception_StackBase    @ Initialize the MSP to the OS_Exception_StackBase
    LDR     R1, [R0]
    MSR     MSP, R1

    LDR     R0, =NVIC_INT_CTRL              @ Trigger the PendSV exception (causes context switch)
    LDR     R1, =NVIC_PENDSVSET
    STR     R1, [R0]

    CPSIE   I                               @ Enable interrupts at processor level

OS_Start_hang:
    B       OS_Start_hang                   @ Should never get here


@********************************************************************************************************
@                                       HANDLE PendSV EXCEPTION
@                                   void OS_PendSV_handler(void)
@
@ Note(s) : 1) PendSV is used to cause a context switch.  This is a recommended method for performing
@              context switches with Cortex-M3.  This is because the Cortex-M3 auto-saves half of the
@              processor context on any exception, and restores same on return from exception.  So only
@              saving of R4-R11 is required and fixing up the stack pointers.  Using the PendSV exception
@              this way means that context saving and restoring is identical whether it is initiated from
@              a thread or occurs due to an interrupt or exception.
@
@           2) Pseudo-code is:
@              a) Get the process SP, if 0 then skip the saving part (first context switch);
@              b) Save remaining regs r4-r11 on process stack;
@              c) Save the process SP in its TCB, OS_TDB_Current->task_sp = SP;
@              d) Call OS_task_switch_hook();
@              e) Get current high priority, OS_Priority_Current = OS_Priority_Ready;
@              f) Get current ready thread TCB, OS_TDB_Current = OS_TDB_Ready;
@              g) Get new process SP from TCB, SP = OS_TDB_Ready->task_sp;
@              h) Restore R4-R11 from new process stack;
@              i) Perform exception return which will restore remaining context.
@
@           3) On entry into PendSV handler:
@              a) The following have been saved on the process stack (by processor):
@                 xPSR, PC, LR, R12, R0-R3
@              b) Processor mode is switched to Handler mode (from Thread mode)
@              c) Stack is Main stack (this means MSP and switched from Process stack)
@              d) OS_TDB_Current	points to the OS_TCB of the task to suspend
@                 OS_TDB_Ready		points to the OS_TCB of the task to resume
@
@           4) Since PendSV is set to lowest priority in the system (by OS_StartHighReady() above), we
@              know that it will only be run when no other exception or interrupt is active, and
@              therefore safe to assume that context being switched out was using the process stack (PSP).
@********************************************************************************************************

.thumb_func
OS_PendSV_handler:
    CPSID   I                               @ Prevent interruption during context switch
    MRS     R0, PSP                         @ PSP is process stack pointer
    CBZ     R0, OS_PendSV_handler_nosave    @ Skip register save the first time

    SUBS    R0, R0, #0x20                   @ Save remaining regs r4-11 on process stack
    STM     R0, {R4-R11}

    LDR     R1, =OS_TDB_Current             @ OS_TDB_Current->stack_pointer = SP
    LDR     R1, [R1]
    STR     R0, [R1]                        @ R0 is SP of process being switched out

                                            @ At this point, entire context of process has been saved
OS_PendSV_handler_nosave:
    @PUSH    {R14}                          @ Save LR exc_return value
    @LDR     R0, =OS_task_switch_hook       @ OS_task_switch_hook();
    @BLX     R0
    @POP     {R14}

    LDR     R0, =OS_Priority_Current            @ OS_Priority_Current <- OS_Priority_Ready;
    LDR     R1, =OS_Priority_Ready
    LDRB    R2, [R1]
    STRB    R2, [R0]

    LDR     R0, =OS_TDB_Current             @ OS_TDB_Current <- OS_TDB_Ready;
    LDR     R1, =OS_TDB_Ready
    LDR     R2, [R1]
    STR     R2, [R0]

    LDR     R0, [R2]                        @ R0 is new process SP; we make SP = OS_TDB_Ready->task_sp;
    LDM     R0, {R4-R11}                    @ Restore r4-11 from new process stack
    ADDS    R0, R0, #0x20
    MSR     PSP, R0                         @ Load PSP with new process SP
    ORR     LR, LR, #0x04                   @ Ensure exception return uses process stack
    CPSIE   I
    BX      LR                              @ Exception return will restore remaining context

@********************************************************************************************************
@                                     FILE END
@********************************************************************************************************

.end
