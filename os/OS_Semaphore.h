/*
 * OS_Semaphore.h
 *
 *  Created on: 2014年3月27日
 *      Author: OH
 */

#ifndef OS_SEMAPHORE_H_
#define OS_SEMAPHORE_H_

#include "OS.h"

void  OS_Semaphore_Init();

void  OS_Semaphore_Create
( OS_STRUCT_Semaphore       *p_Semaphore,
  OS_TYPE_SemaphoreCounter  p_SemaphoreCounter );

OS_TYPE_SemaphoreCounter  OS_Semaphore_Pend
( OS_STRUCT_Semaphore   *p_Semaphore );

OS_TYPE_SemaphoreCounter  OS_Semaphore_Post
( OS_STRUCT_Semaphore   *p_Semaphore );

OS_TYPE_SemaphoreCounter  OS_TaskSemaphore_Pend();

OS_TYPE_SemaphoreCounter  OS_TaskSemaphore_Post
( OS_STRUCT_TDB  *p_TDB );


#endif /* OS_SEMAPHORE_H_ */
