/*
 * OS_Message.h
 *
 *  Created on: 2014年3月27日
 *      Author: OH
 */

#ifndef OS_MESSAGE_H_
#define OS_MESSAGE_H_

#include "OS.h"

void  OS_Message_Init();

void  OS_MessageControl_Init
( OS_STRUCT_MessageControl      *p_MessageControl,
  OS_TYPE_MessageQueueSize      p_MessageQueueSize );

void  OS_Message_Create
( OS_STRUCT_Message             *p_Message,
  OS_TYPE_MessageQueueSize       p_MessageQueueSzie );

void  *OS_Message_Pend
( OS_STRUCT_Message         *p_Message,
  OS_TYPE_MessageSize       *p_ReturnMessageSize );

void  OS_Message_Post
( OS_STRUCT_Message         *p_Message,
  void                      *p_void,
  OS_TYPE_MessageSize       p_ReturnMessageSize );

void  *OS_TaskMessage_Pend
( OS_TYPE_MessageSize      *p_ReturnMessageSize );

void  OS_TaskMessage_Post
( OS_STRUCT_TDB             *p_TDB,
  void                      *p_void,
  OS_TYPE_MessageSize       p_ReturnMessageSize );


#endif /* OS_MESSAGE_H_ */
