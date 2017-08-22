#ifndef OS_CORE_SCHEDULE_H_
#define OS_CORE_SCHEDULE_H_

#include "OS_Core.h"

#ifndef OS_DEF_NVIC_INTERRUPT_CTRL  /* The address of NVIC interrupt control register */
#define OS_DEF_NVIC_INTERRUPT_CTRL      *((volatile uint32 *)0xE000ED04)
#endif

#ifndef OS_DEF_NVIC_PENDSV_SET
#define OS_DEF_NVIC_PENDSV_SET      0x10000000
#endif

#define OS_TASK_SWITCH()    OS_DEF_NVIC_INTERRUPT_CTRL = OS_DEF_NVIC_PENDSV_SET

/*
************************************************************************************************************************
*                                      	ATOMIC ENTER
************************************************************************************************************************
*/
OS_TYPE_AtomicRegister  atomic_reg_save( void );
void  atomic_reg_restore( OS_TYPE_AtomicRegister atomic_reg );

#define CPU_ATOMIC_REGISTER_ALLOCATE()      OS_TYPE_AtomicRegister  atomic_sr = 0;

#define CPU_ATOMIC_ENTER()  do { CPU_INTERRUPT_DISABLE(); } while (0) /* Disable   interrupts */
#define CPU_ATOMIC_EXIT()   do { CPU_INTERRUPT_ENABLE();  } while (0) /* Re-enable interrupts */

#define CPU_INTERRUPT_DISABLE()     do { atomic_sr = atomic_reg_save(); } while (0) /* Save    CPU state word & disable interrupts */
#define CPU_INTERRUPT_ENABLE()      do { atomic_reg_restore(atomic_sr); } while (0) /* Restore CPU state word */





#endif /* end of os_schedule.h */
