/*
 * bsp_usart.h
 *
 *  Created on: 2014年3月6日
 *      Author: Rare
 */

#ifndef BSP_USART_H_
#define BSP_USART_H_

#include "bsp.h"


#define  BSP_DEF_CONFIG_USART1_RECEIVE_BUFFER_SIZE   512

int8  BSP_usart1_fputc( uint8    ch );
int8  BSP_USART1_init( uint32  p_bound_rate );

/*
************************************************************************************************************************
*
************************************************************************************************************************
*/
#define  BSP_DEF_CONFIG_USART2_RECEIVE_BUFFER_SIZE   512

int8  BSP_usart2_fputc( uint8    ch );
int8  BSP_USART2_init( uint32  p_bound_rate );


#endif /* BSP_USART_H_ */
