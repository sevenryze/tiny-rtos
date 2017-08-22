
#include "OS_Message.h"


typedef enum {
    OS_MessageInternal_OK = 0,

    OS_MessageInternal_MessageControlEmpty,
    OS_MessageInternal_MessageControlFull,

    OS_MessageInternal_MessagePoolEmpty,
    OS_MessageInternal_MessagePoolFull,
} OS_MessageInternal;

static OS_MessageInternal  OS_MessageState;

/*
************************************************************************************************************************
*
************************************************************************************************************************
*/
static void  OS_MessagePool_Create
( OS_STRUCT_RealMessage         *p_MessagePool_Base,
  OS_TYPE_MessageQueueSize      p_MessagePool_Size )
{
    OS_STRUCT_RealMessage      *t_msg1;
    OS_STRUCT_RealMessage      *t_msg2;
    OS_TYPE_MessageQueueSize   i;

    t_msg1 = p_MessagePool_Base;
    t_msg2 = p_MessagePool_Base + 1;

    for( i = 0; i < (p_MessagePool_Size - 1); i++ )
    {
        t_msg1->NextMessage = t_msg2;
        t_msg1->RealMessage = (void *)0;
        t_msg1->MessageSize = 0;
        t_msg1++;
        t_msg2++;
    }
    t_msg1->NextMessage = 0; /* Now, t_msg1 point to the Last OS_STRUCT_RealMessage */
    t_msg1->RealMessage = (void *)0;
    t_msg1->MessageSize = 0;
}

static void  OS_MessagePool_Init()
{
    OS_STRUCT_RealMessage   *t_MessagePool_Base;

    t_MessagePool_Base = &OS_MessagePool_Space[0];

    OS_MessagePool_Create( t_MessagePool_Base, OS_CONFIG_MessagePool_Size );

    OS_MessagePool.NextMessage      = t_MessagePool_Base;
    OS_MessagePool.EntryNum_Free    = OS_CONFIG_MessagePool_Size;
    OS_MessagePool.EntryNum_Use     = 0;
}

/*
************************************************************************************************************************
*
************************************************************************************************************************
*/
void  OS_MessageControl_Init
( OS_STRUCT_MessageControl      *p_MessageControl,
  OS_TYPE_MessageQueueSize      p_MessageQueueSize )
{
    p_MessageControl->EntryNum_Limit       = p_MessageQueueSize;
    p_MessageControl->EntryNum_Current     = 0;
    p_MessageControl->EntryNum_Peak        = 0;

    p_MessageControl->InputMessage         = 0;
    p_MessageControl->OutputMessage        = 0;
}

static void  *OS_MessageControl_Get
( OS_STRUCT_MessageControl      *p_MessageControl,
  OS_TYPE_MessageSize           *p_ReturnMessageSize )
{
    OS_STRUCT_RealMessage  *t_message;
    void    *t_void;

    if( p_MessageControl->EntryNum_Current == 0 )
    {
       *p_ReturnMessageSize = 0;
       OS_MessageState = OS_MessageInternal_MessageControlEmpty;

       return (void *)0;
    }

    t_message = p_MessageControl->OutputMessage;

    t_void                  = t_message->RealMessage;
    *p_ReturnMessageSize    = t_message->MessageSize;

    p_MessageControl->OutputMessage = t_message->NextMessage;
    if( p_MessageControl->OutputMessage == 0 )
    {
        p_MessageControl->InputMessage = 0;
        p_MessageControl->EntryNum_Current = 0;
    }
    else
    {
        p_MessageControl->EntryNum_Current--;
    }

    t_message->NextMessage = OS_MessagePool.NextMessage;
    OS_MessagePool.NextMessage = t_message;
    OS_MessagePool.EntryNum_Free++;
    OS_MessagePool.EntryNum_Use--;

    OS_MessageState = OS_MessageInternal_OK;

    return( t_void );
}

