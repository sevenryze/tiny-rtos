/*
 * os_config.h
 *
 *  Created on: 2014年2月1日
 *      Author: Rare
 */

#ifndef OS_CONFIG_H_
#define OS_CONFIG_H_

#define     DEBUG_MODE

/*
************************************************************************************************************************
*                   Usually change these configure
************************************************************************************************************************
*/
/* Define the max number of priority that can be use */
#define     OS_CONFIG_MaxPriority       20


/*
************************************************************************************************************************
*                   Sometimes change these configure
************************************************************************************************************************
*/
#define		OS_CONFIG_Exception_StackSize               256

#define		OS_CONFIG_IdleTask_StackSize                128

#define     OS_CONFIG_SystemTickTask_Priority           1
#define     OS_CONFIG_SystemTickTask_StackSize          128
#define     OS_CONFIG_SystemTick_HZ                     10
#define     OS_CONFIG_TickWheelSize                     3

#define     OS_CONFIG_MessagePool_Size                  20


/*
************************************************************************************************************************
*                   Should better not change these configure
************************************************************************************************************************
*/
/* We need the Priority 0 and OS_CONFIG_MaxPriority to represent the interrupt-handle-task and the idle-task */
#define     OS_DEF_RealMaxPriority      (OS_CONFIG_MaxPriority + 2)

#define     OS_DEF_ListTypeNum          3

#endif /* OS_CONFIG_H_ */
