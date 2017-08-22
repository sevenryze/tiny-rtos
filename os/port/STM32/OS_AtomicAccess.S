
/*
******************************************************************************
*                   Public Functions
******************************************************************************
*/
.global  CPU_interrupt_disable
.global  CPU_interrupt_enable

.global  atomic_reg_save
.global  atomic_reg_restore

@********************************************************************************************************
@                                      CODE GENERATION DIRECTIVES
@********************************************************************************************************

.text
.align 2
.thumb
.syntax unified

@********************************************************************************************************
@                                    DISABLE and ENABLE INTERRUPTS
@
@ Description : Disable/Enable interrupts.
@
@ Prototypes  : void  CPU_interrupt_disable( void );
@               void  CPU_interrupt_enable ( void );
@********************************************************************************************************

.thumb_func
CPU_interrupt_disable:
        CPSID   I
        BX      LR

.thumb_func
CPU_interrupt_enable:
        CPSIE   I
        BX      LR

@********************************************************************************************************
@                                      ATOMIC OPERATE FUNCTIONS
@
@ Description :
@
@ Prototypes  : OS_TYPE_AtomicRegister  atomic_reg_save(void);
@               void	atomic_reg_restore( OS_TYPE_AtomicRegister atomic_reg );
@********************************************************************************************************

.thumb_func
atomic_reg_save:
        MRS     R0, PRIMASK     @ Set prio int mask to mask all (except faults)
        CPSID   I
        BX      LR

.thumb_func
atomic_reg_restore:
        MSR     PRIMASK, R0
        BX      LR

@********************************************************************************************************
@                                     FILE END
@********************************************************************************************************

.end

