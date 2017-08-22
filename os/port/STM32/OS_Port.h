/*
 * OS_Port.h
 *
 *  Created on: 2014年3月27日
 *      Author: OH
 */

#ifndef OS_PORT_H_
#define OS_PORT_H_

#include "OS.h"


OS_TYPE_Priority  CPU_count_lead_zeros( OS_TYPE_CPUDataWidth  val );

void  OS_StartHighReady();

OS_TYPE_StackUnit  *OS_TaskStack_Init
( OS_TYPE_TaskEntry    p_TaskEntry,
  OS_TYPE_StackUnit    *p_StackBase,
  OS_TYPE_StackSize    p_StackSize );

void  OS_SystemTick_BSP();


#endif /* OS_PORT_H_ */
