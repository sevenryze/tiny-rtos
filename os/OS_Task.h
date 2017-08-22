/*
 * OS_Task.h
 *
 *  Created on: 2014年3月27日
 *      Author: OH
 */

#ifndef OS_TASK_H_
#define OS_TASK_H_

#include "OS.h"

void  OS_Task_Init();

void  OS_Task_Create
( OS_STRUCT_TDB                 *p_TDB,
  OS_TYPE_TaskEntry             p_TaskEntry,
  OS_TYPE_Priority              p_TaskPriority,
  OS_TYPE_StackUnit             *p_StackLimit,
  OS_TYPE_StackSize             p_StackSize,
  OS_TYPE_MessageQueueSize      p_MessageQueueSize );



#endif /* OS_TASK_H_ */
