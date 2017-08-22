/*
 *
 */

#include "OS_Task.h"

void  OS_Task_Init()
{
    OS_Quantity_Task = 0;
}

void  OS_Task_Create
( OS_STRUCT_TDB                 *p_TDB,
  OS_TYPE_TaskEntry             p_TaskEntry,
  OS_TYPE_Priority              p_TaskPriority,
  OS_TYPE_StackUnit             *p_StackLimit,
  OS_TYPE_StackSize             p_StackSize,
  OS_TYPE_MessageQueueSize      p_MessageQueueSize )
{
    /* Check the parameters */
	OS_Assert( p_TDB != 0 );
	OS_Assert( p_TaskEntry != 0 );
    if( p_TaskPriority == (OS_DEF_RealMaxPriority - 1) )
    {
        OS_Assert( p_TDB == &OS_IdleTask_TDB );
    }
    else if( p_TaskPriority == 0 )
    {
        OS_Assert(1);
    }
    else
    {
        OS_Assert( p_TaskPriority != 0 && p_TaskPriority <= OS_CONFIG_MaxPriority );
    }
    OS_Assert( p_StackLimit != 0 );
    OS_Assert( p_StackSize >= 0 );
    OS_Assert( p_MessageQueueSize >= 0 );

	CPU_ATOMIC_REGISTER_ALLOCATE();

	uint8   i;

	OS_TYPE_StackUnit		*t_SP;
	OS_TYPE_StackUnit     	*t_StackBase;

	/* Initialize the contents of task's TDB */
	p_TDB->TaskStatus       = OS_ENUM_TaskStatus_Ready;

	p_TDB->TaskPend_Tool    = 0;
	p_TDB->TaskPend_Type    = OS_ENUM_PendType_Nothing;
	p_TDB->TaskPend_Status  = OS_ENUM_PendStatus_OK;

	p_TDB->TaskTick_ExceptTick  = 0;
	p_TDB->TaskTick_RemainTick  = 0;
	p_TDB->TaskTick_Spoke       = 0;

	p_TDB->TaskSemaphore_Counter = 0;

	p_TDB->TaskMessage_RealMessage  = 0;
	p_TDB->TaskMessage_Size         = 0;

	for( i = 0; i < OS_DEF_ListTypeNum; i++ )
	{
	    p_TDB->TaskListLinker[i].PrevTDB = 0;
	    p_TDB->TaskListLinker[i].NextTDB = 0;
	}

    /* Initialize the stack frame of the task, which we used for the first content switch */
	t_StackBase = p_StackLimit + p_StackSize - 1;
    t_SP = OS_TaskStack_Init( p_TaskEntry, t_StackBase, p_StackSize );

    /* Ensure the contents of the task's TDB field */
    p_TDB->TaskEntry           = p_TaskEntry; /* Save task entry point address */

    p_TDB->TaskPriority        = p_TaskPriority; /* Save the task's task_priority */

    p_TDB->TaskSP              = t_SP; /* Save the new top-of-stack pointer */
    p_TDB->TaskStack_Base      = t_StackBase; /* Save pointer to the base address of the stack */
    p_TDB->TaskStack_Limit     = p_StackLimit; /* Save the stack limit pointer */
    p_TDB->TaskStack_Size      = p_StackSize; /* Save the stack size */

    OS_MessageControl_Init( &p_TDB->TaskMessage_Control, p_MessageQueueSize );

    /* Prepare to schedule the task */
    CPU_ATOMIC_ENTER();
    OS_ReadyList_Insert( p_TDB );

    OS_Quantity_Task++ ;

    if( OS_Status != OS_ENUM_OsStatus_Run )
    {
    	CPU_ATOMIC_EXIT(); /* Return if multitasking has not started */
        return;
    }
    CPU_ATOMIC_EXIT();

    OS_Schedule();
}