static void  OS_MessageControl_Put
( OS_STRUCT_MessageControl      *p_MessageControl,
  void                          *p_void,
  OS_TYPE_MessageSize           p_ReturnMessageSize )
{
    OS_STRUCT_RealMessage  *t_message;
    OS_STRUCT_RealMessage  *p_msg_in;

    if( p_MessageControl->EntryNum_Current >= p_MessageControl->EntryNum_Limit )
    {
        OS_MessageState = OS_MessageInternal_MessageControlFull;
        return;
    }

    if( OS_MessagePool.EntryNum_Free == 0 )
    {
        OS_MessageState = OS_MessageInternal_MessagePoolFull;
        return;
    }

    t_message = OS_MessagePool.NextMessage;
    OS_MessagePool.NextMessage = t_message->NextMessage;
    OS_MessagePool.EntryNum_Free--;
    OS_MessagePool.EntryNum_Use++;

    if( p_MessageControl->EntryNum_Current == 0 ) /* Is this first message placed in the queue? */
    {
        p_MessageControl->InputMessage  = t_message;
        p_MessageControl->OutputMessage = t_message;
        p_MessageControl->EntryNum_Current = 1;
    }
    else
    {
        p_msg_in = p_MessageControl->InputMessage; /* FIFO */
        p_msg_in->NextMessage   = t_message;
        t_message->NextMessage  = 0;
        p_MessageControl->InputMessage = t_message;

        p_MessageControl->EntryNum_Current++;
    }

    if( p_MessageControl->EntryNum_Current > p_MessageControl->EntryNum_Peak )
    {
        p_MessageControl->EntryNum_Peak = p_MessageControl->EntryNum_Current;
    }

    OS_MessageState = OS_MessageInternal_OK;

    t_message->RealMessage = p_void; /* Deposit message in the message queue entry */
    t_message->MessageSize = p_ReturnMessageSize;
}

/*
************************************************************************************************************************
*
************************************************************************************************************************
*/
void  OS_Message_Init()
{
    OS_MessageState = OS_MessageInternal_OK;

    OS_MessagePool_Init();

    OS_Quantity_MessageTool = 0;
}

void  OS_Message_Create
( OS_STRUCT_Message             *p_Message,
  OS_TYPE_MessageQueueSize       p_MessageQueueSzie )
{
    OS_Assert( p_Message != 0 );
    OS_Assert( p_MessageQueueSzie >= 0 );

    CPU_ATOMIC_REGISTER_ALLOCATE();

    if( OS_Counter_ISRNest > 0 ) /* Not allowed to be called from an ISR */
    {
        return;
    }

    CPU_ATOMIC_ENTER();
    p_Message->PendTool.Type = OS_ENUM_CoreObject_Message; /* Mark the data structure as a message queue */

    OS_MessageControl_Init( &p_Message->MessageControl, p_MessageQueueSzie );

    OS_PendList_Init( &p_Message->PendTool.PendList ); /* Initialize the waiting list */

    OS_Quantity_MessageTool++; /* One more queue created */
    CPU_ATOMIC_EXIT();
}

void  *OS_Message_Pend
( OS_STRUCT_Message         *p_Message,
  OS_TYPE_MessageSize       *p_ReturnMessageSize )
{
    CPU_ATOMIC_REGISTER_ALLOCATE();

    void    *t_void;

    if( OS_Counter_ISRNest > 0 ) /* Not allowed to be called from an ISR */
    {
        return (void *)0;
    }

    CPU_ATOMIC_ENTER();

    t_void = OS_MessageControl_Get( &p_Message->MessageControl, p_ReturnMessageSize );

    if( OS_MessageState == OS_MessageInternal_OK )
    {
        CPU_ATOMIC_EXIT();
        return t_void; /* Yes, Return message received */
    }

    OS_PendTool_Pend( (OS_STRUCT_PendTool *)((void *)p_Message) );
    CPU_ATOMIC_EXIT();

    OS_Schedule();

    CPU_ATOMIC_ENTER();
    switch( OS_TDB_Current->TaskPend_Status )
    {
        case OS_ENUM_PendStatus_OK:
            t_void = OS_TDB_Current->TaskMessage_RealMessage;
            *p_ReturnMessageSize = OS_TDB_Current->TaskMessage_Size;

            break;

        default:
            t_void = (void *)0;
            *p_ReturnMessageSize = 0;
            while(1)
                ;
            break;
    }
    CPU_ATOMIC_EXIT();

    return( t_void );
}

