/*
 *
 */

#include "OS_Semaphore.h"

void  OS_Semaphore_Init()
{
	OS_Quantity_SemaphoreTool = 0;
}

void  OS_Semaphore_Create
( OS_STRUCT_Semaphore       *p_Semaphore,
  OS_TYPE_SemaphoreCounter  p_SemaphoreCounter )
{
    OS_Assert( p_Semaphore != 0 );
    OS_Assert( p_SemaphoreCounter >= 0 );

	CPU_ATOMIC_REGISTER_ALLOCATE();

	if( OS_Counter_ISRNest > 0 ) /* Not allowed to be called from an ISR */
    {
        return;
    }

    CPU_ATOMIC_ENTER();
    p_Semaphore->PendTool.Type      = OS_ENUM_PendType_Semaphore;
    p_Semaphore->Semaphore_Counter  = p_SemaphoreCounter; /* Set semaphore value */

    OS_PendList_Init( &p_Semaphore->PendTool.PendList ); /* Initialize the pend list of the semaphore */

    OS_Quantity_SemaphoreTool++;
    CPU_ATOMIC_EXIT();
}

OS_TYPE_SemaphoreCounter  OS_Semaphore_Pend
( OS_STRUCT_Semaphore   *p_Semaphore )
{
    OS_Assert( p_Semaphore != 0 );

	CPU_ATOMIC_REGISTER_ALLOCATE();

    if( OS_Counter_ISRNest > 0 ) /* Not allowed to call from an ISR */
    {
        return( 0 );
    }

    CPU_ATOMIC_ENTER();
    if( p_Semaphore->Semaphore_Counter > 0 ) /* Resource available */
    {
        p_Semaphore->Semaphore_Counter--; /* Yes, task will be running again */
        CPU_ATOMIC_EXIT();

        return( p_Semaphore->Semaphore_Counter );
    }

    OS_PendTool_Pend( (OS_STRUCT_PendTool *)((void *)p_Semaphore) ); /* Resource unavailable, so block task */
    CPU_ATOMIC_EXIT();

    OS_Schedule(); /* Find the next highest priority task that is ready to run and switch to it */

    CPU_ATOMIC_ENTER(); /* We get to this just like that we got the semaphore */
    switch( OS_TDB_Current->TaskPend_Status )
    {
        case OS_ENUM_PendStatus_OK: /* We got the semaphore */
             break;
        default:
             CPU_ATOMIC_EXIT();
             return (0);
    }
    CPU_ATOMIC_EXIT();

    return( p_Semaphore->Semaphore_Counter );
}

OS_TYPE_SemaphoreCounter  OS_Semaphore_Post
( OS_STRUCT_Semaphore   *p_Semaphore )
{
    OS_Assert( p_Semaphore != 0 );

	CPU_ATOMIC_REGISTER_ALLOCATE();

    CPU_ATOMIC_ENTER();
    if( p_Semaphore->PendTool.PendList.EntryNum_Current == 0 ) /* No task waiting on semaphore */
    {
        switch( sizeof(OS_TYPE_SemaphoreCounter) )
        {
            case 1u:
                 if (p_Semaphore->Semaphore_Counter == 255u) {
                     CPU_ATOMIC_EXIT();
                     return (0);
                 }
                 break;

            case 2u:
                 if (p_Semaphore->Semaphore_Counter == 65535u) {
                	 CPU_ATOMIC_EXIT();
                     return (0);
                 }
                 break;

            case 4u:
                 if (p_Semaphore->Semaphore_Counter == 4294967295u) {
                	 CPU_ATOMIC_EXIT();
                     return (0);
                 }
                 break;

            default:
                 break;
        }
        p_Semaphore->Semaphore_Counter++;
        CPU_ATOMIC_EXIT();

        return( p_Semaphore->Semaphore_Counter );
    }

    OS_PendTool_Post( p_Semaphore->PendTool.PendList.HeadTDB, 0, 0 );
    CPU_ATOMIC_EXIT();

    OS_Schedule();

    return( p_Semaphore->Semaphore_Counter );
}

OS_TYPE_SemaphoreCounter  OS_TaskSemaphore_Pend()
{
	CPU_ATOMIC_REGISTER_ALLOCATE();

    if( OS_Counter_ISRNest > 0 ) /* Not allowed to call from an ISR */
    {
        return ( 0 );
    }

    CPU_ATOMIC_ENTER();
    if( OS_TDB_Current->TaskSemaphore_Counter > 0 ) /* See if task already been signaled */
    {
    	OS_TDB_Current->TaskSemaphore_Counter--;
        CPU_ATOMIC_EXIT();

        return ( OS_TDB_Current->TaskSemaphore_Counter );
    }

    OS_ReadyList_Remove( OS_TDB_Current ); /* Block task pending on Signal */

    OS_TDB_Current->TaskStatus      = OS_ENUM_TaskStatus_Pend;
    OS_TDB_Current->TaskPend_Type   = OS_ENUM_PendType_TaskSemaphore;
    OS_TDB_Current->TaskPend_Status = OS_ENUM_PendStatus_OK;
    CPU_ATOMIC_EXIT();

    OS_Schedule(); /* Find next highest priority task ready to run */

    return ( OS_TDB_Current->TaskSemaphore_Counter );
}

OS_TYPE_SemaphoreCounter  OS_TaskSemaphore_Post
( OS_STRUCT_TDB  *p_TDB )
{
    OS_Assert( p_TDB != 0 );

	CPU_ATOMIC_REGISTER_ALLOCATE();

    CPU_ATOMIC_ENTER();
    switch( p_TDB->TaskStatus )
    {
        case OS_ENUM_TaskStatus_Ready: /* Task that is signed is not pending on anything */
             switch( sizeof(OS_TYPE_SemaphoreCounter) )
             {
                 case 1u:
                      if( p_TDB->TaskSemaphore_Counter == 255u )
                      {
                    	  CPU_ATOMIC_EXIT();
                          return (0);
                      }
                      break;
                 case 2u:
                      if( p_TDB->TaskSemaphore_Counter == 65535u )
                      {
                    	  CPU_ATOMIC_EXIT();
                          return (0);
                      }
                      break;
                 case 4u:
                      if( p_TDB->TaskSemaphore_Counter == 4294967295u )
                      {
                    	  CPU_ATOMIC_EXIT();
                          return (0);
                      }
                      break;
                 default:
                      break;
             }
             p_TDB->TaskSemaphore_Counter++;
             CPU_ATOMIC_EXIT();
             break;

        case OS_ENUM_TaskStatus_Pend:
            OS_ReadyList_Insert( p_TDB ); /* Make task ready to run */
        	p_TDB->TaskStatus           = OS_ENUM_TaskStatus_Ready;
        	p_TDB->TaskPend_Status      = OS_ENUM_PendStatus_OK;
        	p_TDB->TaskPend_Type        = OS_ENUM_PendType_Nothing;
        	CPU_ATOMIC_EXIT();
        	OS_Schedule();
            break;

        default:
             CPU_ATOMIC_EXIT();
             return 0;

             break;
    }
    return ( p_TDB->TaskSemaphore_Counter );
}

