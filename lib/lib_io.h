/*
 * lib_io.h
 *
 *  Created on: 2014年2月22日
 *      Author: Rare
 */

#ifndef LIB_IO_H_
#define LIB_IO_H_

#include "lib.h"

int32  OS_printf( uint32 p_out_target, const char *p_format, ... );

#define     LIB_DEF_IO_OUT_USART1       ((uint32)1)
#define     LIB_DEF_IO_OUT_USART2       ((uint32)2)
#define     LIB_DEF_IO_OUT_USART3       ((uint32)4)
#define     LIB_DEF_IO_OUT_USART4       ((uint32)8)
#define     IS_LIB_DEF_IO_OUT( ID )     ((((ID) & 0xFFFFFFF0) == 0x00) && ((ID) != 0x00))



#endif /* LIB_IO_H_ */
