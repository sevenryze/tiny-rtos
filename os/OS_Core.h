/*
 * os_core.h
 *
 *  Created on: 2014年2月1日
 *      Author: Rare
 */

#ifndef OS_CORE_H_
#define OS_CORE_H_

#include "OS_Core_Config.h"
#include "OS_Core_Type.h"
#include "OS_Core_Schedule.h"

/*
************************************************************************************************************************
*                           Typedef region
************************************************************************************************************************
*/
typedef struct OS_STRUCT_TDB                OS_STRUCT_TDB;

typedef struct OS_STRUCT_List               OS_STRUCT_List;
typedef struct OS_STRUCT_ListLinker         OS_STRUCT_ListLinker;

typedef struct OS_STRUCT_PendTool           OS_STRUCT_PendTool;

typedef struct OS_STRUCT_Semaphore          OS_STRUCT_Semaphore;

typedef struct OS_STRUCT_RealMessage        OS_STRUCT_RealMessage;
typedef struct OS_STRUCT_MessagePool        OS_STRUCT_MessagePool;
typedef struct OS_STRUCT_Message            OS_STRUCT_Message;
typedef struct OS_STRUCT_MessageControl     OS_STRUCT_MessageControl;

/*
************************************************************************************************************************
*							System variables
************************************************************************************************************************
*/
OS_ENUM_OsStatus    OS_Status;

OS_STRUCT_TDB       *OS_TDB_Current;
OS_STRUCT_TDB       *OS_TDB_Ready;

OS_TYPE_Priority    OS_Priority_Current; /* Priority of current task */
OS_TYPE_Priority    OS_Priority_Ready; /* Priority of highest-ready-priority task */

OS_TYPE_ISRNest     OS_Counter_ISRNest;
OS_TYPE_Counter     OS_Counter_ContextSwitch;

OS_TYPE_CoreObjectQuantity      OS_Quantity_Task;
OS_TYPE_CoreObjectQuantity      OS_Quantity_SemaphoreTool;
OS_TYPE_CoreObjectQuantity      OS_Quantity_MessageTool;
OS_TYPE_CoreObjectQuantity      OS_Quantity_MemoryTool;

/*
************************************************************************************************************************
*							The stack used by exception( pointed by MSP )
************************************************************************************************************************
*/
/* The stack of exceptions, which includes system exceptions and external interrupts.
 * And must satisfy the worst situation( mean lots of ISR nesting ). */
OS_TYPE_StackUnit		OS_Exception_Stack[OS_CONFIG_Exception_StackSize];
OS_TYPE_StackUnit		*OS_Exception_StackBase;
OS_TYPE_StackSize		OS_Exception_StackSize;

/*
************************************************************************************************************************
*                           List Operate
************************************************************************************************************************
*/
struct  OS_STRUCT_List {
    OS_ENUM_ListType                Type;

    OS_STRUCT_TDB                   *HeadTDB;
    OS_STRUCT_TDB                   *TailTDB;

    OS_TYPE_CoreObjectQuantity      EntryNum_Current; /* Tasks are also Core Object in our system */
    OS_TYPE_CoreObjectQuantity      EntryNum_Peak;
};

struct  OS_STRUCT_ListLinker {
    OS_STRUCT_TDB   *PrevTDB;
    OS_STRUCT_TDB   *NextTDB;
};

/*
************************************************************************************************************************
*                           Ready list
************************************************************************************************************************
*/
OS_STRUCT_List  OS_Ready_List[OS_DEF_RealMaxPriority]; /* Use the priority of task to index */

/*
************************************************************************************************************************
*                           Pend tool
************************************************************************************************************************
*/
struct  OS_STRUCT_PendTool {
    OS_ENUM_CoreObject      Type;

    OS_STRUCT_List          PendList;
};

/*
************************************************************************************************************************
*                           Semaphore
************************************************************************************************************************
*/
struct  OS_STRUCT_Semaphore {
	OS_STRUCT_PendTool          PendTool;

	OS_TYPE_SemaphoreCounter    Semaphore_Counter;
};

/*
************************************************************************************************************************
*                           Message
************************************************************************************************************************
*/
struct  OS_STRUCT_RealMessage {
    OS_STRUCT_RealMessage   *NextMessage;

    void                    *RealMessage; /* The real message */
    OS_TYPE_MessageSize     MessageSize; /* Size of the message (in # bytes) */
};

struct  OS_STRUCT_MessagePool {
    OS_STRUCT_RealMessage      *NextMessage; /* Pointer to next message */

    OS_TYPE_MessagePoolSize    EntryNum_Free; /* Number of messages available from this pool */
    OS_TYPE_MessagePoolSize    EntryNum_Use; /* Number of messages used */
};

struct  OS_STRUCT_MessageControl {
    OS_STRUCT_RealMessage       *InputMessage; /* Pointer to next OS_STRUCT_RealMessage to be inserted in the queue */
    OS_STRUCT_RealMessage       *OutputMessage; /* Pointer to next OS_STRUCT_RealMessage to be extracted from the queue */