void  OS_Message_Post
( OS_STRUCT_Message         *p_Message,
  void                      *p_void,
  OS_TYPE_MessageSize       p_ReturnMessageSize )
{
    CPU_ATOMIC_REGISTER_ALLOCATE();

    CPU_ATOMIC_ENTER();
    if( p_Message->PendTool.PendList.EntryNum_Current == 0 ) /* Any task waiting on message queue? */
    {
        OS_MessageControl_Put( &p_Message->MessageControl, p_void, p_ReturnMessageSize );
        CPU_ATOMIC_EXIT();
        return;
    }

    OS_PendTool_Post( p_Message->PendTool.PendList.HeadTDB, p_void, p_ReturnMessageSize );
    CPU_ATOMIC_EXIT();

    OS_Schedule();
}

/*
************************************************************************************************************************
*
************************************************************************************************************************
*/
void  *OS_TaskMessage_Pend
( OS_TYPE_MessageSize      *p_ReturnMessageSize )
{
    CPU_ATOMIC_REGISTER_ALLOCATE();

    void    *t_void;

    if( OS_Counter_ISRNest > 0 ) /* Not allowed to be called from an ISR */
    {
        return (void *)0;
    }

    CPU_ATOMIC_ENTER();
    t_void = OS_MessageControl_Get( &OS_TDB_Current->TaskMessage_Control, p_ReturnMessageSize );

    if( OS_MessageState == OS_MessageInternal_OK )
    {
        CPU_ATOMIC_EXIT();
        return t_void; /* Yes, Return message received */
    }

    /* Block task pending on Signal */
    OS_ReadyList_Remove( OS_TDB_Current );
    OS_TDB_Current->TaskStatus      = OS_ENUM_TaskStatus_Pend;
    OS_TDB_Current->TaskPend_Type   = OS_ENUM_PendType_TaskMessage;
    OS_TDB_Current->TaskPend_Status = OS_ENUM_PendStatus_OK;
    CPU_ATOMIC_EXIT();

    OS_Schedule();

    CPU_ATOMIC_ENTER();
    switch( OS_TDB_Current->TaskPend_Status )
    {
        case OS_ENUM_PendStatus_OK:
            t_void = OS_TDB_Current->TaskMessage_RealMessage;
            *p_ReturnMessageSize = OS_TDB_Current->TaskMessage_Size;

            break;

        default:
            t_void = (void *)0;
            *p_ReturnMessageSize = 0;
            while(1)
                ;
            break;
    }
    CPU_ATOMIC_EXIT();

    return( t_void );
}

void  OS_TaskMessage_Post
( OS_STRUCT_TDB             *p_TDB,
  void                      *p_void,
  OS_TYPE_MessageSize       p_ReturnMessageSize )
{
    CPU_ATOMIC_REGISTER_ALLOCATE();

    CPU_ATOMIC_ENTER();
    if( p_TDB->TaskPend_Type != OS_ENUM_PendType_TaskMessage ) /* Task is not waiting on message queue */
    {
        OS_MessageControl_Put( &p_TDB->TaskMessage_Control, p_void, p_ReturnMessageSize );
        CPU_ATOMIC_EXIT();
        return;
    }

    p_TDB->TaskMessage_RealMessage  = p_void;
    p_TDB->TaskMessage_Size         = p_ReturnMessageSize;

    p_TDB->TaskStatus       = OS_ENUM_TaskStatus_Ready;
    p_TDB->TaskPend_Status  = OS_ENUM_PendStatus_OK;
    p_TDB->TaskPend_Type    = OS_ENUM_PendType_Nothing;
    OS_ReadyList_Insert( p_TDB ); /* Make task ready to run */
    CPU_ATOMIC_EXIT();

    OS_Schedule();
}



