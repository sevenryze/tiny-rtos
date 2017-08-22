/*
 * bsp_EEPROM.h
 *
 *  Created on: 2014年3月5日
 *      Author: Rare
 */

#ifndef BSP_EEPROM_H_
#define BSP_EEPROM_H_

#include "bsp.h"

void  AT24CXX_init( void );
uint8  AT24CXX_read_one_byte( uint16    p_read_address );
void  AT24CXX_write_one_byte( uint16    p_write_address, uint8  p_data );
uint8  AT24CXX_check( void );

void  AT24CXX_write_len_byte( uint16    p_write_address, uint32     p_data, uint8   Len );
uint32  AT24CXX_read_len_byte( uint16   p_read_address, uint8   Len );
void  AT24CXX_read( uint16   p_read_address, uint8   *p_buffer, uint16    p_read_num );
void  AT24CXX_write( uint16     p_write_address, uint8  *p_buffer, uint16   p_write_num );






#endif /* BSP_EEPROM_H_ */