    OS_TYPE_MessageQueueSize    EntryNum_Limit; /* Maximum allowable number of entries in the queue */
	OS_TYPE_MessageQueueSize    EntryNum_Current; /* Current number of entries in the queue */
	OS_TYPE_MessageQueueSize    EntryNum_Peak; /* Peak number of entries in the queue */
};

struct  OS_STRUCT_Message {
    OS_STRUCT_PendTool          PendTool;

    OS_STRUCT_MessageControl    MessageControl;
};

OS_STRUCT_MessagePool      OS_MessagePool;
OS_STRUCT_RealMessage      OS_MessagePool_Space[OS_CONFIG_MessagePool_Size];

/*
************************************************************************************************************************
*                           SystemTick task
************************************************************************************************************************
*/
OS_TYPE_Tick        OS_SystemTick_StandardValue;

OS_STRUCT_TDB       OS_SystemTickTask_TDB;
OS_TYPE_StackUnit   OS_SystemTickTask_Stack[OS_CONFIG_SystemTickTask_StackSize];

/*
************************************************************************************************************************
*                           Idle task
************************************************************************************************************************
*/
OS_TYPE_Counter     OS_IdleTask_Counter;

OS_STRUCT_TDB       OS_IdleTask_TDB;
OS_TYPE_StackUnit   OS_IdleTask_Stack[OS_CONFIG_IdleTask_StackSize];

/*
************************************************************************************************************************
*                           Task Descript Block
************************************************************************************************************************
*/
struct OS_STRUCT_TDB {
    OS_TYPE_StackUnit                   *TaskSP;

    OS_TYPE_TaskEntry                   TaskEntry; /* Pointer that points to task's entry address */

    OS_TYPE_Priority                    TaskPriority;

    OS_ENUM_TaskStatus                  TaskStatus;

    OS_TYPE_StackUnit                   *TaskStack_Base; /* The bottom limit of stack */
    OS_TYPE_StackUnit                   *TaskStack_Limit; /* The top limit of stack */
    OS_TYPE_StackSize                   TaskStack_Size; /* The size of stack */
    OS_TYPE_StackSize                   TaskStack_UseSize;
    OS_TYPE_StackSize                   TaskStack_FreeSize;

    OS_STRUCT_PendTool                  *TaskPend_Tool;
    OS_ENUM_PendType                    TaskPend_Type;
    OS_ENUM_PendStatus                  TaskPend_Status;

    OS_TYPE_Tick                        TaskTick_ExceptTick;
    OS_TYPE_Tick                        TaskTick_RemainTick;
    OS_STRUCT_List                      *TaskTick_Spoke;

    OS_TYPE_SemaphoreCounter            TaskSemaphore_Counter;

    void                                *TaskMessage_RealMessage;
    OS_TYPE_MessageSize                 TaskMessage_Size;
    OS_STRUCT_MessageControl            TaskMessage_Control;

    OS_STRUCT_ListLinker                TaskListLinker[OS_DEF_ListTypeNum + 1];
};

/*
************************************************************************************************************************
*                           Debug mode
************************************************************************************************************************
*/
#ifdef  DEBUG_MODE
    #define OS_Assert(expr) ((expr) ? (void)0 : OS_Assert_failed((uint8 *)__FILE__, __LINE__))
    void  OS_Assert_failed( uint8* file, uint32 line );
#else
    #define OS_Assert(expr)
#endif


/*
************************************************************************************************************************
*                           Debug mode
************************************************************************************************************************
*/
void  OS_Init();
void  OS_Start();
void  OS_Schedule();

void  OS_List_Init
( OS_STRUCT_List    *t_List );
void  OS_List_Insert
( OS_STRUCT_TDB     *p_TDB,
  OS_STRUCT_List    *p_List );
void  OS_List_Remove
( OS_STRUCT_TDB     *p_TDB,
  OS_STRUCT_List    *p_List );

void  OS_PendList_Init
( OS_STRUCT_List    *t_List );
void  OS_PendList_Insert
( OS_STRUCT_TDB     *p_TDB );
void  OS_PendList_Remove
( OS_STRUCT_TDB     *p_TDB );

OS_TYPE_Priority  OS_Priority_GetReady();
void  OS_ReadyList_Init();
void  OS_ReadyList_Insert
( OS_STRUCT_TDB     *p_TDB );
void  OS_ReadyList_Remove
( OS_STRUCT_TDB     *p_TDB );

void  OS_TickList_Init();
void  OS_TickList_Update();
void  OS_TickList_Insert
( OS_STRUCT_TDB     *p_TDB );
void  OS_TickList_Remove
( OS_STRUCT_TDB     *p_TDB );

void  OS_PendTool_Pend
( OS_STRUCT_PendTool   *p_PendTool );
void  OS_PendTool_Post
( OS_STRUCT_TDB             *p_pend_TDB,
  void                      *p_void,
  OS_TYPE_MessageSize       p_message_size );


void  OS_SystemTick_Handler();



#endif /* OS_CORE_H_ */
