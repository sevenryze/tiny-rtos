/*
 *
 */

#include "OS.h"

static void  OS_SystemTickTask_Init();
static void  OS_IdleTask_Init();

/*
************************************************************************************************************************
*
************************************************************************************************************************
*/
void  OS_Init()
{
    OS_TYPE_StackUnit   *t_stack;
    OS_TYPE_StackSize   i;

    OS_Status = OS_ENUM_OsStatus_Stop;

    /* Initialize the exception stack and the stack grows from HIGH to LOW */
    OS_Exception_StackBase = &OS_Exception_Stack[OS_CONFIG_Exception_StackSize - 1];
    OS_Exception_StackSize = OS_CONFIG_Exception_StackSize;
    t_stack = OS_Exception_StackBase;
    for ( i = 0; i < OS_Exception_StackSize; i++ )
    {
        *t_stack = 0;
    	t_stack -- ;
    }

    OS_Counter_ISRNest          = 0;
    OS_Counter_ContextSwitch    = 0;

    /* Initialize the system function modes */
    OS_Task_Init();
    OS_ReadyList_Init();
    OS_Semaphore_Init();
    OS_Message_Init();

    /* Initialize the system tasks */
    OS_IdleTask_Init();
    OS_SystemTickTask_Init(  );
}

void  OS_Start()
{
    if( OS_Status == OS_ENUM_OsStatus_Stop )
    {
    	OS_Priority_Ready	=	OS_Priority_GetReady(); /* Find the highest priority */
        OS_Priority_Current	=	OS_Priority_Ready;
        OS_TDB_Ready	=	OS_Ready_List[OS_Priority_Ready].HeadTDB;
        OS_TDB_Current	=	OS_TDB_Ready;
        OS_Status		=	OS_ENUM_OsStatus_Run;
        OS_StartHighReady(); /* Call the first context switch */
    }
}

void  OS_Schedule()
{
    CPU_ATOMIC_REGISTER_ALLOCATE();

    if( OS_Counter_ISRNest > 0 ) /* ISRs still nested? */
    {
        return; /* Yes, only schedule when no nested ISRs */
    }

    CPU_ATOMIC_ENTER();
    OS_Priority_Ready   = OS_Priority_GetReady(); /* Find the highest priority ready */
    OS_TDB_Ready    = OS_Ready_List[OS_Priority_Ready].HeadTDB;

    if( OS_TDB_Ready == OS_TDB_Current ) /* Current task is still highest priority task? */
    {
        CPU_ATOMIC_EXIT(); /* Yes, no need to context switch */
        return;
    }

    OS_Counter_ContextSwitch++; /* Increment context switch counter */

    OS_TASK_SWITCH(); /* Perform a task level context switch */
    CPU_ATOMIC_EXIT();
}

/*
************************************************************************************************************************
*
************************************************************************************************************************
*/
void  OS_PendTool_Pend
( OS_STRUCT_PendTool   *p_PendTool )
{
    OS_TDB_Current->TaskStatus          = OS_ENUM_TaskStatus_Pend;

    OS_TDB_Current->TaskPend_Status     = OS_ENUM_PendStatus_OK;
    OS_TDB_Current->TaskPend_Type       = p_PendTool->Type;
    OS_TDB_Current->TaskPend_Tool       = p_PendTool;

    OS_ReadyList_Remove( OS_TDB_Current ); /* Block the task  */

    OS_PendList_Insert( OS_TDB_Current );
}

void  OS_PendTool_Post
( OS_STRUCT_TDB             *p_pend_TDB,
  void                      *p_void,
  OS_TYPE_MessageSize       p_message_size )
{
    switch( p_pend_TDB->TaskStatus )
    {
        case OS_ENUM_TaskStatus_Ready: /* Must not be this, or we have a error. Because if the task is ready, it would not be in the pend_list */
            break;

        case OS_ENUM_TaskStatus_Pend:
            p_pend_TDB->TaskMessage_RealMessage = p_void; /* Deposit message in task waiting */
            p_pend_TDB->TaskMessage_Size        = p_message_size;

            OS_PendList_Remove( p_pend_TDB ); /* Remove task from wait list */

            OS_ReadyList_Insert( p_pend_TDB ); /* Make task ready to run */

            p_pend_TDB->TaskStatus          = OS_ENUM_TaskStatus_Ready;
            p_pend_TDB->TaskPend_Status     = OS_ENUM_PendStatus_OK;
            p_pend_TDB->TaskPend_Type       = OS_ENUM_PendType_Nothing;
            break;

        default:
            break;
    }
}

/*
************************************************************************************************************************
*
************************************************************************************************************************
*/
static void  OS_IdleTask()
{
    CPU_ATOMIC_REGISTER_ALLOCATE();

    while (1)
    {
        CPU_ATOMIC_ENTER();
        OS_IdleTask_Counter ++ ;
        CPU_ATOMIC_EXIT();
    }
}

static void  OS_IdleTask_Init()
{
    OS_IdleTask_Counter = 0;

    OS_Task_Create( &OS_IdleTask_TDB,
                    OS_IdleTask,
                    (OS_DEF_RealMaxPriority - 1),
                    &OS_IdleTask_Stack[0],
                    OS_CONFIG_IdleTask_StackSize,
                    0 );
}

/*
************************************************************************************************************************
*
************************************************************************************************************************
*/
static void  OS_SystemTickTask()
{
    while(1)
    {
        OS_TaskSemaphore_Pend();

        OS_TickList_Update();
    }
}

static void  OS_SystemTickTask_Init()
{

    OS_SystemTick_BSP();

    OS_TickList_Init();

    OS_Task_Create( &OS_SystemTickTask_TDB,
                    OS_SystemTickTask,
                    OS_CONFIG_SystemTickTask_Priority,
                    &OS_SystemTickTask_Stack[0],
                    OS_CONFIG_SystemTickTask_StackSize,
                    0 );
}

void  OS_SystemTick_Handler()
{
    CPU_ATOMIC_REGISTER_ALLOCATE();

    if( OS_Status != OS_ENUM_OsStatus_Run )
    {
        return;
    }

    CPU_ATOMIC_ENTER();
    OS_Counter_ISRNest++;
    CPU_ATOMIC_EXIT();

    OS_TaskSemaphore_Post( &OS_SystemTickTask_TDB );

    CPU_ATOMIC_ENTER();
    OS_Counter_ISRNest--;
    CPU_ATOMIC_EXIT();

    OS_Schedule();
}

/*
************************************************************************************************************************
*
************************************************************************************************************************
*/



#ifdef  DEBUG_MODE
void  OS_Assert_failed( uint8* file, uint32 line )
{
    (void) file;
    (void) line;
    while(1);
}
#endif







