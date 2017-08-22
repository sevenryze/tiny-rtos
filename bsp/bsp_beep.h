/*
 * bsp_beep.h
 *
 *  Created on: 2014年3月6日
 *      Author: Rare
 */

#ifndef BSP_BEEP_H_
#define BSP_BEEP_H_

#include "bsp.h"

/*
************************************************************************************************************************
*
************************************************************************************************************************
*/
#define BSP_BEEP_STATUS_OPEN     1 //represent that we have the HIGH voltage
#define BSP_BEEP_STATUS_CLOSE    0 //represent that we have the LOW voltage

void  BSP_beep_init( void );
void  BSP_beep( uint8  p_status );



#endif /* BSP_BEEP_H_ */
