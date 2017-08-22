/*
 * OS_Time.h
 *
 *  Created on: 2014年3月27日
 *      Author: OH
 */

#ifndef OS_TIME_H_
#define OS_TIME_H_

#include "OS.h"

void  OS_Delay_us
( OS_TYPE_Tick  p_Num );

void  OS_Delay_tick
( OS_TYPE_Tick  p_Tick );

void  OS_Delay
( uint8     p_hour,
  uint8     p_minute,
  uint8     p_second,
  uint16    p_milli );



#endif /* OS_TIME_H_ */
