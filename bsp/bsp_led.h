/*
 * bsp_led.h
 *
 *  Created on: 2014年3月6日
 *      Author: Rare
 */

#ifndef BSP_LED_H_
#define BSP_LED_H_

#include "bsp.h"

/*
************************************************************************************************************************
*
************************************************************************************************************************
*/
#define BSP_LED_1       ( (uint32)0x1 )        //represent the LED 1
#define BSP_LED_2       ( (uint32)0x2 )        //represent the LED 2
#define BSP_LED_3       ( (uint32)0x3 )        //represent the LED 3
#define IS_LED_ID(ID)   ((((ID) & 0xFFFFFFFC) == 0x00) && ((ID) != 0x00))

#define BSP_LED_STATUS_OPEN      1   //represent that we have the LOW voltage
#define BSP_LED_STATUS_CLOSE     0   //represent that we have the HIGH voltage

void  BSP_LED_init( void );
void  BSP_LED( uint8    p_LED_number, uint8   p_status );
void BSP_LED_reverse(uint8 p_LED_num);



#endif /* BSP_LED_H_ */
