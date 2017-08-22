/*
 * os_type.h
 *
 *  Created on: 2014年1月20日
 *      Author: Rare
 */

#ifndef OS_TYPE_H_
#define OS_TYPE_H_

/*
************************************************************************************************************************
*
************************************************************************************************************************
*/
typedef	unsigned char	uint8;
typedef	signed char		int8;

typedef	unsigned short	uint16;
typedef	signed short	int16;

typedef	unsigned int	uint32;
typedef	signed int		int32;

typedef	unsigned long	uint64;
typedef	signed long		int64;

/*
************************************************************************************************************************
*
************************************************************************************************************************
*/
typedef enum {
	OS_ENUM_OsStatus_Run = 0,
	OS_ENUM_OsStatus_Stop
} OS_ENUM_OsStatus;

typedef enum {
	OS_ENUM_CoreObject_Nothing = 0,
	OS_ENUM_CoreObject_Task,
	OS_ENUM_CoreObject_Semaphore,
	OS_ENUM_CoreObject_Message,
	OS_ENUM_CoreObject_Memory,
} OS_ENUM_CoreObject;

typedef enum {
	OS_ENUM_TaskStatus_Ready = 0,
	OS_ENUM_TaskStatus_Pend,
	OS_ENUM_TaskStatus_Delay,
	OS_ENUM_TaskStatus_Delete
} OS_ENUM_TaskStatus;

typedef enum {
    OS_ENUM_PendType_Nothing    =   0,
    OS_ENUM_PendType_Semaphore,
    OS_ENUM_PendType_TaskSemaphore,
    OS_ENUM_PendType_Message,
    OS_ENUM_PendType_TaskMessage,
} OS_ENUM_PendType;

typedef enum {
	OS_ENUM_PendStatus_OK   =   0,
	OS_ENUM_PendStatus_Delete
} OS_ENUM_PendStatus;

typedef enum {
    OS_ENUM_ListType_ReadyList = 0,
    OS_ENUM_ListType_PendList,
    OS_ENUM_ListType_TickList,
} OS_ENUM_ListType;


/*
************************************************************************************************************************
*
************************************************************************************************************************
*/
#if OS_DEF_RealMaxPriority <= 256 /* 2^8 */
	typedef uint8 	OS_TYPE_Priority;
#elif (OS_DEF_RealMaxPriority > 256 && OS_DEF_RealMaxPriority <= 65536) /* 2^16 */
	typedef uint16 	OS_TYPE_Priority;
#elif (OS_DEF_RealMaxPriority > 65536 && OS_DEF_RealMaxPriority <= 4294967296) /* 2^32 */
	typedef uint32 	OS_TYPE_Priority;
#endif

typedef	uint32  OS_TYPE_AtomicRegister;

typedef	void    (*OS_TYPE_TaskEntry)(void *p_void );

typedef void    (*OS_TYPE_InterruptEntry)(void);

typedef	uint32  OS_TYPE_StackUnit;
typedef	uint16  OS_TYPE_StackSize;

typedef uint32  OS_TYPE_CPUDataWidth;

typedef uint32  OS_TYPE_Counter;

typedef uint8   OS_TYPE_ISRNest;

typedef uint8   OS_TYPE_CoreObjectQuantity;

typedef uint32  OS_TYPE_Tick;

typedef uint16  OS_TYPE_SemaphoreCounter; // the semaphore counter means the resource quantity

typedef uint8   OS_TYPE_MessagePoolSize;
typedef uint8   OS_TYPE_MessageQueueSize;
typedef uint32	OS_TYPE_MessageSize; // Message size in Byte that we recieved



#endif /* OS_TYPE_H_ */
