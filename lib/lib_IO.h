/*
 * lib_IO.h
 *
 *  Created on: 2014年6月29日
 *      Author: HGX
 */

#ifndef LIB_IO_H_
#define LIB_IO_H_

#include "lib.h"



typedef uint8 *va_list;
// The marco NATIVE_INT indicates the CPU's native datawidth.
#define _NATIVE_INT        uint32
// The macro _INTSIZEOF can get the storage size of the type we want
// and this size must be an integral multiple of the NATIVE_INT
#define _INTSIZEOF(type)    ( (sizeof(type) + sizeof(_NATIVE_INT) - 1) & ~(sizeof(_NATIVE_INT) - 1) )
// The ap is a point that has type of va_list
// and the v is the first argument of the variable argument lists.
#define va_start(ap,v)      ( ap = (va_list)&v + _INTSIZEOF(v) )
#define va_arg(ap,type)     ( *(type *)((ap += _INTSIZEOF(type)) - _INTSIZEOF(type)) )
#define va_end(ap)          ( ap = (va_list)0 )



uint32  LIB_PrintfFormat
( uint8         *p_WriteBuffer,
  uint32        p_WriteBufferSize,
  const uint8   *p_Format,
  va_list       p_Arguments );



#endif /* LIB_IO_H_ */
