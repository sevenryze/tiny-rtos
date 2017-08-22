/*
 * lib_IO.c
 *
 *  Created on: 2014年6月29日
 *      Author: HGX
 */

#include "lib_IO.h"


static uint8  *LIB_InternalNumberFormat
( uint8     *p_WriteBuffer,
  uint8     *p_WriteBufferEnd,
  int32     p_Number )
{
    uint8 tmp[30];
    static const uint8 large_digits[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    if( (int64)p_Number < 0 )
    {
        p_Number = (uint32)(- p_Number);

        if( p_WriteBuffer <= p_WriteBufferEnd )
            *p_WriteBuffer = '-';
        p_WriteBuffer++;
    }
    p_Number = (uint32)p_Number;

    uint8 base = 16;

    int8 i = 0;
    if( p_Number == 0 )
        tmp[i++]='0';
    else
    {
        uint8 mask = base - 1;
        uint8 shift = 4;
        do {
            tmp[i++] = large_digits[((uint8)p_Number) & mask];
            p_Number >>= shift;
        } while( p_Number );
    }

    while( i-- > 0 )
    {
        if( p_WriteBuffer <= p_WriteBufferEnd )
            *p_WriteBuffer = tmp[i];
        p_WriteBuffer++;
    }

    return p_WriteBuffer;
}


uint32  LIB_PrintfFormat
( uint8         *p_WriteBuffer,
  uint32        p_WriteBufferSize,
  const uint8   *p_Format,
  va_list       p_Arguments )
{
    uint8 *t_WriteBufferIndex  = p_WriteBuffer;
    uint8 *t_WriteBufferEnd    = p_WriteBuffer + p_WriteBufferSize - 1;

    for( ; *p_Format; ++p_Format )
    {
        if( *p_Format != '%' )
        {
            if( t_WriteBufferIndex <= t_WriteBufferEnd )
                *t_WriteBufferIndex = *p_Format;
            t_WriteBufferIndex++;
            continue;
        }

        p_Format++; /* This skips the first '%' */

        const uint8 *s;
        uint8   c;
        switch( *p_Format )
        {
            case 'c':
                c = (uint8)va_arg( p_Arguments, uint32 );
                if( t_WriteBufferIndex <= t_WriteBufferEnd )
                    *t_WriteBufferIndex = c;
                t_WriteBufferIndex++;
                continue;

            case 's':
                s = va_arg( p_Arguments, uint8 * );
                for( ; *s != '\0'; s++ )
                {
                    if( t_WriteBufferIndex <= t_WriteBufferEnd )
                        *t_WriteBufferIndex = *s;
                    t_WriteBufferIndex++;
                }
                continue;

            case '%':
                if( t_WriteBufferIndex <= t_WriteBufferEnd )
                    *t_WriteBufferIndex = '%';
                t_WriteBufferIndex++;
                continue;

            case 'n':
                break;

            default:
                if( t_WriteBufferIndex <= t_WriteBufferEnd )
                    *t_WriteBufferIndex = '%';
                t_WriteBufferIndex++;
                if( *p_Format ) // Char that we can't interpret.
                {
                    if( t_WriteBufferIndex <= t_WriteBufferEnd )
                        *t_WriteBufferIndex = *p_Format;
                    t_WriteBufferIndex++;
                }
                else
                {
                    p_Format--;
                }
                continue;
        }

        int32 t_Number = va_arg(p_Arguments, int32);

        t_WriteBufferIndex = LIB_InternalNumberFormat( t_WriteBufferIndex, t_WriteBufferEnd, t_Number );
    }

    if( t_WriteBufferIndex <= t_WriteBufferEnd )
        *t_WriteBufferIndex = '\0';
    else if( p_WriteBufferSize > 0 )
        *t_WriteBufferEnd = '\0';

    return t_WriteBufferIndex-p_WriteBuffer;
}


